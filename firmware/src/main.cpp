/* -----------------------------------------------------------------------------
 * Edge-AI Predictive Maintenance Node -- Raspberry Pi Pico W
 *
 * A scaled-down Industry 4.0 condition-monitoring node. It watches an induction
 * motor's vibration, load current and bearing temperature, runs a random forest
 * on-device to classify the machine's health, estimates a degradation index as
 * a proxy for Remaining Useful Life, drives local annunciation, and publishes
 * structured telemetry.
 *
 * THE REAL-TIME PIPELINE, once per 256 ms window:
 *
 *   1. sample      512 accelerometer samples at 2 kHz into a ring buffer,
 *                  accumulating current and temperature alongside
 *   2. features    project 3 axes -> 1 signal, then 11 features
 *                  (time domain + FFT band energies + current + temp)
 *   3. infer       15-tree random forest, compiled to if/else in model_rf.h
 *   4. score       transparent weighted degradation index, 0..100%
 *   5. annunciate  LED, buzzer, OLED
 *   6. publish     one JSON telemetry line over serial (and MQTT when built
 *                  with ENABLE_MQTT)
 *
 * Steps 2-4 are the on-device ML. Nothing leaves the board to make the
 * decision; the network path carries results, not raw data. That is the entire
 * argument for edge inference: a 2 kHz triaxial stream is ~24 kB/s per node and
 * would swamp a plant network, while the verdict is a few hundred bytes every
 * quarter second.
 * -------------------------------------------------------------------------- */

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "feature_spec.h"
#include "features.h"
#include "model_rf.h"
#include "pins.h"
#include "vibration_source.h"

#ifdef ENABLE_SELFTEST
#include "selftest_vectors.h"
#endif

/* -----------------------------------------------------------------------------
 * THE GUARD THAT MAKES THIS WHOLE PROJECT SAFE TO CHANGE.
 *
 * model_rf.h records the fingerprint of the feature spec it was trained
 * against. feature_spec.h records the fingerprint of the spec this firmware
 * computes. If you alter the sample rate, window length, FFT band edges, PCA
 * iteration count or feature order and forget to retrain and re-export, these
 * two stop matching and the build fails right here.
 *
 * Without it the failure is invisible: the firmware happily feeds feature 7 to
 * a tree that was trained on a different feature 7, and the node reports
 * confident nonsense about a machine someone is relying on.
 * -------------------------------------------------------------------------- */
static_assert(MODEL_FEATURE_SPEC_HASH == FEATURE_SPEC_HASH,
              "model_rf.h was trained against a DIFFERENT feature spec than this "
              "firmware computes. Re-run: py ml/train_random_forest.py && "
              "py ml/export_to_c.py");

/* --- sampling --------------------------------------------------------------- */
#define SAMPLE_PERIOD_US   ((uint32_t)(1000000.0f / FS_HZ))   /* 500 us at 2 kHz */

/* Fixed-size ring buffers, 6 kB total. Nothing in the loop allocates: a heap
 * stall on a node whose job is to not miss a failure would be indefensible. */
static float buf_ax[WINDOW_N];
static float buf_ay[WINDOW_N];
static float buf_az[WINDOW_N];

static float features[N_FEATURES];
static uint8_t votes[N_CLASSES];

