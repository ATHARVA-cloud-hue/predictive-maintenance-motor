/* -----------------------------------------------------------------------------
 * pins.h -- Raspberry Pi Pico W pin map.
 *
 *   function                 GPIO   notes
 *   -----------------------  -----  ---------------------------------------------
 *   I2C0 SDA                 GP4    shared bus: MPU6050 (0x68) + SSD1306 (0x3C)
 *   I2C0 SCL                 GP5    400 kHz fast mode
 *   current transformer      GP26   ADC0, potentiometer standing in for a CT
 *   bearing temperature      GP27   ADC1, potentiometer standing in for an RTD
 *   LED green  (HEALTHY)     GP15   through 220R
 *   LED yellow (EARLY_FAULT) GP14   through 220R
 *   LED red    (CRITICAL)    GP13   through 220R
 *   buzzer                   GP12   sounds on CRITICAL only
 *
 * GP23, GP24, GP25 and GP29 are deliberately unused: on the Pico W they are
 * wired to the CYW43439 wireless chip (and GP29 doubles as VSYS sense), not to
 * the header. Driving them breaks WiFi, which the MQTT telemetry path needs.
 * This is the one pin-map trap specific to the W variant -- on a plain Pico,
 * GP25 is the on-board LED.
 *
 * Both ADC pins are on the RP2040's ADC block (GP26-GP28 only). We call
 * analogReadResolution(12) in setup(), so readings are 0..4095 rather than the
 * Arduino-default 0..1023; ADC_MAX_COUNTS in feature_spec.h must agree.
 * -------------------------------------------------------------------------- */
#ifndef PINS_H
#define PINS_H

#define I2C_SDA_PIN        4
#define I2C_SCL_PIN        5

#define PIN_CURRENT_ADC    26
#define PIN_TEMP_ADC       27

#define PIN_LED_HEALTHY    15
#define PIN_LED_EARLY      14
#define PIN_LED_CRITICAL   13

#define PIN_BUZZER         12

#define OLED_I2C_ADDR      0x3C
#define OLED_WIDTH         128
#define OLED_HEIGHT        64

#endif /* PINS_H */
