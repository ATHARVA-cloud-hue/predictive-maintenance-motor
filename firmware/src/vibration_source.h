/* -----------------------------------------------------------------------------
 * vibration_source.h -- where accelerometer samples come from.
 *
 * TWO BUILD MODES, AND WHY THE SECOND ONE EXISTS
 * ----------------------------------------------
 * VIB_SOURCE_MPU6050   production. Streams real samples from the MPU6050 over
 *                      I2C at FS_HZ. This is the path that runs on hardware.
 *
 * VIB_SOURCE_WOKWI_SIM demo. Wokwi's MPU6050 model is a set of sliders: it
 *                      reports whatever CONSTANT value you drag them to. It
 *                      does not emit a waveform. Run the production path
 *                      against it and every window is a flat line -- variance
 *                      zero, so the PCA projection is undefined, kurtosis is
 *                      0/0, and the spectrum is empty. There is nothing to
 *                      classify.
 *
 *                      So in this mode the slider is reinterpreted as a
 *                      FAULT-SEVERITY KNOB, and the firmware synthesises the
 *                      vibration waveform from the same physical model as
 *                      ml/generate_dataset.py.
 *
 * BE CLEAR ABOUT WHAT THIS DOES AND DOES NOT FAKE.
 * It fakes the SENSOR, because Wokwi's sensor cannot produce a time-varying
 * signal. It does not fake anything downstream: the synthesised samples go
 * through the identical ring buffer, the identical PCA projection, the
 * identical FFT, the identical feature extraction and the identical random
 * forest that real samples would. The TinyML claim -- model trained offline,
 * converted to C, inferring on-device in real time -- is unaffected, because
 * the model genuinely runs on the Pico against genuinely computed features.
 * Swapping to real hardware is a one-flag change with no other edits.
 *
 * The synthesis clock is driven by SAMPLE INDEX, not wall-clock time. If Wokwi
 * runs the RP2040 slower than real time, the window still contains exactly
 * FS_HZ-spaced samples, so the spectrum stays numerically correct and only the
 * refresh rate drops. Deriving phase from micros() would let simulator lag
 * smear the frequencies and corrupt the features.
 * -------------------------------------------------------------------------- */
#ifndef VIBRATION_SOURCE_H
#define VIBRATION_SOURCE_H

#include <stdbool.h>
#include <stdint.h>

/* Bring up I2C and the MPU6050. Returns false if the device never answers. */
bool vib_source_begin(void);

/* Called once per window, before sampling starts.
 *
 * Production: no-op.
 * Sim: reads the MPU6050 sliders and converts them into a severity in 0..1.
 * Doing this once per window rather than per sample keeps the operating point
 * constant across a window, which is what the training data assumes -- each
 * training window came from a machine in one steady state.
 */
void vib_source_update_operating_point(void);

/* One accelerometer sample, in g. */
void vib_source_next(float *ax, float *ay, float *az);

/* Injected severity 0..1. Returns 0 in production builds; used only for the
 * on-screen demo readout so you can see what the slider is asking for. */
float vib_source_severity(void);

/* True if the MPU6050 answered on the bus. */
bool vib_source_present(void);

#endif /* VIBRATION_SOURCE_H */
