/* -----------------------------------------------------------------------------
 * features.h -- on-device feature extraction.
 *
 * This is the C half of the Python/C contract. features.cpp is a line-for-line
 * translation of ml/extract_features.py; read them side by side.
 * -------------------------------------------------------------------------- */
#ifndef FEATURES_H
#define FEATURES_H

#include "feature_spec.h"

/* Build the Hann and FFT twiddle tables. Call once from setup(). */
void features_init(void);

/* Turn one filled window into the N_FEATURES vector.
 *
 *   ax, ay, az      WINDOW_N raw accelerometer samples each, in g
 *   current_sumsq   running sum of (amps * amps) over the window
 *   temp_sum        running sum of degrees C over the window
 *   out             receives N_FEATURES floats in FEATURE_NAMES order
 *
 * Current and temperature arrive as accumulators rather than arrays: the
 * firmware only ever needs their RMS and mean, so there is no reason to spend
 * 4 kB of RAM storing samples we would immediately reduce. The arithmetic is
 * identical to the Python, which does keep the arrays.
 */
void features_compute(const float *ax, const float *ay, const float *az,
                      float current_sumsq, float temp_sum, float *out);

/* 0..100 heuristic severity score. Transparent by design -- see feature_spec.py
 * section 7 for why this is hand-written rather than learned. */
float degradation_index(const float *features, uint8_t predicted_class);

#endif /* FEATURES_H */