static Adafruit_SSD1306 oled(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
static bool oled_present = false;

/* Smoothed degradation reading, so the bar glides rather than flickering. */
static float degradation_ema = 0.0f;
static bool  ema_primed = false;

static uint32_t window_count = 0;

/* Short labels: "EARLY_FAULT" at text size 2 is 132 px, wider than the panel. */
static const char *const SHORT_NAMES[N_CLASSES] = { "HEALTHY", "EARLY", "CRITICAL" };

/* -----------------------------------------------------------------------------
 * Sensor conversion
 * -------------------------------------------------------------------------- */
static inline float read_current_amps(void)
{
    /* A real CT (say an SCT-013) feeds a burden resistor and a conditioning
     * stage that outputs an already-rectified level proportional to RMS
     * current. The pot stands in for that conditioned output, which is why we
     * can treat the reading as amps directly instead of sampling a 50 Hz
     * waveform and computing RMS ourselves. */
    return ((float)analogRead(PIN_CURRENT_ADC) / ADC_MAX_COUNTS) * CURRENT_MAX_A;
}

static inline float read_temp_celsius(void)
{
    float frac = (float)analogRead(PIN_TEMP_ADC) / ADC_MAX_COUNTS;
    return TEMP_MIN_C + frac * (TEMP_MAX_C - TEMP_MIN_C);
}

/* -----------------------------------------------------------------------------
 * Annunciation
 * -------------------------------------------------------------------------- */
static void drive_outputs(uint8_t state)
{
    digitalWrite(PIN_LED_HEALTHY,  state == 0 ? HIGH : LOW);
    digitalWrite(PIN_LED_EARLY,    state == 1 ? HIGH : LOW);
    digitalWrite(PIN_LED_CRITICAL, state == 2 ? HIGH : LOW);

    /* A short chirp per window rather than a continuous tone: it still reads as
     * an alarm, but it does not mask the operator's ability to hear the machine
     * itself, which is a real diagnostic instrument. */
    if (state == 2) {
        tone(PIN_BUZZER, 2200, 120);
    } else {
        noTone(PIN_BUZZER);
    }
}

static void render_display(uint8_t state, float confidence, float degradation)
{
    if (!oled_present) {
        return;
    }

    oled.clearDisplay();

    /* header ------------------------------------------------------------- */
    oled.setTextSize(1);
    oled.setTextColor(SSD1306_WHITE);
    oled.setCursor(0, 0);
    oled.print(F("PdM EDGE"));
    oled.setCursor(74, 0);
    oled.print(votes[state]);
    oled.print('/');
    oled.print(RF_N_TREES);
    oled.drawFastHLine(0, 9, OLED_WIDTH, SSD1306_WHITE);

    /* state -------------------------------------------------------------- */
    oled.setTextSize(2);
    oled.setCursor(0, 13);
    oled.print(SHORT_NAMES[state]);

    /* degradation bar ----------------------------------------------------- */
    int filled = (int)(degradation + 0.5f);
    if (filled < 0) { filled = 0; } else if (filled > 100) { filled = 100; }
    oled.drawRect(0, 31, 100, 9, SSD1306_WHITE);
    oled.fillRect(1, 32, (filled * 98) / 100, 7, SSD1306_WHITE);
    oled.setTextSize(1);
    oled.setCursor(104, 32);
    oled.print(filled);
    oled.print('%');

    /* live features -------------------------------------------------------
     * Chosen for what an analyst actually reads: impulsiveness, severity, the
     * dominant line, and the bearing-defect band share. */
    oled.setCursor(0, 44);
    oled.print(F("K"));
    oled.print(features[FEAT_VIB_KURTOSIS], 1);
    oled.print(F(" C"));
    oled.print(features[FEAT_VIB_CREST], 1);
    oled.print(F(" "));
    oled.print((int)features[FEAT_DOM_FREQ_HZ]);
    oled.print(F("Hz"));

    oled.setCursor(0, 53);
    oled.print(F("g"));
    oled.print(features[FEAT_VIB_STD], 3);
    oled.print(F(" "));
    oled.print(features[FEAT_CURRENT_RMS_A], 1);
    oled.print(F("A "));
    oled.print((int)features[FEAT_TEMP_C]);
    oled.print(F("C"));

    oled.display();
}

/* -----------------------------------------------------------------------------
 * Telemetry
 *
 * One JSON object per window on the serial line. This is deliberately shaped
 * like what a real node would publish northbound to SCADA or a cloud broker:
 * an identity, a timestamp, the verdict with its confidence, the derived
 * severity, and the full feature vector so the historian can re-derive trends
 * or retrain without ever needing the raw waveform.
 *
 * Built with Serial.print rather than snprintf("%f") on purpose -- float
 * formatting in newlib-nano is a linker flag away from silently printing
 * nothing, and a telemetry path that fails quietly is worse than none.
 * -------------------------------------------------------------------------- */
static void publish_telemetry(uint8_t state, float confidence, float degradation)
{
    Serial.print(F("{\"node\":\"motor-01\",\"t_ms\":"));
    Serial.print(millis());
    Serial.print(F(",\"seq\":"));
    Serial.print(window_count);
    Serial.print(F(",\"state\":\""));
    Serial.print(CLASS_NAMES[state]);
    Serial.print(F("\",\"class\":"));
    Serial.print(state);
    Serial.print(F(",\"confidence\":"));
    Serial.print(confidence, 3);
    Serial.print(F(",\"votes\":["));
    for (uint8_t i = 0; i < N_CLASSES; i++) {
        if (i) {
            Serial.print(',');
        }
        Serial.print(votes[i]);
    }
    Serial.print(F("],\"degradation_pct\":"));
    Serial.print(degradation, 1);
    Serial.print(F(",\"features\":{"));
    for (uint8_t i = 0; i < N_FEATURES; i++) {
        if (i) {
            Serial.print(',');
        }
        Serial.print('"');
        Serial.print(FEATURE_NAMES[i]);
        Serial.print(F("\":"));
        Serial.print(features[i], 5);
    }
    Serial.print(F("}}"));
    Serial.println();
}

/* -----------------------------------------------------------------------------
 * Cloud telemetry path (architecture made visible, not built by default)
 *
 * Wokwi's WiFi gateway reaches only a limited set of hosts and adds latency
 * that would stall the sampling loop, so this is #ifdef'd out for the demo.
 * On real hardware, define ENABLE_MQTT and add `knolleary/PubSubClient` to
 * lib_deps.
 *
 * Note the shape of what gets published: the same compact verdict the OLED
 * shows, NOT the waveform. One node streaming raw triaxial data at 2 kHz is
 * about 24 kB/s; a hundred nodes would be 2.4 MB/s of plant network for data
 * that is 99.9% uninteresting. Publishing the inference result instead is the
 * whole economic case for edge ML, and it is also why the degradation index
 * and confidence travel with the verdict -- the cloud needs enough context to
 * trend severity without ever seeing a sample.
 * -------------------------------------------------------------------------- */
#ifdef ENABLE_MQTT
#include <WiFi.h>
#include <PubSubClient.h>

static WiFiClient wifi_client;
static PubSubClient mqtt(wifi_client);

static void mqtt_ensure_connected(void)
{
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        return;                      /* non-blocking: never stall the sampler */
    }
    if (!mqtt.connected()) {
        mqtt.setServer(MQTT_BROKER, MQTT_PORT);
        mqtt.connect("pdm-motor-01");
    }
}

