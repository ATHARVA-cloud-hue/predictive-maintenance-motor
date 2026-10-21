/* -----------------------------------------------------------------------------
 * features.cpp -- the C translation of ml/extract_features.py.
 *
 * PARITY IS THE WHOLE POINT OF THIS FILE. The model in model_rf.h learned split
 * thresholds against features computed by the Python reference. If this file
 * computes them even slightly differently -- a Hann window with the wrong
 * denominator, kurtosis in the excess convention, a band edge off by one bin --
 * nothing crashes and nothing logs an error. The classifier simply becomes
 * wrong, confidently.
 *
 * So every function below names the Python function it mirrors, and every
 * constant comes from feature_spec.h, which is generated from the same Python
 * module that the training pipeline imports.
 *
 * WHY A HAND-WRITTEN FFT INSTEAD OF arduinoFFT
 * --------------------------------------------
 * A library would be one more thing whose internals must match numpy's. Library
 * versions differ in windowing conventions, in whether they normalise output,
 * and in their API across major releases. A 40-line radix-2 Cooley-Tukey is
 * small enough to read, has no version to drift, and -- because we only ever use
 * RATIOS of bin powers and the ARGMAX bin -- any constant scale factor it might
 * apply cancels out entirely. That removes a whole class of parity bug for the
 * cost of forty lines.
 * -------------------------------------------------------------------------- */

#include "features.h"

#include <math.h>
#include <string.h>

/* --- static working memory -------------------------------------------------
 * Everything is fixed-size and file-scope. Nothing in the hot loop allocates:
 * a heap fragmentation stall on a node that is supposed to be watching a
 * machine for impending failure would be an embarrassing way to lose data.
 *
 *   hann        WINDOW_N floats   2 kB
 *   twiddle     WINDOW_N floats   2 kB  (re + im, N/2 each)
 *   fft_re/im   2 x WINDOW_N      4 kB
 *                                 ------
 *                                 8 kB, plus 6 kB of ring buffers in main.cpp
 */
static float hann[WINDOW_N];
static float twiddle_re[WINDOW_N / 2];
static float twiddle_im[WINDOW_N / 2];
static float fft_re[WINDOW_N];
static float fft_im[WINDOW_N];

static const float EPS = 1e-20f;

void features_init(void)
{
    /* Hann window. Mirrors the HANN array in extract_features.py exactly,
     * including the (WINDOW_N - 1) denominator -- numpy's np.hanning uses the
     * symmetric form, and using N instead of N-1 here would be a silent
     * mismatch. */
    for (int i = 0; i < WINDOW_N; i++) {
        hann[i] = 0.5f * (1.0f - cosf(2.0f * (float)M_PI * (float)i
                                      / (float)(WINDOW_N - 1)));
    }

    /* Twiddle factors e^(-2*pi*i*j/N) for j = 0 .. N/2-1.
     * Precomputed rather than derived by recurrence: the recurrence form
     * accumulates rounding error across 9 butterfly stages, which would show up
     * as spurious energy in the high band -- directly corrupting a feature. */
    for (int j = 0; j < WINDOW_N / 2; j++) {
        float angle = -2.0f * (float)M_PI * (float)j / (float)WINDOW_N;
        twiddle_re[j] = cosf(angle);
        twiddle_im[j] = sinf(angle);
    }
}

/* --- radix-2 in-place complex FFT, decimation in time ---------------------- */
static void fft_run(float *re, float *im)
{
    /* Bit-reversal permutation. */
    for (int i = 1, j = 0; i < WINDOW_N; i++) {
        int bit = WINDOW_N >> 1;
        for (; j & bit; bit >>= 1) {
            j ^= bit;
        }
        j ^= bit;
        if (i < j) {
            float tr = re[i]; re[i] = re[j]; re[j] = tr;
            float ti = im[i]; im[i] = im[j]; im[j] = ti;
        }
    }

    /* Butterfly stages. */
    for (int len = 2; len <= WINDOW_N; len <<= 1) {
        int half = len >> 1;
        int step = WINDOW_N / len;          /* twiddle table stride */
        for (int base = 0; base < WINDOW_N; base += len) {
            for (int k = 0; k < half; k++) {
                float wr = twiddle_re[k * step];
                float wi = twiddle_im[k * step];
                int a = base + k;
                int b = a + half;

                float xr = re[b] * wr - im[b] * wi;
                float xi = re[b] * wi + im[b] * wr;

                re[b] = re[a] - xr;
                im[b] = im[a] - xi;
                re[a] = re[a] + xr;
                im[a] = im[a] + xi;
            }
        }
    }
}

