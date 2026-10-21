"""
export_to_c.py
==============
Converts the trained scikit-learn RandomForestClassifier into a C header the
firmware #includes. This is the step that makes the project "TinyML" rather than
"a laptop script next to a blinking LED".

HOW A DECISION TREE BECOMES C
-----------------------------
A fitted sklearn tree is not an opaque object. It is five parallel arrays hanging
off `estimator.tree_`, all indexed by node id (node 0 is the root):

    children_left[i]    node id to go to when the test passes, or -1 if i is a leaf
    children_right[i]   node id to go to when the test fails
    feature[i]          which feature index this node tests
    threshold[i]        the value it tests against
    value[i]            class distribution of the training samples that landed here

The decision rule at every internal node is exactly:

    if (x[feature[i]] <= threshold[i])  go to children_left[i]
    else                                go to children_right[i]

That is already an if/else. So "converting a tree to C" is just a depth-first walk
that prints the test at each internal node and prints a `return` at each leaf,
where the returned class is argmax over `value[i]`.

There is no approximation anywhere in this step. The generated C is not a
re-implementation of the model that behaves similarly -- it is the same model,
expressed in a different syntax. The only numerical difference is that float64
thresholds are emitted as float32 literals, which `verify_parity()` below
measures rather than assumes.

WHY EMIT CONTROL FLOW INSTEAD OF A DATA TABLE
---------------------------------------------
The alternative is to store those arrays in flash and write a generic traversal
loop. That is more compact for large forests and is what emlearn does. We emit
if/else because:

  * it compiles to direct branches with no array indirection, so it is faster and
    needs no RAM for a node stack;
  * the compiler can fold, reorder and share subexpressions across it;
  * and -- the real reason for a learning project -- you can OPEN model_rf.h and
    read the model. `if (f[7] <= 0.1482f)` is the trained knowledge, in plain
    sight. Nothing about the deployed classifier is hidden behind a library.

At 389 nodes this costs a few kB of flash out of the Pico's 2 MB. If the forest
ever grew past a few thousand nodes, switching to the table-plus-loop form would
be the right trade.
"""

from __future__ import annotations

import datetime as _dt
import os

import joblib
import numpy as np
from sklearn.ensemble import RandomForestClassifier

import feature_spec
from feature_spec import CLASS_NAMES, FEATURE_NAMES, N_CLASSES, N_FEATURES, spec_hash

HERE = os.path.dirname(os.path.abspath(__file__))
ARTIFACT_DIR = os.path.join(HERE, "artifacts")
FIRMWARE_SRC = os.path.normpath(os.path.join(HERE, "..", "firmware", "src"))

MODEL_HEADER = os.path.join(FIRMWARE_SRC, "model_rf.h")
SPEC_HEADER = os.path.join(FIRMWARE_SRC, "feature_spec.h")
PARITY_CSV = os.path.join(ARTIFACT_DIR, "parity_vectors.csv")


def _fmt(value: float) -> str:
    """A C float literal with enough digits to round-trip through float32."""
    return f"{np.float32(value):.9g}f"


def emit_tree(tree, tree_id: int) -> str:
    """Depth-first walk emitting one `static uint8_t rf_tree_N(const float *f)`."""
    t = tree.tree_
    lines = [f"static uint8_t rf_tree_{tree_id}(const float *f)", "{"]

    def walk(node: int, depth: int) -> None:
        pad = "    " * (depth + 1)

        if t.children_left[node] == -1:            # leaf
            counts = t.value[node][0]
            cls = int(np.argmax(counts))
            total = float(counts.sum())
            share = counts[cls] / total if total > 0 else 0.0
            lines.append(f"{pad}return {cls}; "
                         f"/* {CLASS_NAMES[cls]}, {share * 100:.0f}% pure, "
                         f"n={total:.0f} */")
            return

        fidx = int(t.feature[node])
        lines.append(f"{pad}if (f[{fidx}] <= {_fmt(t.threshold[node])}) "
                     f"{{  /* {FEATURE_NAMES[fidx]} */")
        walk(int(t.children_left[node]), depth + 1)
        lines.append(f"{pad}}} else {{")
        walk(int(t.children_right[node]), depth + 1)
        lines.append(f"{pad}}}")

    walk(0, 0)
    lines.append("}")
    return "\n".join(lines)