static void mqtt_publish(uint8_t state, float degradation)
{
    if (!mqtt.connected()) {
        return;
    }
    char payload[192];
    snprintf(payload, sizeof(payload),
             "{\"node\":\"motor-01\",\"state\":\"%s\",\"degradation_pct\":%d,"
             "\"kurtosis_x100\":%d,\"current_a_x10\":%d,\"temp_c\":%d}",
             CLASS_NAMES[state], (int)degradation,
             (int)(features[FEAT_VIB_KURTOSIS] * 100.0f),
             (int)(features[FEAT_CURRENT_RMS_A] * 10.0f),
             (int)features[FEAT_TEMP_C]);
    mqtt.publish("plant/line3/motor-01/health", payload);
}
#endif

/* -----------------------------------------------------------------------------
 * On-target self-test -- verifies BOTH halves of the Python/C contract, on the
 * actual target, in float32, with the actual toolchain's libm.
 *
 *   A. rf_predict() reproduces scikit-learn's class on captured feature vectors
 *      -> the MODEL survived the conversion to C.
 *
 *   B. features_compute() reproduces extract_features.py on raw windows
 *      -> the FEATURE PIPELINE agrees: FFT, Hann window, PCA projection,
 *         kurtosis convention, band edges, the lot.
 *
 * (B) is the check that cannot be convincingly done anywhere else. Compiling
 * features.cpp on a laptop would test a different compiler, a different libm
 * and (on x86) frequently different float behaviour. The question worth
 * answering is whether the numbers match ON THE PICO, so the test runs there.
 * -------------------------------------------------------------------------- */
