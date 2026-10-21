/* -----------------------------------------------------------------------------
 * feature_spec.h -- GENERATED FILE. DO NOT EDIT BY HAND.
 *
 * Produced by ml/feature_spec.py. Every constant here is the same object the
 * Python training pipeline used; editing this file instead of the spec is how you
 * get a model and a firmware that disagree.
 *
 * Regenerate with:   py ml/export_to_c.py
 * -------------------------------------------------------------------------- */
#ifndef FEATURE_SPEC_H
#define FEATURE_SPEC_H

#include <stdint.h>

/* Fingerprint of the whole spec. model_rf.h carries a copy taken at training
 * time; main.cpp static_asserts that the two match. A mismatch means the model
 * in flash was trained against a different feature definition than the one this
 * firmware computes -- a bug that is otherwise completely silent. */
#define FEATURE_SPEC_HASH 0x5BC1E2EDu

/* --- acquisition ---------------------------------------------------------- */
#define FS_HZ            2000.0f
#define WINDOW_N         512
#define BIN_HZ           (2000.0f / (float)WINDOW_N)   /* 3.90625 Hz */
#define N_BINS           257

/* --- spectral bands (inclusive FFT bin indices; bin 0 = DC is excluded) ---- */
#define SPECTRUM_K_MIN   1
#define SPECTRUM_K_MAX   256
#define BAND_LOW_K0      1
#define BAND_LOW_K1      15
#define BAND_MID_K0      16
#define BAND_MID_K1      51
#define BAND_HIGH_K0     52
#define BAND_HIGH_K1     256

/* --- 3-axis -> 1-axis projection ------------------------------------------ */
#define PCA_ITERS        8

/* --- feature vector layout ------------------------------------------------ */
#define N_FEATURES       11
#define N_CLASSES        3

enum FeatureIndex {
    FEAT_VIB_RMS = 0,
    FEAT_VIB_PEAK = 1,
    FEAT_VIB_STD = 2,
    FEAT_VIB_KURTOSIS = 3,
    FEAT_VIB_CREST = 4,
    FEAT_DOM_FREQ_HZ = 5,
    FEAT_BAND_LOW_RATIO = 6,
    FEAT_BAND_MID_RATIO = 7,
    FEAT_BAND_HIGH_RATIO = 8,
    FEAT_CURRENT_RMS_A = 9,
    FEAT_TEMP_C = 10,
};

static const char *const FEATURE_NAMES[N_FEATURES] = {
    "vib_rms",
    "vib_peak",
    "vib_std",
    "vib_kurtosis",
    "vib_crest",
    "dom_freq_hz",
    "band_low_ratio",
    "band_mid_ratio",
    "band_high_ratio",
    "current_rms_a",
    "temp_c"
};

static const char *const CLASS_NAMES[N_CLASSES] = {
    "HEALTHY",
    "EARLY_FAULT",
    "CRITICAL"
};

/* --- sensor scaling ------------------------------------------------------- */
#define CURRENT_MAX_A    30.0f
#define TEMP_MIN_C       20.0f
#define TEMP_MAX_C       120.0f
#define ADC_MAX_COUNTS   4095.0f

/* --- machine model (used by the Wokwi vibration shim) --------------------- */
#define SHAFT_HZ         25.0f
#define BPFO_HZ          89.0f
#define RESONANCE_HZ     600.0f
#define IMPACT_TAU_S     0.0015f

/* --- degradation index ---------------------------------------------------- */
typedef struct {
    uint8_t feature;   /* index into the feature vector */
    float   lo;        /* reading that maps to 0%   */
    float   hi;        /* reading that maps to 100% */
    float   weight;    /* contribution; weights sum to 1.0 */
} DegradationTerm;

#define N_DEGRADATION_TERMS 5

static const DegradationTerm DEGRADATION_TERMS[N_DEGRADATION_TERMS] = {
    { FEAT_VIB_KURTOSIS, 3.0f, 12.0f, 0.3f },
    { FEAT_VIB_STD, 0.02f, 0.35f, 0.25f },
    { FEAT_BAND_HIGH_RATIO, 0.05f, 0.55f, 0.15f },
    { FEAT_CURRENT_RMS_A, 12.0f, 26.0f, 0.15f },
    { FEAT_TEMP_C, 45.0f, 105.0f, 0.15f },
};

static const float DEGRADATION_CLASS_FLOOR[N_CLASSES] = {
    0.0f, 35.0f, 70.0f
};

#define DEGRADATION_EMA_ALPHA 0.25f

#endif /* FEATURE_SPEC_H */
