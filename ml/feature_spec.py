"""
feature_spec.py
===============
THE CONTRACT between the offline Python training pipeline and the on-device C firmware.

Why this file exists
--------------------
A TinyML system fails silently in one specific way: the model is trained on features
computed one way, and the firmware computes them a *slightly* different way. Nothing
crashes. The model just quietly becomes wrong. A swapped feature order, an off-by-one
FFT bin edge, or `scipy`'s excess-kurtosis convention vs. the textbook one are all
enough to turn a 97%-accurate classifier into a coin flip on-device.

So we do not keep Python and C "in sync by discipline". Instead:

  1. Every constant that both sides need lives HERE, exactly once.
  2. `emit_c_header()` GENERATES `firmware/src/feature_spec.h` from these constants.
     The firmware does not hard-code the sample rate, window length, band edges or
     feature order -- it #includes them from generated code.
  3. `spec_hash()` fingerprints the whole spec. The hash is baked into both
     feature_spec.h and model_rf.h, and main.cpp has a static_assert comparing them.
     If you change the spec and forget to retrain + re-export the model, the firmware
     REFUSES TO COMPILE instead of misclassifying.

That static_assert is the single most valuable line in this project.

Run `py feature_spec.py` to regenerate the C header on its own, or just run
`export_to_c.py`, which regenerates both headers together.
"""

from __future__ import annotations

import os

# ---------------------------------------------------------------------------
# 1. SIGNAL ACQUISITION
# ---------------------------------------------------------------------------
# We sample the accelerometer at 2 kHz into a 512-sample window.
#
#   window duration = 512 / 2000 = 256 ms   -> ~4 classifications per second
#   Nyquist limit   = 2000 / 2   = 1000 Hz
#   FFT bin width   = 2000 / 512 = 3.90625 Hz
#
# WHY THE SAMPLE RATE IS THIS HIGH -- the reasoning is all about KURTOSIS:
#
# The machine we model is a 1500 RPM induction motor (25 Hz shaft). Its outer-race
# bearing fault frequency (BPFO) is ~89 Hz and the impact-excited bearing
# resonance sits around 600 Hz.
#
# At 100 Hz sampling (Nyquist 50 Hz) BPFO cannot be represented at all. But the
# binding constraint is sharper than Nyquist. Kurtosis only rises if the impacts
# are genuinely IMPULSIVE -- large, brief, and rare relative to the gaps between
# them. Impacts arrive every 1/89 s = 11.2 ms and ring down with a ~1.5 ms time
# constant, so each one is "live" for roughly 5 ms, a ~13% duty cycle.
#
# To see that as impulsive, the sample interval must be short compared with the
# ring-down. At 2 kHz an impact spans ~3 samples out of the 22 between strikes:
# sharp spikes separated by quiet, which is exactly the heavy-tailed distribution
# kurtosis detects. At 1 kHz the same impact spans 1.5 samples and the resonance
# itself would be above Nyquist; the signal degenerates into noise and kurtosis
# goes flat. We measured this -- at 1 kHz the class medians were 2.3 / 2.9 / 3.1,
# i.e. the project's headline feature carried almost no information.
#
# 2 kHz is still well inside what an MPU6050 can stream (its internal sample rate
# goes to 1 kHz for the DLPF-filtered path and 8 kHz raw).
FS_HZ = 2000.0
WINDOW_N = 512

BIN_HZ = FS_HZ / WINDOW_N          # 3.90625 Hz per FFT bin
N_BINS = WINDOW_N // 2 + 1         # 129 usable bins, k = 0 .. 128

# ---------------------------------------------------------------------------
# 2. SPECTRAL BAND EDGES  (expressed as FFT BIN INDICES, inclusive)
# ---------------------------------------------------------------------------
# These are defined as integer bin indices rather than as frequencies in Hz on
# purpose. If we stored "60 Hz" and each side converted to a bin index with its own
# rounding, Python and C could disagree by one bin. Integers cannot drift.
#
# Bin 0 (DC) is always EXCLUDED: we mean-remove the signal, so bin 0 carries no
# information, only numerical residue.
#
#   LOW   bins  1..15   =   3.9 ..  58.6 Hz
#         Shaft speed (25 Hz) and its 2nd harmonic (50 Hz). This is the
#         IMBALANCE / MISALIGNMENT band. It is elevated on almost any machine
#         and is NOT by itself a bearing-fault indicator.
#
#   MID   bins 16..51   =  62.5 .. 199.2 Hz
#         BPFO (89 Hz) and 2xBPFO (178 Hz), plus their shaft-speed sidebands at
#         89 +/- 25 Hz. This is the BEARING DEFECT band -- energy appearing here
#         is the frequency-domain fingerprint of a spalled race.
#
#   HIGH  bins 52..256  = 203.1 .. 1000.0 Hz
#         The bearing's structural RESONANCE band, centred on ~600 Hz. A
#         metal-on-metal impact is broadband, so it "rings" this resonance.
#         Energy here rises FIRST in an incipient fault, often before the BPFO
#         tone itself is visible -- which is exactly why this feature earns its
#         place alongside the mid band.
SPECTRUM_K_MIN = 1
SPECTRUM_K_MAX = WINDOW_N // 2     # 256