#ifdef ENABLE_SELFTEST
static void run_selftest(void)
{
    /* --- A: model parity --------------------------------------------------- */
    uint16_t pass = 0;
    uint8_t local_votes[N_CLASSES];

    for (uint16_t i = 0; i < N_SELFTEST_VECTORS; i++) {
        if (rf_predict(SELFTEST_X[i], local_votes) == SELFTEST_Y[i]) {
            pass++;
        }
    }

    Serial.print(F("# SELFTEST A rf_predict      : "));
    Serial.print(pass);
    Serial.print('/');
    Serial.print(N_SELFTEST_VECTORS);
    Serial.println(pass == N_SELFTEST_VECTORS ? F("  PASS") : F("  FAIL"));

    /* --- B: feature pipeline parity ---------------------------------------- */
    /* 0.5% relative tolerance absorbs float32 vs float64 and the difference
     * between this radix-2 FFT and numpy's. Anything structurally wrong -- a
     * band edge off by one bin, the Fisher kurtosis convention, a mis-scaled
     * Hann window -- lands orders of magnitude outside it. */
    const float tolerance = 0.005f;
    bool all_ok = true;
    float worst_rel = 0.0f;
    uint8_t worst_feature = 0;

    for (uint8_t w = 0; w < N_RAW_WINDOWS; w++) {
        float got[N_FEATURES];
        features_compute(RAW_AX[w], RAW_AY[w], RAW_AZ[w],
                         RAW_CUR_SUMSQ[w], RAW_TEMP_SUM[w], got);

        for (uint8_t i = 0; i < N_FEATURES; i++) {
            float expect = RAW_EXPECTED[w][i];
            float scale = fabsf(expect);
            if (scale < 1.0f) {
                scale = 1.0f;          /* absolute floor for near-zero features */
            }
            float rel = fabsf(got[i] - expect) / scale;
            if (rel > worst_rel) {
                worst_rel = rel;
                worst_feature = i;
            }
            if (rel > tolerance) {
                all_ok = false;
                Serial.print(F("#   MISMATCH win "));
                Serial.print(w);
                Serial.print(' ');
                Serial.print(FEATURE_NAMES[i]);
                Serial.print(F(": got "));
                Serial.print(got[i], 6);
                Serial.print(F(" expected "));
                Serial.println(expect, 6);
            }
        }
    }

    Serial.print(F("# SELFTEST B features_compute: "));
    Serial.print(N_RAW_WINDOWS);
    Serial.print(F(" windows, worst rel err "));
    Serial.print(worst_rel, 6);
    Serial.print(F(" on "));
    Serial.print(FEATURE_NAMES[worst_feature]);
    Serial.println(all_ok ? F("  PASS") : F("  FAIL"));
}
#endif

