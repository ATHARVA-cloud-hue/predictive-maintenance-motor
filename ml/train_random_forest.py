"""
train_random_forest.py
======================
Trains the classifier that gets converted to C and flashed onto the Pico W.

WHY A RANDOM FOREST (and not a neural network)
----------------------------------------------
This is the central ML decision in the project, so it is worth justifying properly
rather than asserting.

  1. IT CONVERTS TO C AS PLAIN CONTROL FLOW.
     A trained tree IS a nested if/else statement. There is no matrix algebra, no
     activation function, no runtime, no interpreter -- `export_to_c.py` emits
     literal `if (f[3] <= 4.812f)` lines. No TensorFlow Lite Micro (~20 kB of
     runtime before your model), no operator kernels, no quantisation step. The
     model does not need a framework to execute; it IS the executable.

  2. INFERENCE IS INTEGER-SPEED AND BRANCH-PREDICTABLE.
     Classifying one sample costs ~15 trees x ~8 float comparisons = ~120
     compares. On a 133 MHz Cortex-M0+ with no FPU that is microseconds. A small
     MLP would need hundreds of software-float multiply-accumulates.

  3. NO FEATURE SCALING TO KEEP IN SYNC.
     This one is underrated for embedded work. Trees split on raw thresholds, so
     there is no mean/variance normalisation to replicate in C. Every normaliser
     you ship is another object that can silently drift out of sync with the
     firmware -- the exact failure mode feature_spec.py exists to prevent. Using
     a model that needs none removes a whole category of bug. An SVM or an MLP
     would require shipping the StandardScaler's parameters too.

  4. IT IS INTERPRETABLE, WHICH A MAINTENANCE ENGINEER WILL DEMAND.
     "Why did you shut my production line down?" is answerable: you can print the
     decision path. Feature importances tell you which physical measurement drove
     the call. A neural network gives you a number and a shrug.

  5. IT HANDLES SMALL, MIXED-SCALE, CORRELATED TABULAR DATA WELL.
     Our 11 features span 0.02 g to 115 degC and several are correlated. Trees are
     scale-invariant and untroubled by collinearity. This is exactly the regime
     where gradient-boosted/bagged trees still beat deep learning on tabular data.

The trade-off we accept: a forest can only recognise fault signatures resembling
those it was trained on. It cannot flag a novel failure mode. That is precisely
the gap the Tier-2 LSTM autoencoder closes -- see the notebook.

SIZING FOR FLASH
----------------
n_estimators and max_depth are kept deliberately small. Accuracy saturates quickly
on this problem, and every extra node is flash. The script prints the node count
and the measured header size so the cost is visible, not assumed.
"""

from __future__ import annotations

import os

import numpy as np

# matplotlib must pick a non-interactive backend BEFORE pyplot is imported,
# otherwise it tries to open a window on a machine that may not have a display.
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt                                    # noqa: E402
from matplotlib.colors import LinearSegmentedColormap              # noqa: E402

import joblib                                                      # noqa: E402
from sklearn.ensemble import RandomForestClassifier                # noqa: E402
from sklearn.inspection import permutation_importance              # noqa: E402
from sklearn.metrics import (accuracy_score, classification_report,  # noqa: E402
                             confusion_matrix)
from sklearn.model_selection import cross_val_score, train_test_split  # noqa: E402

from feature_spec import CLASS_NAMES, FEATURE_NAMES, N_FEATURES    # noqa: E402

ARTIFACT_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "artifacts")

# --- model size knobs -------------------------------------------------------
N_ESTIMATORS = 15
MAX_DEPTH = 8
MIN_SAMPLES_LEAF = 4
RANDOM_STATE = 42

# --- plot styling -----------------------------------------------------------
# Three-class severity palette, drawn from a validated categorical ramp rather
# than from raw red/amber/green. A naive green/red pair measures Delta E 4.1 under
# deuteranopia -- indistinguishable to roughly 1 in 12 men -- which is unusable for
# a fault dashboard. These three measure 6.9 at worst, which is only acceptable
# WITH a second visual channel, so every chart below also varies marker shape and
# labels its classes directly. Colour never carries the meaning alone.
CLASS_COLORS = ["#1baf7a", "#eda100", "#e34948"]   # healthy / early / critical
CLASS_MARKERS = ["o", "s", "^"]

