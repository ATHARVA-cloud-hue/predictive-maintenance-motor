"""
generate_dataset.py
===================
Synthetic run data for a 1500 RPM induction motor in three health states.

Why synthetic?
--------------
Real run-to-failure bearing datasets exist (NASA IMS, Case Western Reserve), and in
a production project you would use them. Synthesising instead buys two things that
matter here: we control the ground truth exactly, and -- critically -- the SAME
signal model is reimplemented in the firmware's Wokwi shim, so the live demo feeds
the classifier data drawn from the distribution it was trained on. If you later
swap in CWRU data, replace this file only; nothing downstream changes.

The physics being modelled
--------------------------
Every component below corresponds to something a vibration analyst would name on a
spectrum. Nothing is a magic number chosen to make the classes separable.

  1x SHAFT (25 Hz) -- residual IMBALANCE.
     A rotor is never perfectly balanced, so its centre of mass orbits the shaft
     axis once per revolution and throws a sinusoidal force at exactly running
     speed. Present on every healthy machine; a rising 1x means imbalance, NOT a
     bearing fault. Including it is what stops the model from treating "any
     vibration at all" as a fault.

  2x SHAFT (50 Hz) -- MISALIGNMENT.
     A coupling that is not concentric loads the shaft twice per revolution.
     Also a normal background feature at low amplitude.

  BPFO (89 Hz) + shaft sidebands -- the BEARING DEFECT TONE.
     Ball-Pass Frequency, Outer race: how often a rolling element passes the
     defect. The defect sits in a fixed position, but the load on it rises and
     falls as the shaft's load zone rotates past -- so the tone is AMPLITUDE
     MODULATED at shaft speed. Multiplying by (1 + m*sin(2*pi*25*t)) reproduces
     that, and the modulation automatically creates sidebands at 89+/-25 Hz
     (64 and 114 Hz). Those sidebands are the analyst's confirmation that the
     tone is really a rotating-load bearing defect and not some unrelated
     resonance that happens to sit at 89 Hz.

  IMPACT TRAIN -- what actually makes kurtosis spike.
     Each time a ball strikes the spall it delivers a near-impulsive hammer blow.
     An impulse is broadband, so it excites the bearing housing's structural
     RESONANCE (~600 Hz here) which rings down with a ~1.5 ms time constant. At
     89 strikes per second that is roughly 5 ms of ringing in every 11.2 ms gap:
     short, sharp, high-frequency bursts separated by quiet. Large values
     occurring rarely is a heavy-tailed amplitude distribution, which is exactly
     what kurtosis measures.

     The impacts are JITTERED by 1-2%. This is real: rolling elements slip
     slightly in the cage, so the defect frequency is never exactly periodic.
     That jitter is precisely why the impact train smears in a raw spectrum and
     why envelope analysis exists as a technique -- and why we hand the model
     time-domain kurtosis instead of relying on the spectrum alone.

  BROADBAND NOISE -- everything else in the plant.

Deliberate difficulty
---------------------
Two things in here exist to stop the model learning a shortcut:

  * SMEARED CRITICAL samples (20% of the class). In an advanced fault the race is
    damaged everywhere, impacts merge into continuous broadband noise, and
    kurtosis FALLS BACK toward 3 even as RMS goes through the roof. A model that
    learned "kurtosis > 6 = critical" would score well on easy data and then
    downgrade a machine that is about to seize.

  * CONFOUNDER samples (~15% of every class). Temperature and current are the
    easiest features to separate on, so without this the forest would key almost
    entirely on them -- and the live Wokwi demo, where you inject a fault by
    dragging the MPU6050 and nothing else, would not react. So we include healthy
    motors running hot under heavy load, and fast-onset critical faults that have
    not heated up yet. This forces the vibration features to carry the decision.
"""

from __future__ import annotations

import os

import numpy as np

from extract_features import extract_features
from feature_spec import (
    BPFO_HZ,
    CLASS_NAMES,
    FEATURE_NAMES,
    FS_HZ,
    IMPACT_TAU_S,
    RESONANCE_HZ,
    SHAFT_HZ,
    WINDOW_N,
)

HEALTHY, EARLY_FAULT, CRITICAL = 0, 1, 2