BAND_BINS = {
    "low":  (1, 15),
    "mid":  (16, 51),
    "high": (52, 256),
}

# ---------------------------------------------------------------------------
# 3. THE FEATURE VECTOR  --  ORDER IS LOAD-BEARING
# ---------------------------------------------------------------------------
# The classifier sees an anonymous float[11]. Index 3 means "kurtosis" only because
# both sides agree it does. Reordering this list without retraining is the single
# easiest way to break the system, which is why the spec hash covers it.
FEATURE_NAMES = [
    "vib_rms",          # 0  overall level, DC-coupled (includes the 1 g gravity term)
    "vib_peak",         # 1  max |AC| in the window
    "vib_std",          # 2  AC-coupled RMS == true vibration severity (ISO 10816 sense)
    "vib_kurtosis",     # 3  4th standardised moment, Pearson convention (3.0 = Gaussian)
    "vib_crest",        # 4  peak / std -- impulsiveness, dimensionless
    "dom_freq_hz",      # 5  frequency of the single largest spectral peak
    "band_low_ratio",   # 6  fraction of AC spectral power in the imbalance band
    "band_mid_ratio",   # 7  fraction in the bearing-defect band
    "band_high_ratio",  # 8  fraction in the resonance band
    "current_rms_a",    # 9  motor load current, amps RMS over the window
    "temp_c",           # 10 bearing housing temperature, deg C, mean over the window
]
N_FEATURES = len(FEATURE_NAMES)

CLASS_NAMES = ["HEALTHY", "EARLY_FAULT", "CRITICAL"]
N_CLASSES = len(CLASS_NAMES)

# ---------------------------------------------------------------------------
# 4. VIBRATION PROJECTION
# ---------------------------------------------------------------------------
# How do you turn three accelerometer axes into ONE signal to run an FFT on?
#
# The obvious answer -- vector magnitude sqrt(ax^2+ay^2+az^2) -- is WRONG for
# spectral analysis, and it is worth understanding why. Squaring is a nonlinearity.
# If the true radial vibration is A*sin(2*pi*89*t), the magnitude signal contains a
# term proportional to A^2*sin^2(...) = (A^2/2)(1 - cos(2*pi*178*t)). Your 89 Hz
# bearing tone gets reported at 178 Hz. The dominant-frequency feature would point
# at a harmonic that does not physically exist, and it would do so consistently
# enough that the model would happily learn the wrong thing.
#
# Instead we use a LINEAR projection onto the dominant vibration direction:
#   - remove each axis's DC component (this is what strips out gravity and the
#     sensor's zero-g bias, without needing to know the mounting orientation),
#   - find the direction of maximum variance of the resulting 3-D AC cloud --
#     the first principal component -- via power iteration on the 3x3 covariance,
#   - project onto it.
#
# This is the automatic version of what a vibration analyst does by hand when they
# choose the radial measurement axis. It is fully linear, so frequencies survive
# untouched, and it is orientation-independent, so the sensor can be glued on at
# any angle.
#
# PCA_ITERS is fixed (not convergence-based) so Python and C execute the identical
# number of arithmetic operations. The sign of the recovered eigenvector is
# arbitrary, but every feature we compute (RMS, |peak|, even moments, |FFT|) is
# sign-invariant, so it does not matter.
PCA_ITERS = 8

# ---------------------------------------------------------------------------
# 5. SENSOR SCALING  (shared by firmware ADC conversion and the data generator)
# ---------------------------------------------------------------------------
CURRENT_MAX_A = 30.0    # pot fully clockwise == 30 A, the CT's full-scale output
TEMP_MIN_C = 20.0       # pot fully anticlockwise == 20 deg C
TEMP_MAX_C = 120.0      # pot fully clockwise == 120 deg C
ADC_MAX_COUNTS = 4095   # we call analogReadResolution(12) on the Pico