INK = "#0b0b0b"
INK_MUTED = "#898781"
GRID = "#e1e0d9"
SURFACE = "#fcfcfb"
SERIES_BLUE = "#2a78d6"
# Sequential single-hue ramp, light -> dark. Never a rainbow: a rainbow has no
# perceptual ordering, so readers cannot tell which end means "more".
BLUE_RAMP = ["#cde2fb", "#9ec5f4", "#6da7ec", "#3987e5", "#256abf", "#184f95", "#0d366b"]


def _style_axes(ax, *, grid_axis: str | None = "y") -> None:
    """Recessive chrome: the data should be the only assertive thing on the page."""
    ax.set_facecolor(SURFACE)
    for side in ("top", "right"):
        ax.spines[side].set_visible(False)
    for side in ("left", "bottom"):
        ax.spines[side].set_color("#c3c2b7")
        ax.spines[side].set_linewidth(0.8)
    ax.tick_params(colors=INK_MUTED, labelsize=9, length=3, width=0.8)
    if grid_axis:
        ax.grid(axis=grid_axis, color=GRID, linewidth=0.8, zorder=0)
        ax.set_axisbelow(True)


def load_dataset():
    path = os.path.join(ARTIFACT_DIR, "dataset.npz")
    if not os.path.exists(path):
        raise SystemExit("No dataset found. Run `py generate_dataset.py` first.")
    data = np.load(path, allow_pickle=True)
    return data["x"], data["y"]


def count_nodes(forest: RandomForestClassifier) -> tuple[int, int]:
    """Total and leaf node counts -- the thing that becomes flash bytes."""
    total = sum(est.tree_.node_count for est in forest.estimators_)
    leaves = sum(int(np.sum(est.tree_.children_left == -1))
                 for est in forest.estimators_)
    return total, leaves


# ---------------------------------------------------------------------------
# Plots
# ---------------------------------------------------------------------------
def plot_confusion(cm: np.ndarray, path: str) -> None:
    """Confusion matrix as a sequential heatmap.

    Magnitude -> one hue, light to dark. Every cell is also printed as a number,
    so the colour is reinforcement rather than the only way to read the chart.
    """
    cmap = LinearSegmentedColormap.from_list("blues", BLUE_RAMP)
    norm_cm = cm / cm.sum(axis=1, keepdims=True)

    fig, ax = plt.subplots(figsize=(5.6, 5.0), facecolor=SURFACE)
    ax.imshow(norm_cm, cmap=cmap, vmin=0.0, vmax=1.0)

    ax.set_xticks(range(len(CLASS_NAMES)), CLASS_NAMES, fontsize=9, color=INK)
    ax.set_yticks(range(len(CLASS_NAMES)), CLASS_NAMES, fontsize=9, color=INK)
    ax.set_xlabel("Predicted", fontsize=10, color=INK, labelpad=8)
    ax.set_ylabel("Actual", fontsize=10, color=INK, labelpad=8)
    ax.set_title("Confusion matrix (held-out test set)",
                 fontsize=11, color=INK, pad=12, loc="left")

    for i in range(len(CLASS_NAMES)):
        for j in range(len(CLASS_NAMES)):
            # Flip the label ink on dark cells so it stays legible.
            colour = "#ffffff" if norm_cm[i, j] > 0.5 else INK
            ax.text(j, i, f"{cm[i, j]}\n{norm_cm[i, j]*100:.1f}%",
                    ha="center", va="center", fontsize=9.5, color=colour)

    for side in ("top", "right", "left", "bottom"):
        ax.spines[side].set_visible(False)
    ax.tick_params(length=0)
    # 2px surface gap between cells, so adjacent fills never bleed together.
    ax.set_xticks(np.arange(-0.5, len(CLASS_NAMES), 1), minor=True)
    ax.set_yticks(np.arange(-0.5, len(CLASS_NAMES), 1), minor=True)
    ax.grid(which="minor", color=SURFACE, linewidth=2)

    fig.tight_layout()
    fig.savefig(path, dpi=160, facecolor=SURFACE)
    plt.close(fig)