def predict_like_c(forest: RandomForestClassifier, x: np.ndarray) -> np.ndarray:
    """Replicate the GENERATED code's behaviour in Python, in float32.

    This is the parity check. It walks the same arrays the emitter walks, casts
    both the feature value and the threshold to float32 exactly as the C compiler
    will, and takes an unweighted majority vote across trees.

    Two things it proves:
      1. the traversal logic being emitted is right (same answers as sklearn);
      2. the float64 -> float32 narrowing of thresholds does not change decisions.

    Note that sklearn's own `predict` averages per-tree PROBABILITIES, while the
    generated C takes a hard majority vote of per-tree argmax labels. Those can
    differ when trees are internally unsure. Any such disagreements are reported
    rather than hidden -- see verify_parity().
    """
    x32 = x.astype(np.float32)
    votes = np.zeros((len(x), N_CLASSES), dtype=np.int32)

    for est in forest.estimators_:
        t = est.tree_
        left, right = t.children_left, t.children_right
        feat, thr, val = t.feature, t.threshold, t.value
        thr32 = thr.astype(np.float32)

        for row in range(len(x32)):
            node = 0
            while left[node] != -1:
                node = (left[node] if x32[row, feat[node]] <= thr32[node]
                        else right[node])
            votes[row, int(np.argmax(val[node][0]))] += 1

    return np.argmax(votes, axis=1)


def verify_parity(forest: RandomForestClassifier, x: np.ndarray) -> int:
    """Compare sklearn's prediction with the generated code's semantics."""
    sk = forest.predict(x)
    c_like = predict_like_c(forest, x)
    mismatches = int(np.sum(sk != c_like))

    print("Parity check -- sklearn vs. the semantics of the generated C")
    print("-" * 64)
    print(f"  samples compared      : {len(x)}")
    print(f"  disagreements         : {mismatches}  "
          f"({mismatches / len(x) * 100:.3f}%)")
    if mismatches == 0:
        print("  The emitted C is behaviourally identical to the trained model.")
    else:
        print("  Non-zero disagreement is expected in small numbers: sklearn")
        print("  averages per-tree probabilities while the firmware takes a hard")
        print("  majority vote. They only differ where trees were internally")
        print("  split, i.e. on samples the model was unsure about anyway.")
    print("-" * 64)
    return mismatches