/* --- mirrors magnitude_rms() ----------------------------------------------- */
static float magnitude_rms(const float *ax, const float *ay, const float *az)
{
    float acc = 0.0f;
    for (int i = 0; i < WINDOW_N; i++) {
        acc += ax[i] * ax[i] + ay[i] * ay[i] + az[i] * az[i];
    }
    return sqrtf(acc / (float)WINDOW_N);
}

/* --- mirrors project_to_principal_axis() -----------------------------------
 * Writes the projected 1-D vibration signal into fft_re.
 *
 * Vector magnitude is deliberately NOT used here: squaring is nonlinear and
 * would report an 89 Hz bearing tone at 178 Hz. We project onto the direction
 * of maximum variance instead, which is linear and orientation-independent.
 * (magnitude_rms above is the one place magnitude IS right, because energy
 * survives squaring even though frequency does not.)
 */
static void project_to_principal_axis(const float *ax, const float *ay,
                                      const float *az)
{
    /* 1. per-axis means -- this is what removes gravity and sensor bias. */
    float mx = 0.0f, my = 0.0f, mz = 0.0f;
    for (int i = 0; i < WINDOW_N; i++) {
        mx += ax[i];
        my += ay[i];
        mz += az[i];
    }
    mx /= (float)WINDOW_N;
    my /= (float)WINDOW_N;
    mz /= (float)WINDOW_N;

    /* 2. 3x3 covariance of the centred cloud. */
    float cxx = 0.0f, cxy = 0.0f, cxz = 0.0f, cyy = 0.0f, cyz = 0.0f, czz = 0.0f;
    for (int i = 0; i < WINDOW_N; i++) {
        float dx = ax[i] - mx;
        float dy = ay[i] - my;
        float dz = az[i] - mz;
        cxx += dx * dx;  cxy += dx * dy;  cxz += dx * dz;
        cyy += dy * dy;  cyz += dy * dz;  czz += dz * dz;
    }
    float inv = 1.0f / (float)WINDOW_N;
    cxx *= inv;  cxy *= inv;  cxz *= inv;
    cyy *= inv;  cyz *= inv;  czz *= inv;

    /* 3. power iteration, FIXED count so Python and C run identical arithmetic. */
    float vx = 0.57735027f, vy = 0.57735027f, vz = 0.57735027f;  /* [1,1,1]/sqrt(3) */
    for (int it = 0; it < PCA_ITERS; it++) {
        float nx = cxx * vx + cxy * vy + cxz * vz;
        float ny = cxy * vx + cyy * vy + cyz * vz;
        float nz = cxz * vx + cyz * vy + czz * vz;
        float norm = sqrtf(nx * nx + ny * ny + nz * nz);
        if (norm > EPS) {
            vx = nx / norm;
            vy = ny / norm;
            vz = nz / norm;
        } else {
            vx = nx; vy = ny; vz = nz;
        }
    }

    /* Deterministic sign. Every feature downstream is sign-invariant, so this
     * only keeps debug traces stable between windows. */
    float axv = fabsf(vx), ayv = fabsf(vy), azv = fabsf(vz);
    float dominant = (axv >= ayv && axv >= azv) ? vx : ((ayv >= azv) ? vy : vz);
    if (dominant < 0.0f) {
        vx = -vx; vy = -vy; vz = -vz;
    }

    /* 4. project. */
    for (int i = 0; i < WINDOW_N; i++) {
        fft_re[i] = (ax[i] - mx) * vx + (ay[i] - my) * vy + (az[i] - mz) * vz;
    }
}