def plot_importance(names, impurity, perm_mean, perm_std, path: str) -> None:
    """Permutation importance, sorted. One series, so no legend -- the title names it."""
    order = np.argsort(perm_mean)
    y = np.arange(len(order))

    fig, ax = plt.subplots(figsize=(7.4, 5.0), facecolor=SURFACE)
    ax.barh(y, perm_mean[order], xerr=perm_std[order],
            color=SERIES_BLUE, height=0.62, zorder=3,
            error_kw=dict(ecolor=INK_MUTED, elinewidth=1, capsize=2.5))

    ax.set_yticks(y, [names[i] for i in order], fontsize=9, color=INK)
    ax.set_xlabel("Drop in accuracy when this feature is shuffled",
                  fontsize=9.5, color=INK, labelpad=8)
    ax.set_title("Permutation importance", fontsize=11, color=INK, pad=12, loc="left")
    _style_axes(ax, grid_axis="x")

    # Direct labels, so the reader never has to measure a bar against the axis.
    span = max(perm_mean.max(), 1e-9)
    for yi, idx in enumerate(order):
        ax.text(perm_mean[idx] + span * 0.02 + perm_std[idx], yi,
                f"{perm_mean[idx]:.3f}", va="center", fontsize=8.5, color=INK_MUTED)
    ax.set_xlim(0, span * 1.22)

    fig.tight_layout()
    fig.savefig(path, dpi=160, facecolor=SURFACE)
    plt.close(fig)


def plot_separation(x: np.ndarray, y: np.ndarray, path: str) -> None:
    """The money plot: kurtosis vs AC severity, which is the physics of the fault."""
    ki = FEATURE_NAMES.index("vib_kurtosis")
    si = FEATURE_NAMES.index("vib_std")

    fig, ax = plt.subplots(figsize=(7.0, 5.0), facecolor=SURFACE)
    for c, name in enumerate(CLASS_NAMES):
        m = y == c
        ax.scatter(x[m, ki], x[m, si],
                   s=13, alpha=0.55, linewidths=0,
                   color=CLASS_COLORS[c], marker=CLASS_MARKERS[c],
                   label=name, zorder=3)

    ax.set_xlabel("vib_kurtosis   (3.0 = Gaussian; higher = impulsive)",
                  fontsize=9.5, color=INK, labelpad=8)
    ax.set_ylabel("vib_std   (AC vibration severity, g)",
                  fontsize=9.5, color=INK, labelpad=8)
    ax.set_title("Why kurtosis is the early indicator",
                 fontsize=11, color=INK, pad=12, loc="left")
    ax.axvline(3.0, color=INK_MUTED, linewidth=1, linestyle=(0, (4, 3)), zorder=2)
    ax.text(3.08, ax.get_ylim()[1] * 0.96, "Gaussian (3.0)",
            fontsize=8.5, color=INK_MUTED, va="top")
    _style_axes(ax, grid_axis="both")

    # Legend AND distinct marker shapes: the colour pair sits in the CVD floor
    # band, so a second channel is mandatory rather than nice to have.
    leg = ax.legend(frameon=False, fontsize=9, loc="upper right",
                    labelcolor=INK, markerscale=1.8)
    for handle in leg.legend_handles:
        handle.set_alpha(1.0)

    fig.tight_layout()
    fig.savefig(path, dpi=160, facecolor=SURFACE)
    plt.close(fig)


# ---------------------------------------------------------------------------
def ablation(x_tr, y_tr, x_te, y_te) -> None:
    """Which sensor group is actually carrying the decision?

    This is not a formality. The live Wokwi demo injects a fault by dragging the
    MPU6050 and nothing else. If the forest had quietly learned "temperature >
    70 means critical" -- the easiest rule in the dataset -- the demo would sit
    there showing HEALTHY while the operator shook the accelerometer, and the
    whole exhibit would fall flat. The confounder samples in generate_dataset.py
    exist to prevent that; this measures whether they worked.
    """
    vib_idx = [FEATURE_NAMES.index(n) for n in FEATURE_NAMES
               if n not in ("current_rms_a", "temp_c")]
    aux_idx = [FEATURE_NAMES.index("current_rms_a"), FEATURE_NAMES.index("temp_c")]

    print("\nAblation -- can each sensor group stand alone?")
    print("-" * 62)
    for label, idx in (("vibration features only (9)", vib_idx),
                       ("current + temperature only (2)", aux_idx),
                       ("all features (11)", list(range(N_FEATURES)))):
        clf = RandomForestClassifier(n_estimators=N_ESTIMATORS, max_depth=MAX_DEPTH,
                                     min_samples_leaf=MIN_SAMPLES_LEAF,
                                     random_state=RANDOM_STATE, n_jobs=-1)
        clf.fit(x_tr[:, idx], y_tr)
        acc = accuracy_score(y_te, clf.predict(x_te[:, idx]))
        print(f"  {label:<34} {acc*100:6.2f}%")
    print("-" * 62)
    print("  Vibration-only accuracy is what matters for the demo. If it is well")
    print("  above chance (33%), dragging the MPU6050 alone will move the state.")