/* -------------------------------------------------------------------------- */
void setup(void)
{
    Serial.begin(115200);

    pinMode(PIN_LED_HEALTHY, OUTPUT);
    pinMode(PIN_LED_EARLY, OUTPUT);
    pinMode(PIN_LED_CRITICAL, OUTPUT);
    pinMode(PIN_BUZZER, OUTPUT);

    /* 12-bit ADC. ADC_MAX_COUNTS in feature_spec.h assumes this; leaving the
     * Arduino 10-bit default would scale current and temperature by 4x. */
    analogReadResolution(12);

    /* Lamp test: every indicator, briefly, so a dead LED is obvious at startup
     * rather than being mistaken for "nothing wrong". */
    digitalWrite(PIN_LED_HEALTHY, HIGH);
    digitalWrite(PIN_LED_EARLY, HIGH);
    digitalWrite(PIN_LED_CRITICAL, HIGH);
    delay(400);
    digitalWrite(PIN_LED_HEALTHY, LOW);
    digitalWrite(PIN_LED_EARLY, LOW);
    digitalWrite(PIN_LED_CRITICAL, LOW);

    features_init();
    bool mpu_ok = vib_source_begin();

    /* periphBegin=false: vib_source_begin() has already set up Wire on GP4/GP5
     * at 400 kHz. Letting the display library call Wire.begin() a second time
     * would re-initialise the bus with default pins and default speed. It
     * happens to be harmless here because GP4/GP5 ARE arduino-pico's I2C0
     * defaults, but depending on that coincidence is how a project breaks the
     * day someone moves the bus. (reset=true is a no-op: the constructor was
     * given -1 for the reset pin.) */
    oled_present = oled.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR, true, false);
    if (oled_present) {
        oled.clearDisplay();
        oled.setTextColor(SSD1306_WHITE);
        oled.setTextSize(1);
        oled.setCursor(0, 0);
        oled.println(F("PdM EDGE NODE"));
        oled.println(F("motor-01"));
        oled.print(F("MPU6050: "));
        oled.println(mpu_ok ? F("ok") : F("MISSING"));
        oled.print(F("model: "));
        oled.print(RF_N_TREES);
        oled.println(F(" trees"));
        oled.println(F("2 kHz / 512 pt FFT"));
        oled.display();
    }

    Serial.println();
    Serial.println(F("# Edge-AI Predictive Maintenance Node"));
    Serial.print(F("# sample rate "));
    Serial.print(FS_HZ, 0);
    Serial.print(F(" Hz, window "));
    Serial.print(WINDOW_N);
    Serial.print(F(" samples, "));
    Serial.print(RF_N_TREES);
    Serial.println(F(" trees"));
    Serial.print(F("# MPU6050: "));
    Serial.println(mpu_ok ? F("detected") : F("NOT FOUND"));
#if VIB_SOURCE_WOKWI_SIM
    Serial.println(F("# vibration source: WOKWI SIM "
                     "(MPU6050 X/Y slider = fault severity 0..1)"));
#else
    Serial.println(F("# vibration source: MPU6050 live stream"));
#endif

#ifdef ENABLE_SELFTEST
    run_selftest();
#endif

    Serial.println(F("# telemetry: one JSON object per window"));
    delay(1200);
}

void loop(void)
{
    /* --- 1. acquire one window ------------------------------------------- */
    vib_source_update_operating_point();

    float current_sumsq = 0.0f;
    float temp_sum = 0.0f;
    uint32_t next_us = micros();

    for (int i = 0; i < WINDOW_N; i++) {
        /* Signed comparison handles micros() rollover correctly. */
        while ((int32_t)(micros() - next_us) < 0) {
            /* spin */
        }
        next_us += SAMPLE_PERIOD_US;

        vib_source_next(&buf_ax[i], &buf_ay[i], &buf_az[i]);

        float amps = read_current_amps();
        current_sumsq += amps * amps;
        temp_sum += read_temp_celsius();
    }

    /* --- 2. features ------------------------------------------------------ */
    features_compute(buf_ax, buf_ay, buf_az, current_sumsq, temp_sum, features);

    /* --- 3. inference ----------------------------------------------------- */
    uint8_t state = rf_predict(features, votes);
    float confidence = (float)votes[state] / (float)RF_N_TREES;

    /* --- 4. degradation index, lightly smoothed --------------------------- */
    float raw_degradation = degradation_index(features, state);
    if (!ema_primed) {
        degradation_ema = raw_degradation;
        ema_primed = true;
    } else {
        degradation_ema = DEGRADATION_EMA_ALPHA * raw_degradation
                        + (1.0f - DEGRADATION_EMA_ALPHA) * degradation_ema;
    }

    window_count++;

    /* --- 5. annunciate ---------------------------------------------------- */
    drive_outputs(state);
    render_display(state, confidence, degradation_ema);

    /* --- 6. publish -------------------------------------------------------
     * Display and telemetry run AFTER the window closes, never during
     * sampling. A full SSD1306 frame is 1 kB over I2C, about 25 ms at 400 kHz,
     * which would blow the 500 us sample budget many times over. Each window
     * is sampled contiguously and the housekeeping happens in the gap, so the
     * spectrum is never distorted by a display update -- it just means the
     * effective duty cycle is around 90% rather than 100%. */
    publish_telemetry(state, confidence, degradation_ema);

#ifdef ENABLE_MQTT
    mqtt_ensure_connected();
    mqtt.loop();
    mqtt_publish(state, degradation_ema);
#endif
}