SAMPLES_PER_CLASS = 2000
RANDOM_SEED = 20251021

ARTIFACT_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "artifacts")

# Per-class signal parameters. Ranges overlap between adjacent classes on purpose:
# degradation is a continuum, and a classifier that sees cleanly separated classes
# learns a boundary that does not exist on a real machine.
#
#   a1      1x imbalance amplitude, g
#   a2      2x misalignment amplitude, g
#   abp     BPFO tone amplitude, g
#   mod     load-zone modulation depth, 0..1
#   aimp    impact amplitude, g
#   noise   broadband noise sigma, g
#   cur     load current, A
#   temp    bearing housing temperature, deg C
# The 1x/2x amplitudes are kept DELIBERATELY SMALL relative to the broadband
# noise floor. A pure sine has kurtosis 1.5, so a sine-dominated healthy baseline
# would sit near 2.3 -- below Gaussian -- and the fault classes would then have to
# climb out of that hole before they even reached 3.0. Real accelerometer signals
# from a healthy machine are broadband-dominated with a modest 1x peak on top,
# which puts healthy kurtosis right at the Gaussian 3.0 where the textbook says it
# belongs and where the fault threshold is meaningful.
#
# The cur/temp ranges OVERLAP HEAVILY between adjacent classes. An earlier version
# had them nearly disjoint (healthy 40-52 degC, critical 70-115 degC), which let
# those two features alone score 93.7% -- another shortcut around the vibration
# analysis. Bearing housings really do sit anywhere in a wide band depending on
# ambient temperature, duty cycle and cooling, so the overlap is the honest model
# as well as the useful one.
CLASS_PARAMS = {
    HEALTHY: dict(
        a1=(0.010, 0.030), a2=(0.004, 0.012), abp=(0.000, 0.000),
        mod=(0.0, 0.0), aimp=(0.000, 0.000), noise=(0.010, 0.025),
        cur=(10.5, 14.5), temp=(38.0, 62.0),
    ),
    EARLY_FAULT: dict(
        a1=(0.015, 0.040), a2=(0.006, 0.018), abp=(0.015, 0.050),
        mod=(0.30, 0.70), aimp=(0.100, 0.350), noise=(0.012, 0.030),
        cur=(12.0, 18.0), temp=(48.0, 80.0),
    ),
    CRITICAL: dict(
        a1=(0.030, 0.090), a2=(0.012, 0.040), abp=(0.060, 0.200),
        mod=(0.40, 0.90), aimp=(0.400, 1.000), noise=(0.025, 0.070),
        cur=(15.0, 27.0), temp=(62.0, 115.0),
    ),
}

ACCEL_FULL_SCALE_G = 2.0     # MPU6050 configured for +/-2 g, so the signal clips there
P_SMEARED_CRITICAL = 0.20
P_CONFOUNDER = 0.15

# MOUNTING / LOAD GAIN -- the single most important line in this generator.
#
# Without it, absolute vibration amplitude separates the three classes perfectly
# and the model degenerates into one threshold on vib_std. We measured exactly
# that: 100.00% test accuracy, 0.00% CV deviation, and permutation importance of
# 0.0000 for every feature except vib_std -- including kurtosis. A model that
# scores perfectly by learning one rule has not learned the problem.
#
# It is also physically wrong. The absolute level an accelerometer reports depends
# on where it is mounted, how thick the adhesive is, the sensor's own sensitivity
# tolerance, and how hard the motor is working. Two healthy machines can differ
# several-fold. A real PdM system cannot assume "0.1 g means broken" -- it has to
# judge the CHARACTER of the vibration: how impulsive it is, how its energy is
# distributed across frequency. That is precisely what kurtosis, crest factor and
# the band ratios measure, and all of them are SCALE-INVARIANT.
#
# So we multiply the whole composite vibration signal by a random gain. Because
# every component scales together, kurtosis / crest / band ratios / dominant
# frequency are untouched, while vib_std and vib_peak lose their free ride. The
# classes now overlap in amplitude and the model is forced onto the shape
# features -- which is the entire engineering argument for computing them.
#
# Log-uniform rather than uniform: a gain of 0.5x and a gain of 2.0x are equally
# far from nominal in the way that matters (a ratio), so the distribution should
# be symmetric in log space, not linear space.
MOUNTING_GAIN_RANGE = (0.5, 2.0)