def main() -> None:
    os.makedirs(ARTIFACT_DIR, exist_ok=True)
    x, y = load_dataset()

    x_tr, x_te, y_tr, y_te = train_test_split(
        x, y, test_size=0.25, random_state=RANDOM_STATE, stratify=y)

    clf = RandomForestClassifier(
        n_estimators=N_ESTIMATORS,
        max_depth=MAX_DEPTH,
        min_samples_leaf=MIN_SAMPLES_LEAF,
        random_state=RANDOM_STATE,
        n_jobs=-1,
    )
    clf.fit(x_tr, y_tr)

    pred = clf.predict(x_te)
    acc = accuracy_score(y_te, pred)
    cm = confusion_matrix(y_te, pred)

    print("=" * 62)
    print(f"Random forest: {N_ESTIMATORS} trees, max_depth={MAX_DEPTH}, "
          f"min_samples_leaf={MIN_SAMPLES_LEAF}")
    print("=" * 62)
    print(f"  train samples : {len(y_tr)}")
    print(f"  test samples  : {len(y_te)}")
    print(f"\n  TEST ACCURACY : {acc * 100:.2f}%")

    cv = cross_val_score(clf, x, y, cv=5, n_jobs=-1)
    print(f"  5-fold CV     : {cv.mean()*100:.2f}% +/- {cv.std()*100:.2f}%")

    print("\nConfusion matrix (rows = actual, cols = predicted)")
    print("-" * 62)
    print(f"{'':<14}" + "".join(f"{n:>16}" for n in CLASS_NAMES))
    for i, name in enumerate(CLASS_NAMES):
        print(f"{name:<14}" + "".join(f"{v:>16}" for v in cm[i]))

    print("\nPer-class report")
    print("-" * 62)
    print(classification_report(y_te, pred, target_names=CLASS_NAMES, digits=3))

    # --- importances --------------------------------------------------------
    # Two kinds, because they disagree in instructive ways.
    #
    # IMPURITY importance (the .feature_importances_ attribute) is free, but it
    # is biased toward high-cardinality continuous features -- a feature with
    # many distinct values offers more candidate split points, so it wins splits
    # by chance as well as by merit.
    #
    # PERMUTATION importance is the honest one: shuffle a single column in the
    # TEST set and measure how much accuracy actually falls. If a feature can be
    # randomised without hurting predictions, the model was not relying on it,
    # whatever the impurity number says. It is the one worth quoting.
    perm = permutation_importance(clf, x_te, y_te, n_repeats=20,
                                  random_state=RANDOM_STATE, n_jobs=-1)

    print("Feature importance")
    print("-" * 62)
    print(f"{'feature':<18}{'impurity':>12}{'permutation':>14}{'+/- std':>10}")
    print("-" * 62)
    for i in np.argsort(perm.importances_mean)[::-1]:
        print(f"{FEATURE_NAMES[i]:<18}{clf.feature_importances_[i]:>12.4f}"
              f"{perm.importances_mean[i]:>14.4f}{perm.importances_std[i]:>10.4f}")
    print("-" * 62)

    ablation(x_tr, y_tr, x_te, y_te)

    # --- size ---------------------------------------------------------------
    total_nodes, leaves = count_nodes(clf)
    print("\nEmbedded cost")
    print("-" * 62)
    print(f"  decision nodes : {total_nodes - leaves}")
    print(f"  leaf nodes     : {leaves}")
    print(f"  total nodes    : {total_nodes}")
    print("  Each decision node becomes one `if (f[i] <= t)` in model_rf.h;")
    print("  export_to_c.py reports the resulting header size.")

    # --- save ---------------------------------------------------------------
    model_path = os.path.join(ARTIFACT_DIR, "rf_model.joblib")
    joblib.dump(clf, model_path)

    plot_confusion(cm, os.path.join(ARTIFACT_DIR, "confusion_matrix.png"))
    plot_importance(np.array(FEATURE_NAMES), clf.feature_importances_,
                    perm.importances_mean, perm.importances_std,
                    os.path.join(ARTIFACT_DIR, "feature_importance.png"))
    plot_separation(x, y, os.path.join(ARTIFACT_DIR, "feature_separation.png"))

    print(f"\n  saved {model_path}")
    print(f"  saved {os.path.join(ARTIFACT_DIR, 'confusion_matrix.png')}")
    print(f"  saved {os.path.join(ARTIFACT_DIR, 'feature_importance.png')}")
    print(f"  saved {os.path.join(ARTIFACT_DIR, 'feature_separation.png')}")
    print("\nNext: py export_to_c.py")


if __name__ == "__main__":
    main()