# ---------------------------------------------------------------------------
# 6. MACHINE MODEL  (the physics the synthetic generator and the Wokwi demo share)
# ---------------------------------------------------------------------------
# A 4-pole induction motor on a 50 Hz supply runs near 1500 RPM = 25 Hz shaft speed.
# For a typical deep-groove ball bearing the outer-race defect frequency is roughly
# 3.5x shaft speed, so BPFO ~ 89 Hz. When a ball rolls over a spall in the outer
# race it delivers a sharp impact that excites the bearing's structural resonance
# (a few hundred Hz here), which rings down in a few milliseconds.
SHAFT_HZ = 25.0          # 1x running speed
BPFO_HZ = 89.0           # ball-pass frequency, outer race -- the defect repetition rate
RESONANCE_HZ = 600.0     # structural resonance excited by each impact
IMPACT_TAU_S = 0.0015    # ring-down time constant of one impact (1.5 ms)

# ---------------------------------------------------------------------------
# 7. DEGRADATION INDEX  (transparent heuristic, NOT a learned model)
# ---------------------------------------------------------------------------
# The classifier answers "what state is the motor in". Maintenance planners also
# want "how bad, on a continuous scale" -- a stand-in for Remaining Useful Life.
#
# A real RUL estimate needs run-to-failure history that we do not have (and cannot
# synthesise honestly), so this is deliberately a HAND-WRITTEN weighted score, not
# something pretending to be learned. Each contributing feature is normalised 0..1
# against a documented healthy->failed range, then weighted and summed.
#
# Being a heuristic is a feature, not an apology: a maintenance engineer can read
# these five numbers and understand exactly why the index moved. The true
# data-driven RUL path is the LSTM autoencoder in the Tier-2 notebook.
DEGRADATION_TERMS = [
    # (feature_name,      value_at_0%,  value_at_100%,  weight)
    ("vib_kurtosis",            3.0,          12.0,      0.30),
    ("vib_std",                 0.02,          0.35,     0.25),
    ("band_high_ratio",         0.05,          0.55,     0.15),
    ("current_rms_a",          12.0,          26.0,      0.15),
    ("temp_c",                 45.0,         105.0,      0.15),
]

# Floors keep the index coherent with the LED: the bar can never read "12%" while
# the red CRITICAL light is on. Indexed by class.
DEGRADATION_CLASS_FLOOR = [0.0, 35.0, 70.0]

# Exponential moving average on the displayed index, so the OLED bar glides instead
# of flickering between windows. alpha applies to the NEW reading.
DEGRADATION_EMA_ALPHA = 0.25


# ---------------------------------------------------------------------------
# 8. SPEC FINGERPRINT
# ---------------------------------------------------------------------------
def _spec_signature() -> str:
    """A canonical string covering everything the firmware and model must agree on."""
    parts = [
        f"FS={FS_HZ:.6f}",
        f"N={WINDOW_N}",
        f"KMIN={SPECTRUM_K_MIN}",
        f"KMAX={SPECTRUM_K_MAX}",
        f"PCA={PCA_ITERS}",
        "BANDS=" + ";".join(f"{k}:{lo}-{hi}" for k, (lo, hi) in BAND_BINS.items()),
        "FEATS=" + ",".join(FEATURE_NAMES),
        "CLASSES=" + ",".join(CLASS_NAMES),
    ]
    return "|".join(parts)


def spec_hash() -> int:
    """FNV-1a 32-bit over the canonical signature.

    FNV-1a is used rather than, say, hashlib because it is five lines long and can
    be reimplemented anywhere; there is no hidden dependency in the one mechanism
    whose whole job is to catch drift.
    """
    h = 0x811C9DC5
    for byte in _spec_signature().encode("utf-8"):
        h ^= byte
        h = (h * 0x01000193) & 0xFFFFFFFF
    return h


def band_bin_ranges() -> list[tuple[int, int]]:
    """Band edges in the fixed order low, mid, high."""
    return [BAND_BINS["low"], BAND_BINS["mid"], BAND_BINS["high"]]


def feature_index(name: str) -> int:
    return FEATURE_NAMES.index(name)


