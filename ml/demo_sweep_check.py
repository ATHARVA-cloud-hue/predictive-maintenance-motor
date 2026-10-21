"""
demo_sweep_check.py
===================
Does the live demo actually work?

The Wokwi demo depends on a mapping that lives in the FIRMWARE, not in the
training pipeline: `vib_source_update_operating_point()` in
firmware/src/vibration_source.cpp turns the MPU6050 slider position into signal
parameters. If that mapping is mistuned, the demo fails in the most
embarrassing possible way -- you drag the slider in front of an audience and the
LED stays stubbornly green, or it slams straight to red and never shows the
EARLY_FAULT state that the entire "catch it early" pitch depends on.

Finding that out while the projector is on is not the plan. This script
re-implements the firmware's synthesis in Python, pushes it through the real
feature extractor and the real trained forest, and prints what the demo will do
at each slider position.

KEEP THIS IN SYNC with vibration_source.cpp. It is a test of that file, so if
you retune the severity mapping there, mirror it here and re-run.
"""

from __future__ import annotations

import os

import joblib
import numpy as np

from extract_features import extract_features
from feature_spec import (
    BPFO_HZ,
    CLASS_NAMES,
    FS_HZ,
    IMPACT_TAU_S,
    RESONANCE_HZ,
    SHAFT_HZ,
    WINDOW_N,
)

ARTIFACT_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "artifacts")

# Pot positions used while sweeping vibration. Mid-scale on both, so the demo is
# judged on vibration alone -- which is the point.
DEMO_CURRENT_A = 14.0
DEMO_TEMP_C = 58.0


def firmware_operating_point(severity: float) -> dict:
    """EXACT mirror of vib_source_update_operating_point() in vibration_source.cpp."""
    s2 = severity * severity
    return dict(
        a1=0.020 + 0.040 * s2,
        a2=0.008 + 0.018 * s2,
        abp=0.130 * s2,
        mod=0.30 + 0.50 * severity,
        aimp=0.700 * s2,
        noise=0.017 + 0.030 * s2,
    )


def firmware_window(severity: float, rng: np.random.Generator):
    """EXACT mirror of vib_source_next(), run WINDOW_N times."""
    p = firmware_operating_point(severity)
    inv_fs = 1.0 / FS_HZ
    decay = float(np.exp(-1.0 / (IMPACT_TAU_S * FS_HZ)))

    ph_shaft = ph_shaft2 = ph_bpfo = 0.0
    ph_impact = 0.0
    impact_threshold = 1.0
    ring_amp = 0.0
    ring_phase = 0.0

    v_out = np.empty(WINDOW_N)

    for i in range(WINDOW_N):
        v = p["a1"] * np.sin(2 * np.pi * ph_shaft)
        v += p["a2"] * np.sin(2 * np.pi * ph_shaft2)

        envelope = 1.0 + p["mod"] * np.sin(2 * np.pi * ph_shaft)
        v += p["abp"] * envelope * np.sin(2 * np.pi * ph_bpfo)

        ph_impact += BPFO_HZ * inv_fs
        if ph_impact >= impact_threshold:
            ph_impact -= impact_threshold
            impact_threshold = 1.0 + (rng.random() - 0.5) * 0.03
            ring_amp = p["aimp"] * (0.6 + 0.8 * rng.random())
            ring_phase = 0.0
        if ring_amp > 1e-6:
            v += ring_amp * np.sin(2 * np.pi * ring_phase)
            ring_amp *= decay
            ring_phase += RESONANCE_HZ * inv_fs
            if ring_phase >= 1.0:
                ring_phase -= 1.0

        v += p["noise"] * rng.normal()

        ph_shaft += SHAFT_HZ * inv_fs
        if ph_shaft >= 1.0:
            ph_shaft -= 1.0
        ph_shaft2 += 2.0 * SHAFT_HZ * inv_fs
        if ph_shaft2 >= 1.0:
            ph_shaft2 -= 1.0
        ph_bpfo += BPFO_HZ * inv_fs
        if ph_bpfo >= 1.0:
            ph_bpfo -= 1.0

        v_out[i] = v

    ax = np.clip(0.940 * v_out + 0.004 * rng.normal(size=WINDOW_N), -2.0, 2.0)
    ay = np.clip(0.300 * v_out + 0.004 * rng.normal(size=WINDOW_N), -2.0, 2.0)
    az = np.clip(1.0 + 0.160 * v_out + 0.004 * rng.normal(size=WINDOW_N), -2.0, 2.0)
    return ax, ay, az


def main() -> None:
    model_path = os.path.join(ARTIFACT_DIR, "rf_model.joblib")
    if not os.path.exists(model_path):
        raise SystemExit("No trained model. Run train_random_forest.py first.")
    forest = joblib.load(model_path)

    rng = np.random.default_rng(7)
    repeats = 12

    print("=" * 76)
    print("Demo sweep -- what the Wokwi simulation will show as you drag the slider")
    print(f"(current pot held at {DEMO_CURRENT_A:.1f} A, temp pot at "
          f"{DEMO_TEMP_C:.0f} C, so vibration alone drives the verdict)")
    print("=" * 76)
    print(f"{'slider':>7}{'severity':>10}{'kurtosis':>10}{'vib_std':>9}"
          f"{'mid band':>10}{'dom Hz':>8}   verdict (of 12 windows)")
    print("-" * 76)

    seen = {}
    for severity in np.linspace(0.0, 1.0, 11):
        rows = []
        for _ in range(repeats):
            ax, ay, az = firmware_window(float(severity), rng)
            rows.append(extract_features(
                ax, ay, az,
                np.full(WINDOW_N, DEMO_CURRENT_A),
                np.full(WINDOW_N, DEMO_TEMP_C)))
        feats = np.asarray(rows)
        preds = forest.predict(feats)
        counts = np.bincount(preds, minlength=3)
        winner = int(np.argmax(counts))
        seen.setdefault(winner, severity)

        tally = " ".join(f"{CLASS_NAMES[c][:4]}:{counts[c]}"
                         for c in range(3) if counts[c])
        # The slider reads in g on the X axis; severity = tilt / 1.5.
        print(f"{severity * 1.5:>7.2f}{severity:>10.2f}"
              f"{np.median(feats[:, 3]):>10.2f}{np.median(feats[:, 2]):>9.3f}"
              f"{np.median(feats[:, 7]):>10.3f}{np.median(feats[:, 5]):>8.1f}"
              f"   {tally}")

    print("-" * 76)
    missing = [CLASS_NAMES[c] for c in range(3) if c not in seen]
    if missing:
        print(f"  DEMO PROBLEM: never reaches {', '.join(missing)}.")
        print("  Retune the op_* mapping in vibration_source.cpp and re-run.")
    else:
        print("  All three states are reachable by dragging accelerationX alone:")
        for c in range(3):
            print(f"    {CLASS_NAMES[c]:<12} first appears at slider "
                  f"{seen[c] * 1.5:.2f} g")
        print("\n  Demo script: start at 0.00 g (green), drag X to ~0.6 g for")
        print("  yellow, then past ~1.1 g for red and the buzzer.")


if __name__ == "__main__":
    main()
