"""
extract_features.py
===================
The reference implementation of the feature pipeline.

`firmware/src/features.cpp` is a line-for-line C translation of `extract_features()`
below. When you change anything here, change it there -- and bump nothing by hand:
re-run `export_to_c.py` so the spec hash changes and the firmware refuses to build
against a stale model.

Reading order, if you are learning the ML side: start at `extract_features()` at the
bottom and follow the calls upward. Each helper carries the "why does this feature
exist" explanation rather than just the "what does this compute".

A note on float precision
-------------------------
Python computes in float64; the Cortex-M0+ firmware computes in float32. The
resulting relative disagreement is around 1e-7. Random-forest split thresholds in a
trained model are separated by far more than that, so this is not a source of
misclassification. It would matter if we were using a model with sharp decision
boundaries in a normalised space (an SVM with a tight margin, say) -- another small
reason trees suit embedded deployment.
"""

from __future__ import annotations

import numpy as np

from feature_spec import (
    BAND_BINS,
    FS_HZ,
    N_FEATURES,
    PCA_ITERS,
    SPECTRUM_K_MAX,
    SPECTRUM_K_MIN,
    WINDOW_N,
)

# Hann window, precomputed once. Defined explicitly rather than via np.hanning so
# the formula is visible and the C side can copy it character for character.
#
# WHY WINDOW AT ALL: the FFT assumes the 256-sample window repeats forever. A real
# vibration signal does not line up at the window edges, so the discontinuity
# smears energy across every bin ("spectral leakage"), which would corrupt the
# band-ratio features. The Hann window tapers both ends to zero and removes it.
HANN = 0.5 * (1.0 - np.cos(2.0 * np.pi * np.arange(WINDOW_N) / (WINDOW_N - 1)))

_EPS = 1e-20


def project_to_principal_axis(ax: np.ndarray,
                              ay: np.ndarray,
                              az: np.ndarray) -> np.ndarray:
    """Collapse three accelerometer axes into one linear vibration signal.

    See the long explanation in feature_spec.py section 4. In short: vector
    magnitude would square the signal and report an 89 Hz bearing tone at 178 Hz,
    so instead we project onto the direction of maximum variance.

    Steps:
      1. Mean-remove each axis. This is what strips gravity and the sensor's
         zero-g bias, with no need to know how the sensor is mounted.
      2. Build the 3x3 covariance matrix of the AC cloud.
      3. Power-iterate a fixed PCA_ITERS times to find its dominant eigenvector.
         Fixed iteration count, not a convergence test, so Python and C perform
         byte-for-byte the same sequence of operations.
      4. Project the centred data onto it.
    """
    a = np.stack([ax, ay, az], axis=1).astype(np.float64)
    a = a - a.mean(axis=0, keepdims=True)          # step 1: kill DC / gravity

    cov = (a.T @ a) / float(WINDOW_N)              # step 2: 3x3 covariance

    v = np.array([1.0, 1.0, 1.0]) / np.sqrt(3.0)   # step 3: power iteration
    for _ in range(PCA_ITERS):
        v = cov @ v
        n = np.sqrt(v @ v)
        if n > _EPS:
            v = v / n

    # The eigenvector's sign is arbitrary. Every feature below is sign-invariant
    # (RMS, |peak|, even moments, |FFT|), so this only exists to make debug traces
    # reproducible rather than flipping randomly between windows.
    if v[int(np.argmax(np.abs(v)))] < 0.0:
        v = -v

    return a @ v                                   # step 4: the 1-D vibration signal


def magnitude_rms(ax: np.ndarray, ay: np.ndarray, az: np.ndarray) -> float:
    """DC-coupled RMS of the raw acceleration vector magnitude: total specific force.

    Note what this is NOT: it is not the AC vibration severity (that is vib_std).
    This is sqrt(mean(ax^2 + ay^2 + az^2)) on the RAW, un-centred axes, so it
    includes the ~1 g of gravity and sits a little above 1.0 at rest, climbing as
    vibration energy is added.

    Why compute it on the vector magnitude when feature_spec.py spends a paragraph
    explaining that magnitude is the wrong thing to feed an FFT? Because the
    objection is specifically spectral. Squaring destroys FREQUENCY content -- it
    reports an 89 Hz tone at 178 Hz -- but it preserves ENERGY perfectly well; that
    is what Parseval's theorem is about. So we use magnitude for the scalar energy
    level and the principal-axis projection for anything spectral. Two different
    jobs, two appropriate tools.

    Be warned (and this is worth watching for in the feature-importance output):
    because rms^2 ~ 1 + std^2 here, this feature is a gravity-offset, compressed
    version of vib_std and should land near the BOTTOM of the importance ranking.
    That is the correct result, not a bug. It is a cheap live demonstration of why
    you look at feature importances at all -- you cannot tell which of two
    plausible-sounding features actually carries signal by reasoning about them.
    """
    a2 = (np.asarray(ax, dtype=np.float64) ** 2
          + np.asarray(ay, dtype=np.float64) ** 2
          + np.asarray(az, dtype=np.float64) ** 2)
    return float(np.sqrt(np.mean(a2)))