def _u(rng: np.random.Generator, span: tuple[float, float]) -> float:
    return float(rng.uniform(span[0], span[1]))


def _impact_train(t: np.ndarray, amplitude: float,
                  rng: np.random.Generator) -> np.ndarray:
    """A jittered train of exponentially-decaying resonance ring-downs at BPFO."""
    if amplitude <= 0.0:
        return np.zeros_like(t)

    out = np.zeros_like(t)
    period = 1.0 / BPFO_HZ
    window_s = WINDOW_N / FS_HZ

    # Start one period BEFORE the window so a ring-down already in progress at
    # t=0 is represented. Windows are sliced from a continuously running machine;
    # pretending each one starts on a clean impact would be a tell the model could
    # learn instead of the physics.
    k = -1
    while k * period < window_s:
        strike = k * period * (1.0 + rng.normal(0.0, 0.015))   # 1.5% cage slip
        dt = t - strike
        live = dt >= 0.0
        # Each impact varies in severity: the ball does not hit the spall the same
        # way every pass (varying load angle, lubricant film thickness).
        gain = amplitude * float(rng.uniform(0.6, 1.4))
        out[live] += gain * np.exp(-dt[live] / IMPACT_TAU_S) \
                          * np.sin(2.0 * np.pi * RESONANCE_HZ * dt[live])
        k += 1

    return out


def synth_window(state: int, rng: np.random.Generator):
    """Generate one 256-sample window of (ax, ay, az, current, temp)."""
    p = CLASS_PARAMS[state]
    t = np.arange(WINDOW_N) / FS_HZ

    a1 = _u(rng, p["a1"])
    a2 = _u(rng, p["a2"])
    abp = _u(rng, p["abp"])
    mod = _u(rng, p["mod"])
    aimp = _u(rng, p["aimp"])
    noise_sigma = _u(rng, p["noise"])
    current_dc = _u(rng, p["cur"])
    temp_dc = _u(rng, p["temp"])

    # --- the smeared-critical subtype: impacts have merged into broadband noise --
    if state == CRITICAL and rng.random() < P_SMEARED_CRITICAL:
        aimp *= 0.35                     # discrete impacts largely gone
        noise_sigma = _u(rng, (0.12, 0.26))   # replaced by heavy broadband energy
        abp *= 0.6                       # the clean tone is washed out too

    # --- confounders: break the temperature/current shortcut ---------------------
    if rng.random() < P_CONFOUNDER:
        if state == HEALTHY:
            # Healthy but working hard on a hot day. High temp and current with a
            # completely clean vibration signature.
            current_dc = _u(rng, (15.0, 22.0))
            temp_dc = _u(rng, (62.0, 88.0))
        elif state == CRITICAL:
            # Fast-onset failure: a race has cracked but the housing has not had
            # time to heat up, and the load has not risen yet.
            current_dc = _u(rng, (12.0, 15.5))
            temp_dc = _u(rng, (46.0, 60.0))

    # --- build the single-axis radial vibration signal --------------------------
    phase = rng.uniform(0.0, 2.0 * np.pi, size=3)

    vib = a1 * np.sin(2.0 * np.pi * SHAFT_HZ * t + phase[0])
    vib += a2 * np.sin(2.0 * np.pi * 2.0 * SHAFT_HZ * t + phase[1])

    if abp > 0.0:
        # Load-zone amplitude modulation -> sidebands at BPFO +/- SHAFT.
        envelope = 1.0 + mod * np.sin(2.0 * np.pi * SHAFT_HZ * t + phase[2])
        vib += abp * envelope * np.sin(2.0 * np.pi * BPFO_HZ * t)

    vib += _impact_train(t, aimp, rng)
    vib += rng.normal(0.0, noise_sigma, WINDOW_N)

    # Mounting / load gain. Applied to the composite so every component scales
    # together: amplitude features move, shape features do not. See the comment
    # on MOUNTING_GAIN_RANGE for why this is load-bearing.
    lo, hi = MOUNTING_GAIN_RANGE
    vib *= float(np.exp(rng.uniform(np.log(lo), np.log(hi))))

    # --- distribute onto three axes at a random mounting orientation ------------
    # The sensor is glued on at whatever angle was convenient, so the radial
    # vibration direction is arbitrary. This is what the firmware's PCA projection
    # has to recover; training on a single fixed axis would let the model cheat.
    direction = np.array([1.0, rng.uniform(-0.5, 0.5), rng.uniform(-0.25, 0.25)])
    direction /= np.linalg.norm(direction)

    gravity = np.array([0.0, 0.0, 1.0])
    bias = rng.normal(0.0, 0.03, 3)        # per-axis zero-g offset, a real MPU6050 trait

    axes = []
    for i in range(3):
        channel = gravity[i] + bias[i] + direction[i] * vib
        channel += rng.normal(0.0, 0.004, WINDOW_N)   # per-axis sensor noise floor
        axes.append(np.clip(channel, -ACCEL_FULL_SCALE_G, ACCEL_FULL_SCALE_G))

    # --- current and temperature ------------------------------------------------
    # The CT conditioning board outputs an already-rectified level, so we model
    # per-sample measurement noise rather than the 50 Hz waveform itself.
    current = current_dc + rng.normal(0.0, 0.15, WINDOW_N)
    temp = temp_dc + rng.normal(0.0, 0.30, WINDOW_N)

    return axes[0], axes[1], axes[2], current, temp