# ---------------------------------------------------------------------------
# 9. C HEADER GENERATION
# ---------------------------------------------------------------------------
def emit_c_header(path: str) -> str:
    """Write firmware/src/feature_spec.h from the constants above."""
    (lo_l, hi_l), (lo_m, hi_m), (lo_h, hi_h) = band_bin_ranges()

    enum_lines = "\n".join(
        f"    FEAT_{name.upper()} = {i}," for i, name in enumerate(FEATURE_NAMES)
    )
    name_lines = ",\n".join(f'    "{name}"' for name in FEATURE_NAMES)
    class_lines = ",\n".join(f'    "{name}"' for name in CLASS_NAMES)

    deg_lines = "\n".join(
        f"    {{ FEAT_{name.upper()}, {lo}f, {hi}f, {w}f }},"
        for name, lo, hi, w in DEGRADATION_TERMS
    )

    body = f"""/* -----------------------------------------------------------------------------
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
#define FEATURE_SPEC_HASH 0x{spec_hash():08X}u

/* --- acquisition ---------------------------------------------------------- */
#define FS_HZ            {FS_HZ:.1f}f
#define WINDOW_N         {WINDOW_N}
#define BIN_HZ           ({FS_HZ:.1f}f / (float)WINDOW_N)   /* {BIN_HZ:.5f} Hz */
#define N_BINS           {N_BINS}

/* --- spectral bands (inclusive FFT bin indices; bin 0 = DC is excluded) ---- */
#define SPECTRUM_K_MIN   {SPECTRUM_K_MIN}
#define SPECTRUM_K_MAX   {SPECTRUM_K_MAX}
#define BAND_LOW_K0      {lo_l}
#define BAND_LOW_K1      {hi_l}
#define BAND_MID_K0      {lo_m}
#define BAND_MID_K1      {hi_m}
#define BAND_HIGH_K0     {lo_h}
#define BAND_HIGH_K1     {hi_h}

/* --- 3-axis -> 1-axis projection ------------------------------------------ */
#define PCA_ITERS        {PCA_ITERS}

/* --- feature vector layout ------------------------------------------------ */
#define N_FEATURES       {N_FEATURES}
#define N_CLASSES        {N_CLASSES}

enum FeatureIndex {{
{enum_lines}
}};

static const char *const FEATURE_NAMES[N_FEATURES] = {{
{name_lines}
}};

static const char *const CLASS_NAMES[N_CLASSES] = {{
{class_lines}
}};

/* --- sensor scaling ------------------------------------------------------- */
#define CURRENT_MAX_A    {CURRENT_MAX_A}f
#define TEMP_MIN_C       {TEMP_MIN_C}f
#define TEMP_MAX_C       {TEMP_MAX_C}f
#define ADC_MAX_COUNTS   {ADC_MAX_COUNTS}.0f

/* --- machine model (used by the Wokwi vibration shim) --------------------- */
#define SHAFT_HZ         {SHAFT_HZ}f
#define BPFO_HZ          {BPFO_HZ}f
#define RESONANCE_HZ     {RESONANCE_HZ}f
#define IMPACT_TAU_S     {IMPACT_TAU_S}f

/* --- degradation index ---------------------------------------------------- */
typedef struct {{
    uint8_t feature;   /* index into the feature vector */
    float   lo;        /* reading that maps to 0%   */
    float   hi;        /* reading that maps to 100% */
    float   weight;    /* contribution; weights sum to 1.0 */
}} DegradationTerm;

#define N_DEGRADATION_TERMS {len(DEGRADATION_TERMS)}

static const DegradationTerm DEGRADATION_TERMS[N_DEGRADATION_TERMS] = {{
{deg_lines}
}};

static const float DEGRADATION_CLASS_FLOOR[N_CLASSES] = {{
    {", ".join(f"{v}f" for v in DEGRADATION_CLASS_FLOOR)}
}};

#define DEGRADATION_EMA_ALPHA {DEGRADATION_EMA_ALPHA}f

#endif /* FEATURE_SPEC_H */
"""

    os.makedirs(os.path.dirname(os.path.abspath(path)), exist_ok=True)
    with open(path, "w", encoding="utf-8", newline="\n") as fh:
        fh.write(body)
    return path


def _default_header_path() -> str:
    here = os.path.dirname(os.path.abspath(__file__))
    return os.path.join(here, "..", "firmware", "src", "feature_spec.h")


if __name__ == "__main__":
    out = os.path.normpath(_default_header_path())
    emit_c_header(out)
    print("Feature spec")
    print("-" * 60)
    print(f"  sample rate      : {FS_HZ:.0f} Hz")
    print(f"  window           : {WINDOW_N} samples ({WINDOW_N / FS_HZ * 1000:.0f} ms)")
    print(f"  FFT bin width    : {BIN_HZ:.4f} Hz")
    print(f"  usable bins      : {SPECTRUM_K_MIN}..{SPECTRUM_K_MAX}")
    for band, (lo, hi) in BAND_BINS.items():
        print(f"  band {band:<5}       : bins {lo:>3}..{hi:<3} "
              f"= {lo * BIN_HZ:6.1f} .. {hi * BIN_HZ:6.1f} Hz")
    print(f"  features ({N_FEATURES})     : {', '.join(FEATURE_NAMES)}")
    print(f"  spec hash        : 0x{spec_hash():08X}")
    print(f"\nWrote {out}")