def time_domain_features(vib: np.ndarray) -> tuple[float, float, float, float]:
    """Peak, std, kurtosis, crest factor on the projected vibration signal.

    `vib` is a projection of centred data, so its mean is exactly 0 and its
    standard deviation IS its AC root-mean-square. That is the ISO 10816 "overall
    vibration level" -- the number a condition-monitoring handbook means when it
    says "RMS velocity/acceleration". The separate vib_rms feature is a different
    quantity entirely (see magnitude_rms above).

    KURTOSIS is the headline feature, and it is worth understanding properly.
    It is the 4th standardised moment: mean(v^4) / mean(v^2)^2. For Gaussian noise
    it equals 3.0 exactly. It measures how heavy the distribution's TAILS are --
    how often the signal takes an extreme excursion relative to its own spread.

    A healthy bearing produces near-Gaussian random vibration: kurtosis ~3.
    A spalled race produces a sharp impact every time a ball rolls over the defect.
    Those impacts are rare (89 per second, each lasting ~4 ms) but large, which is
    precisely a heavy-tailed distribution -- kurtosis climbs to 5, 8, 15.

    The crucial part: kurtosis rises while overall RMS is still almost normal,
    because a handful of brief spikes barely move the average energy. That is why
    kurtosis is the classic EARLY warning and RMS is the late one.

    THE TRAP (we deliberately put this in the training data): in a very advanced
    fault the race is so damaged that impacts happen everywhere and merge into
    broadband noise. The distribution becomes Gaussian again and kurtosis FALLS
    BACK toward 3 -- while RMS is enormous. A model trained only on "high kurtosis
    = bad" would downgrade a machine that is about to seize. This is exactly why
    we give the classifier RMS, current and temperature alongside kurtosis instead
    of thresholding on kurtosis alone.

    CREST FACTOR (peak/RMS) is the cheap cousin of kurtosis: same idea, one spike
    instead of the whole tail, so it is noisier but costs almost nothing.
    """
    m2 = float(np.mean(vib * vib))
    m4 = float(np.mean(vib ** 4))

    std = float(np.sqrt(m2))
    peak = float(np.max(np.abs(vib)))

    # Pearson (non-excess) convention: 3.0 for Gaussian. scipy.stats.kurtosis
    # defaults to FISHER, which subtracts 3 and would shift every split threshold
    # in the trained model by 3.0 relative to what the firmware computes. This one
    # line is a genuinely common way to break a TinyML port.
    kurtosis = float(m4 / (m2 * m2)) if m2 > _EPS else 0.0
    crest = float(peak / std) if std > _EPS else 0.0

    return peak, std, kurtosis, crest


def spectral_features(vib: np.ndarray) -> tuple[float, float, float, float]:
    """Dominant frequency and the three band-energy ratios.

    Band ratios rather than absolute band energies, on purpose: ratios are
    invariant to overall gain. A sensor mounted with a thicker adhesive layer, or
    a motor running at 80% load, scales every band equally -- absolute energies
    would shift and the model would see "a different machine", while the ratios
    stay put. The absolute level is still available to the model as vib_rms and
    vib_std; splitting "how much" from "where" gives the trees cleaner questions
    to ask.

    The three ratios sum to 1.0 by construction, so one is formally redundant.
    That is harmless for a random forest (it is not a linear model, so there is no
    collinearity problem) and keeping all three means no band is privileged as
    "the leftover".
    """
    spectrum = np.fft.rfft(vib * HANN)
    power = (spectrum.real ** 2) + (spectrum.imag ** 2)

    # Bin 0 (DC) is excluded everywhere -- the signal is mean-removed, so bin 0 is
    # numerical residue, not information.
    usable = power[SPECTRUM_K_MIN:SPECTRUM_K_MAX + 1]
    total = float(usable.sum())

    if total <= _EPS:
        return 0.0, 0.0, 0.0, 0.0

    dom_k = SPECTRUM_K_MIN + int(np.argmax(usable))
    dom_freq = float(dom_k) * (FS_HZ / float(WINDOW_N))

    ratios = []
    for band in ("low", "mid", "high"):
        k0, k1 = BAND_BINS[band]
        ratios.append(float(power[k0:k1 + 1].sum()) / total)

    return dom_freq, ratios[0], ratios[1], ratios[2]