def build_dataset(samples_per_class: int = SAMPLES_PER_CLASS,
                  seed: int = RANDOM_SEED):
    rng = np.random.default_rng(seed)

    rows, labels = [], []
    for state in (HEALTHY, EARLY_FAULT, CRITICAL):
        for _ in range(samples_per_class):
            ax, ay, az, cur, temp = synth_window(state, rng)
            rows.append(extract_features(ax, ay, az, cur, temp))
            labels.append(state)

    x = np.asarray(rows, dtype=np.float64)
    y = np.asarray(labels, dtype=np.int64)

    # Shuffle so any downstream code that forgets to stratify still behaves.
    order = rng.permutation(len(y))
    return x[order], y[order]


def _print_class_profile(x: np.ndarray, y: np.ndarray) -> None:
    print("\nPer-class feature medians (what the classifier actually sees)")
    print("-" * 78)
    header = f"{'feature':<18}" + "".join(f"{n:>18}" for n in CLASS_NAMES)
    print(header)
    print("-" * 78)
    for i, name in enumerate(FEATURE_NAMES):
        line = f"{name:<18}"
        for state in (HEALTHY, EARLY_FAULT, CRITICAL):
            line += f"{np.median(x[y == state, i]):>18.4f}"
        print(line)
    print("-" * 78)
    print("Read the kurtosis row: it should climb clearly from ~3 in HEALTHY. The")
    print("CRITICAL median is pulled DOWN by the smeared subtype -- that is the")
    print("intended difficulty, not a generator bug.")


def main() -> None:
    os.makedirs(ARTIFACT_DIR, exist_ok=True)

    print(f"Generating {SAMPLES_PER_CLASS} windows per class "
          f"({SAMPLES_PER_CLASS * 3} total, {WINDOW_N} samples each at {FS_HZ:.0f} Hz)...")
    x, y = build_dataset()

    npz_path = os.path.join(ARTIFACT_DIR, "dataset.npz")
    np.savez_compressed(npz_path, x=x, y=y, feature_names=np.array(FEATURE_NAMES))

    csv_path = os.path.join(ARTIFACT_DIR, "dataset.csv")
    with open(csv_path, "w", encoding="utf-8", newline="\n") as fh:
        fh.write(",".join(FEATURE_NAMES) + ",label,label_name\n")
        for row, label in zip(x, y):
            fh.write(",".join(f"{v:.6f}" for v in row))
            fh.write(f",{label},{CLASS_NAMES[label]}\n")

    print(f"  wrote {npz_path}")
    print(f"  wrote {csv_path}")
    print(f"  shape: X={x.shape}  y={y.shape}  "
          f"class balance={np.bincount(y).tolist()}")

    _print_class_profile(x, y)


if __name__ == "__main__":
    main()