void features_compute(const float *ax, const float *ay, const float *az,
                      float current_sumsq, float temp_sum, float *out)
{
    /* --- 0. DC-coupled overall level, computed on the RAW axes -------------- */
    out[FEAT_VIB_RMS] = magnitude_rms(ax, ay, az);

    /* --- projection into fft_re -------------------------------------------- */
    project_to_principal_axis(ax, ay, az);

    /* --- time domain, on the UNWINDOWED projection -------------------------
     * Order matters: the Hann window is applied only for the FFT. Computing
     * kurtosis on a windowed signal would be wrong -- tapering the ends
     * suppresses exactly the impulses we are trying to detect if one happens to
     * land near a window edge, and it changes the moments systematically. */
    float m2 = 0.0f, m4 = 0.0f, peak = 0.0f;
    for (int i = 0; i < WINDOW_N; i++) {
        float v = fft_re[i];
        float v2 = v * v;
        m2 += v2;
        m4 += v2 * v2;
        float a = fabsf(v);
        if (a > peak) {
            peak = a;
        }
    }
    m2 /= (float)WINDOW_N;
    m4 /= (float)WINDOW_N;

    float std = sqrtf(m2);
    out[FEAT_VIB_PEAK] = peak;
    out[FEAT_VIB_STD] = std;

    /* Pearson (non-excess) kurtosis: 3.0 for Gaussian noise. scipy defaults to
     * the Fisher convention, which subtracts 3 -- training against one and
     * running the other shifts every kurtosis split by exactly 3.0. */
    out[FEAT_VIB_KURTOSIS] = (m2 > EPS) ? (m4 / (m2 * m2)) : 0.0f;
    out[FEAT_VIB_CREST] = (std > EPS) ? (peak / std) : 0.0f;

    /* --- frequency domain --------------------------------------------------- */
    for (int i = 0; i < WINDOW_N; i++) {
        fft_re[i] *= hann[i];
        fft_im[i] = 0.0f;
    }
    fft_run(fft_re, fft_im);

    /* Bin 0 (DC) is excluded: the signal is mean-removed, so it holds only
     * numerical residue. */
    float total = 0.0f, band_low = 0.0f, band_mid = 0.0f, band_high = 0.0f;
    for (int k = SPECTRUM_K_MIN; k <= SPECTRUM_K_MAX; k++) {
        float p = fft_re[k] * fft_re[k] + fft_im[k] * fft_im[k];
        total += p;
        if (k >= BAND_LOW_K0 && k <= BAND_LOW_K1) {
            band_low += p;
        } else if (k >= BAND_MID_K0 && k <= BAND_MID_K1) {
            band_mid += p;
        } else if (k >= BAND_HIGH_K0 && k <= BAND_HIGH_K1) {
            band_high += p;
        }
    }

    if (total <= EPS) {
        out[FEAT_DOM_FREQ_HZ] = 0.0f;
        out[FEAT_BAND_LOW_RATIO] = 0.0f;
        out[FEAT_BAND_MID_RATIO] = 0.0f;
        out[FEAT_BAND_HIGH_RATIO] = 0.0f;
    } else {
        int dom_k = SPECTRUM_K_MIN;
        float best = -1.0f;
        for (int k = SPECTRUM_K_MIN; k <= SPECTRUM_K_MAX; k++) {
            float p = fft_re[k] * fft_re[k] + fft_im[k] * fft_im[k];
            if (p > best) {
                best = p;
                dom_k = k;
            }
        }
        out[FEAT_DOM_FREQ_HZ] = (float)dom_k * BIN_HZ;
        out[FEAT_BAND_LOW_RATIO] = band_low / total;
        out[FEAT_BAND_MID_RATIO] = band_mid / total;
        out[FEAT_BAND_HIGH_RATIO] = band_high / total;
    }

    /* --- auxiliary channels -------------------------------------------------- */
    out[FEAT_CURRENT_RMS_A] = sqrtf(current_sumsq / (float)WINDOW_N);
    out[FEAT_TEMP_C] = temp_sum / (float)WINDOW_N;
}

/* -----------------------------------------------------------------------------
 * Degradation index -- a deliberately simple, auditable severity score.
 *
 * Each contributing feature is normalised 0..1 against a documented
 * healthy->failed range from feature_spec.py, then weighted and summed. The
 * result is floored by the classifier's verdict so the percentage can never
 * contradict the LED that is lit.
 *
 * This is NOT a learned Remaining Useful Life model and is not presented as
 * one. It is an index a maintenance planner can audit: five numbers, five
 * weights, no hidden state. The data-driven RUL path is the LSTM autoencoder in
 * the Tier-2 notebook, which stays offline.
 * -------------------------------------------------------------------------- */
float degradation_index(const float *features, uint8_t predicted_class)
{
    float score = 0.0f;

    for (int i = 0; i < N_DEGRADATION_TERMS; i++) {
        const DegradationTerm *term = &DEGRADATION_TERMS[i];
        float span = term->hi - term->lo;
        float norm = (span > EPS) ? ((features[term->feature] - term->lo) / span)
                                  : 0.0f;
        if (norm < 0.0f) {
            norm = 0.0f;
        } else if (norm > 1.0f) {
            norm = 1.0f;
        }
        score += term->weight * norm;
    }

    score *= 100.0f;

    if (predicted_class < N_CLASSES) {
        float floor_pct = DEGRADATION_CLASS_FLOOR[predicted_class];
        if (score < floor_pct) {
            score = floor_pct;
        }
    }

    if (score > 100.0f) {
        score = 100.0f;
    }
    return score;
}