def extract_features(ax: np.ndarray,
                     ay: np.ndarray,
                     az: np.ndarray,
                     current: np.ndarray,
                     temp: np.ndarray) -> np.ndarray:
    """Turn one window of raw sensor samples into the 11-element feature vector.

    Inputs are all length WINDOW_N:
      ax, ay, az : accelerometer, g
      current    : motor load current, A (per-sample, from the CT conditioning board)
      temp       : bearing housing temperature, deg C

    Output order is FEATURE_NAMES from feature_spec.py, and nothing else.
    """
    if not (len(ax) == len(ay) == len(az) == len(current) == len(temp) == WINDOW_N):
        raise ValueError(f"every channel must be exactly {WINDOW_N} samples")

    rms = magnitude_rms(ax, ay, az)
    vib = project_to_principal_axis(ax, ay, az)

    peak, std, kurtosis, crest = time_domain_features(vib)
    dom_freq, band_low, band_mid, band_high = spectral_features(vib)

    # Current is RMS over the window, matching what the firmware accumulates
    # sample by sample. Temperature is a plain mean -- a bearing housing has a
    # thermal time constant of minutes, so there is nothing for an RMS to capture.
    current_rms = float(np.sqrt(np.mean(np.asarray(current, dtype=np.float64) ** 2)))
    temp_c = float(np.mean(np.asarray(temp, dtype=np.float64)))

    out = np.array([
        rms,          # 0  vib_rms
        peak,         # 1  vib_peak
        std,          # 2  vib_std
        kurtosis,     # 3  vib_kurtosis
        crest,        # 4  vib_crest
        dom_freq,     # 5  dom_freq_hz
        band_low,     # 6  band_low_ratio
        band_mid,     # 7  band_mid_ratio
        band_high,    # 8  band_high_ratio
        current_rms,  # 9  current_rms_a
        temp_c,       # 10 temp_c
    ], dtype=np.float64)

    assert out.shape == (N_FEATURES,), "feature vector length drifted from the spec"
    return out


if __name__ == "__main__":
    # Self-check: feed in signals whose features we can predict analytically.
    from feature_spec import FEATURE_NAMES

    t = np.arange(WINDOW_N) / FS_HZ

    print("Sanity check 1 -- pure 89 Hz tone on X, gravity on Z")
    print("  expect: kurtosis ~1.5 (a sine is flat-tailed, FAR below Gaussian's 3.0),")
    print("          crest ~1.41 (= sqrt(2) for a sine), dom_freq ~89 Hz, mid band ~1.0")
    tone = 0.2 * np.sin(2 * np.pi * 89.0 * t)
    f = extract_features(tone, np.zeros(WINDOW_N), np.ones(WINDOW_N),
                         np.full(WINDOW_N, 12.0), np.full(WINDOW_N, 45.0))
    for name, val in zip(FEATURE_NAMES, f):
        print(f"    {name:<16} {val: .5f}")

    print("\nSanity check 2 -- Gaussian noise only")
    print("  expect: kurtosis ~3.0, crest ~3-4, energy spread across all bands")
    rng = np.random.default_rng(0)
    noise = rng.normal(0.0, 0.05, WINDOW_N)
    f = extract_features(noise, 0.3 * rng.normal(0, 0.05, WINDOW_N), np.ones(WINDOW_N),
                         np.full(WINDOW_N, 12.0), np.full(WINDOW_N, 45.0))
    for name, val in zip(FEATURE_NAMES, f):
        print(f"    {name:<16} {val: .5f}")

    print("\nSanity check 3 -- noise plus sparse impacts (the bearing-fault case)")
    print("  expect: kurtosis WELL above 3, high band ratio elevated")
    impacts = rng.normal(0.0, 0.03, WINDOW_N)
    for k in range(0, WINDOW_N, 11):           # ~89 impacts/sec at 1 kHz
        tt = (np.arange(WINDOW_N) - k) / FS_HZ
        m = tt >= 0
        impacts[m] += 0.5 * np.exp(-tt[m] / 0.004) * np.sin(2 * np.pi * 250.0 * tt[m])
    f = extract_features(impacts, np.zeros(WINDOW_N), np.ones(WINDOW_N),
                         np.full(WINDOW_N, 20.0), np.full(WINDOW_N, 85.0))
    for name, val in zip(FEATURE_NAMES, f):
        print(f"    {name:<16} {val: .5f}")