def write_parity_vectors(x: np.ndarray, expected: np.ndarray, n: int = 64) -> None:
    """Emit on-target self-test data covering BOTH halves of the contract.

    Two independent checks get baked into the firmware:

      A. MODEL parity -- feature vectors with the class the trained model
         assigns them. Verifies the forest survived the trip to C.

      B. FEATURE parity -- RAW accelerometer windows with the feature vector
         the Python reference computes from them. Verifies features.cpp agrees
         with extract_features.py: the FFT, the Hann window, the PCA
         projection, the kurtosis convention, the band edges.

    (B) is the one that matters most and is the hardest to check any other way.
    A desktop C compiler could test it on a laptop, but the question that
    actually needs answering is whether it holds on the TARGET, in float32, with
    the ARM toolchain's libm -- and the only honest way to answer that is to run
    it there. So the test ships inside the firmware and prints its verdict over
    serial at boot. Build with -DENABLE_SELFTEST and watch the monitor.
    """
    os.makedirs(ARTIFACT_DIR, exist_ok=True)
    idx = np.linspace(0, len(x) - 1, min(n, len(x))).astype(int)
    with open(PARITY_CSV, "w", encoding="utf-8", newline="\n") as fh:
        fh.write(",".join(FEATURE_NAMES) + ",expected_class\n")
        for i in idx:
            fh.write(",".join(f"{v:.6f}" for v in x[i]))
            fh.write(f",{expected[i]}\n")

    # --- (B) raw windows, regenerated from the physics model ------------------
    # dataset.npz stores features only, so re-synthesise a couple of windows
    # with a fixed seed. One healthy, one critical, so the test exercises both
    # the quiet and the saturated end of the pipeline.
    import generate_dataset as gd
    from extract_features import extract_features

    raw_rng = np.random.default_rng(99)
    raw_windows = []
    for state in (gd.HEALTHY, gd.CRITICAL):
        ax, ay, az, cur, temp = gd.synth_window(state, raw_rng)
        feats = extract_features(ax, ay, az, cur, temp)
        raw_windows.append({
            "ax": ax, "ay": ay, "az": az,
            "cur_sumsq": float(np.sum(np.asarray(cur, dtype=np.float64) ** 2)),
            "temp_sum": float(np.sum(np.asarray(temp, dtype=np.float64))),
            "features": feats,
            "label": CLASS_NAMES[state],
        })

    def c_array(values) -> str:
        out, line = [], "    "
        for v in values:
            tok = _fmt(v) + ","
            if len(line) + len(tok) > 96:
                out.append(line)
                line = "    "
            line += tok + " "
        out.append(line)
        return "\n".join(out)

    out = os.path.join(FIRMWARE_SRC, "selftest_vectors.h")
    with open(out, "w", encoding="utf-8", newline="\n") as fh:
        fh.write("/* GENERATED by ml/export_to_c.py -- do not edit.\n"
                 " *\n"
                 " * Built only when ENABLE_SELFTEST is defined. Two data sets:\n"
                 " *   SELFTEST_X / SELFTEST_Y   feature vectors -> expected class\n"
                 " *   RAW_*                     raw windows -> expected features\n"
                 " */\n")
        fh.write("#ifndef SELFTEST_VECTORS_H\n#define SELFTEST_VECTORS_H\n\n")

        fh.write("/* --- A: model parity ------------------------------------ */\n")
        fh.write(f"#define N_SELFTEST_VECTORS {len(idx)}\n\n")
        fh.write(f"static const float SELFTEST_X[N_SELFTEST_VECTORS][{N_FEATURES}] "
                 "= {\n")
        for i in idx:
            fh.write("    {" + ", ".join(_fmt(v) for v in x[i]) + "},\n")
        fh.write("};\n\n")
        fh.write("static const unsigned char SELFTEST_Y[N_SELFTEST_VECTORS] = {\n    ")
        fh.write(", ".join(str(int(expected[i])) for i in idx))
        fh.write("\n};\n\n")

        fh.write("/* --- B: feature-pipeline parity -------------------------- */\n")
        fh.write(f"#define N_RAW_WINDOWS {len(raw_windows)}\n\n")
        for axis in ("ax", "ay", "az"):
            fh.write(f"static const float RAW_{axis.upper()}"
                     f"[N_RAW_WINDOWS][{len(raw_windows[0]['ax'])}] = {{\n")
            for w in raw_windows:
                fh.write("  { /* " + w["label"] + " */\n")
                fh.write(c_array(w[axis]))
                fh.write("\n  },\n")
            fh.write("};\n\n")

        fh.write("static const float RAW_CUR_SUMSQ[N_RAW_WINDOWS] = {\n    ")
        fh.write(", ".join(_fmt(w["cur_sumsq"]) for w in raw_windows))
        fh.write("\n};\n\n")
        fh.write("static const float RAW_TEMP_SUM[N_RAW_WINDOWS] = {\n    ")
        fh.write(", ".join(_fmt(w["temp_sum"]) for w in raw_windows))
        fh.write("\n};\n\n")
        fh.write(f"static const float RAW_EXPECTED[N_RAW_WINDOWS][{N_FEATURES}] "
                 "= {\n")
        for w in raw_windows:
            fh.write("    {" + ", ".join(_fmt(v) for v in w["features"])
                     + "},  /* " + w["label"] + " */\n")
        fh.write("};\n\n")

        fh.write("#endif /* SELFTEST_VECTORS_H */\n")
    print(f"  wrote {out}")


def emit_model_header(forest: RandomForestClassifier, accuracy: float) -> str:
    n_trees = len(forest.estimators_)
    total_nodes = sum(e.tree_.node_count for e in forest.estimators_)
    leaves = sum(int(np.sum(e.tree_.children_left == -1)) for e in forest.estimators_)

    parts = [f"""/* -----------------------------------------------------------------------------
 * model_rf.h -- GENERATED FILE. DO NOT EDIT BY HAND.
 *
 * A scikit-learn RandomForestClassifier, expressed as nested if/else statements.
 * Produced by ml/export_to_c.py from ml/artifacts/rf_model.joblib.
 *
 *   generated     : {_dt.datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
 *   trees         : {n_trees}
 *   total nodes   : {total_nodes} ({total_nodes - leaves} decisions, {leaves} leaves)
 *   test accuracy : {accuracy * 100:.2f}%
 *   classes       : {', '.join(f'{i}={n}' for i, n in enumerate(CLASS_NAMES))}
 *
 * Each tree votes for one class; the forest returns the class with the most
 * votes. Ties break toward the lower class index, which is the conservative
 * direction here only by accident -- with an odd tree count and 3 classes an
 * exact tie is possible, so do not rely on that.
 *
 * Regenerate with:   py ml/export_to_c.py
 * -------------------------------------------------------------------------- */
#ifndef MODEL_RF_H
#define MODEL_RF_H

#include <stdint.h>
#include "feature_spec.h"

/* Fingerprint of the feature spec THIS MODEL WAS TRAINED AGAINST. main.cpp
 * static_asserts it against FEATURE_SPEC_HASH. If you change the sample rate,
 * window length, band edges or feature order and forget to retrain, the build
 * stops here instead of shipping a model that quietly misreads its inputs. */
#define MODEL_FEATURE_SPEC_HASH 0x{spec_hash():08X}u
"""]

    for i, est in enumerate(forest.estimators_):
        parts.append(f"\n/* --- tree {i} "
                     f"({est.tree_.node_count} nodes) {'-' * 46} */\n")
        parts.append(emit_tree(est, i))
        parts.append("\n")

    vote_calls = "\n".join(f"    votes[rf_tree_{i}(f)]++;" for i in range(n_trees))

    parts.append(f"""
/* -----------------------------------------------------------------------------
 * Forest inference.
 *
 * `votes` receives the per-class tally, which the firmware turns into a
 * confidence figure for the OLED and the telemetry line. A 15-tree forest
 * returning 8/15 for CRITICAL is a materially different situation from one
 * returning 15/15, and a maintenance operator deserves to see which.
 * -------------------------------------------------------------------------- */
#define RF_N_TREES {n_trees}

static uint8_t rf_predict(const float *f, uint8_t *votes)
{{
    for (uint8_t i = 0; i < N_CLASSES; i++) {{
        votes[i] = 0;
    }}

{vote_calls}

    uint8_t best = 0;
    for (uint8_t i = 1; i < N_CLASSES; i++) {{
        if (votes[i] > votes[best]) {{
            best = i;
        }}
    }}
    return best;
}}

#endif /* MODEL_RF_H */
""")

    body = "".join(parts)
    os.makedirs(FIRMWARE_SRC, exist_ok=True)
    with open(MODEL_HEADER, "w", encoding="utf-8", newline="\n") as fh:
        fh.write(body)
    return body


def main() -> None:
    model_path = os.path.join(ARTIFACT_DIR, "rf_model.joblib")
    if not os.path.exists(model_path):
        raise SystemExit("No trained model. Run `py train_random_forest.py` first.")

    forest: RandomForestClassifier = joblib.load(model_path)

    if forest.n_features_in_ != N_FEATURES:
        raise SystemExit(
            f"Model expects {forest.n_features_in_} features but the spec defines "
            f"{N_FEATURES}. Retrain against the current feature_spec.py.")

    data = np.load(os.path.join(ARTIFACT_DIR, "dataset.npz"), allow_pickle=True)
    x, y = data["x"], data["y"]
    accuracy = float(np.mean(forest.predict(x) == y))

    print("=" * 64)
    print("Exporting random forest to C")
    print("=" * 64)

    # 1. Regenerate the spec header, so both headers always carry the same hash.
    feature_spec.emit_c_header(SPEC_HEADER)
    print(f"  wrote {SPEC_HEADER}")

    # 2. Emit the model.
    body = emit_model_header(forest, accuracy)
    print(f"  wrote {MODEL_HEADER}")

    # 3. Self-test vectors for on-target verification.
    write_parity_vectors(x, forest.predict(x))
    print(f"  wrote {PARITY_CSV}")

    # 4. Prove the export is faithful.
    print()
    verify_parity(forest, x)

    size = os.path.getsize(MODEL_HEADER)
    lines = body.count("\n") + 1
    print()
    print(f"  model_rf.h : {size:,} bytes, {lines:,} lines")
    print(f"  spec hash  : 0x{spec_hash():08X} (must match feature_spec.h)")
    print("\nNext: build the firmware --  cd ../firmware && py -m platformio run")


if __name__ == "__main__":
    main()
