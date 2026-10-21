# Edge-AI Predictive Maintenance Node

A condition-monitoring edge node for an industrial induction motor, built on a
Raspberry Pi Pico W and simulated in Wokwi.

It samples vibration, load current and bearing temperature, extracts eleven
diagnostic features in real time, runs a **random forest that was trained offline
in Python and compiled to C**, and classifies the machine as `HEALTHY`,
`EARLY_FAULT` or `CRITICAL` about four times a second — then drives a local
annunciator and publishes JSON telemetry.

No inference happens off-board. The network carries verdicts, not waveforms.

```
                    ┌──────────────────────────────────────────┐
   MPU6050 ────I2C──┤                                          │
   (vibration)      │  1. sample    512 @ 2 kHz → ring buffer  │
                    │                                          │
   pot ──────ADC0───┤  2. features  3 axes → PCA → 1 signal    │──► LED  ×3
   (CT / current)   │               time domain + 512-pt FFT   │──► buzzer
                    │               → 11 floats                │──► SSD1306 OLED
   pot ──────ADC1───┤                                          │
   (bearing temp)   │  3. infer     15-tree random forest,     │──► JSON telemetry
                    │               compiled to if/else        │    over serial
                    │                                          │    (#ifdef MQTT)
                    │  4. score     degradation index 0–100%   │
                    └──────────────────────────────────────────┘
                              Raspberry Pi Pico W
```

---

## Repository layout

```
predictive-maintenance-motor/
├── ml/                                  offline training pipeline
│   ├── feature_spec.py                  ★ the Python↔C contract; generates feature_spec.h
│   ├── extract_features.py              reference feature implementation
│   ├── generate_dataset.py              synthetic motor physics → labelled windows
│   ├── train_random_forest.py           train, evaluate, plot
│   ├── export_to_c.py                   forest → model_rf.h  (+ parity check)
│   ├── demo_sweep_check.py              proves the live demo reaches all 3 states
│   ├── lstm_autoencoder_scaling_path.ipynb   Tier-2, never deployed
│   ├── requirements.txt / requirements-tier2.txt
│   └── artifacts/                       dataset, model, plots  (generated)
├── firmware/
│   ├── platformio.ini
│   ├── wokwi.toml                       points at the built .uf2 / .elf
│   ├── diagram.json                     the Wokwi circuit
│   └── src/
│       ├── main.cpp                     pipeline, annunciation, telemetry
│       ├── features.cpp/.h              C mirror of extract_features.py
│       ├── vibration_source.cpp/.h      MPU6050 driver + Wokwi synthesis shim
│       ├── pins.h                       pin map
│       ├── feature_spec.h               ← GENERATED
│       ├── model_rf.h                   ← GENERATED
│       └── selftest_vectors.h           ← GENERATED
└── docs/portfolio_onepager.md
```

---

## Pin map (Raspberry Pi Pico W)

| Signal | GPIO | Notes |
|---|---|---|
| I2C0 SDA | GP4 | shared bus: MPU6050 `0x68` + SSD1306 `0x3C` |
| I2C0 SCL | GP5 | 400 kHz |
| Current (CT proxy) | GP26 / ADC0 | potentiometer |
| Bearing temperature | GP27 / ADC1 | potentiometer |
| LED green — HEALTHY | GP15 | via 220 Ω |
| LED yellow — EARLY_FAULT | GP14 | via 220 Ω |
| LED red — CRITICAL | GP13 | via 220 Ω |
| Buzzer | GP12 | chirps on CRITICAL only |

> **GP23, GP24, GP25 and GP29 are intentionally unused.** On the Pico W they
> belong to the CYW43439 radio, not the header. This is the one pin trap that
> differs from a plain Pico, where GP25 is the on-board LED.

---

## Running it

### 1. The ML pipeline

```bash
cd ml
py -m pip install -r requirements.txt

py generate_dataset.py        # 6000 labelled windows from the motor physics model
py train_random_forest.py     # trains, prints metrics, writes plots to artifacts/
py export_to_c.py             # writes firmware/src/feature_spec.h + model_rf.h
py demo_sweep_check.py        # verifies the Wokwi demo will reach all three states
```

Run them in that order — each consumes the previous one's output. `export_to_c.py`
regenerates **both** headers so their spec hashes can never disagree.

### 2. The firmware

```bash
cd firmware
py -m platformio run
```

### 3. The simulation

The Wokwi extension looks for `wokwi.toml` and `diagram.json` relative to the
workspace folder, and both live in `firmware/`. So **open `firmware/` as the
workspace folder** (or `File → Add Folder to Workspace`), then press **F1 →
"Wokwi: Start Simulator"**.

---

## The live demo

Once the simulator is running, the OLED shows the state, a degradation bar, and
live diagnostics. The serial monitor prints one JSON object per window.

**Click the MPU6050 in the diagram** to open its slider panel.

| Step | Action | What you should see |
|---|---|---|
| 1 | Leave `accelerationX` at 0 | Green LED. `HEALTHY`. Dominant frequency ~23 Hz — that is the 25 Hz shaft, i.e. ordinary residual imbalance. Kurtosis ~2.9 ≈ Gaussian. |
| 2 | Drag `accelerationX` to **≈ 0.6 g** | Yellow LED, `EARLY`. **Dominant frequency jumps to ~90 Hz** — the bearing outer-race defect frequency. Kurtosis climbs past 3.5 as impacts begin. Degradation bar ~40%. |
| 3 | Drag to **≈ 1.4 g** | Red LED, buzzer chirps, `CRITICAL`. Kurtosis ~5, mid-band energy ~0.38, degradation 70%+. |
| 4 | Turn the **current** pot up | Degradation rises further; confidence shifts. Shows the model fusing a second sensor. |
| 5 | Turn the **temperature** pot up | Same, via the thermal channel. |
| 6 | Drop `accelerationX` back to 0 but leave current/temp high | Returns to green or yellow. **This is the interesting one**: the model does not simply threshold on temperature. |

`py ml/demo_sweep_check.py` prints this whole sweep as a table, computed against
the real trained model, so you can rehearse it without opening the simulator.

### Why the MPU6050 slider is a "severity" knob

Wokwi's MPU6050 reports whatever **constant** value the sliders are set to — it
does not emit a waveform. Run a real feature pipeline against a flat line and the
variance is zero, the PCA projection is undefined, kurtosis is 0/0 and the
spectrum is empty. There is nothing to classify.

So the Wokwi build (`-DVIB_SOURCE_WOKWI_SIM=1`) reinterprets the slider as a
fault-severity input and synthesises the vibration waveform from the same physics
model as `generate_dataset.py`.

**This replaces the sensor, and nothing else.** The synthesised samples go through
the identical ring buffer, PCA projection, FFT, feature extraction and random
forest. The model genuinely executes on the Pico against genuinely computed
features. Building with `-e rpipicow_hardware` switches to the live MPU6050 stream
with no other change.

---

## How Python and C are kept in sync

This is the failure mode that quietly kills TinyML projects: the model is trained
on features computed one way and the firmware computes them a slightly different
way. Nothing crashes. The classifier just becomes wrong.

Three mechanisms, in increasing order of strength:

1. **One definition.** Sample rate, window length, FFT band edges (as integer bin
   indices, so rounding cannot drift), PCA iteration count and feature order all
   live once, in `ml/feature_spec.py`.
2. **Generated, not duplicated.** `export_to_c.py` writes
   `firmware/src/feature_spec.h` from those constants. The firmware does not
   hard-code any of them.
3. **A compile-time tripwire.** The spec is fingerprinted with FNV-1a. That hash
   is baked into both `feature_spec.h` and `model_rf.h`, and `main.cpp` has:

   ```cpp
   static_assert(MODEL_FEATURE_SPEC_HASH == FEATURE_SPEC_HASH, ...);
   ```

   Change the spec and forget to retrain, and **the build fails** instead of
   shipping a model that misreads its inputs.

`export_to_c.py` additionally re-runs the forest through a float32 Python
replica of the generated C and reports any disagreement (currently 0.25%, all of
it from sklearn's soft probability averaging vs. the firmware's hard majority
vote — see the note in that file).

### Verifying it on the target

The strongest check runs on the Pico itself. Uncomment `-DENABLE_SELFTEST` in
`platformio.ini`, rebuild, and watch the serial monitor at boot:

```
# SELFTEST A rf_predict      : 64/64  PASS
# SELFTEST B features_compute: 2 windows, worst rel err 0.000xxx on vib_kurtosis  PASS
```

- **A** feeds captured feature vectors into `rf_predict()` and checks the class —
  proving the *model* survived conversion to C.
- **B** feeds raw accelerometer windows through `features_compute()` and compares
  all 11 outputs against values `extract_features.py` computed from the identical
  samples — proving the *feature pipeline* agrees. This is the check that matters
  and the one that is hard to do any other way: it runs in float32, on the real
  core, against the ARM toolchain's libm, which is where the answer counts.

Costs ~16 kB of flash. Off by default.

> **Gotcha:** set the flag by editing `platformio.ini`, not via the
> `PLATFORMIO_BUILD_FLAGS` environment variable. PlatformIO does not treat
> env-var flag changes as a reason to rebuild, so you can silently get a stale
> binary. If you do use the env var, run `pio run -t clean` first.

---

## Results

| | |
|---|---|
| Test accuracy | **98.40%** (1500 held-out windows) |
| 5-fold cross-validation | 98.43% ± 0.36% |
| Model | 15 trees, max depth 8, 389 nodes |
| Features | 11 (5 time-domain, 4 spectral, 2 auxiliary) |
| Window | 512 samples @ 2 kHz = 256 ms → ~4 classifications/s |
| Export fidelity | 0.25% disagreement vs. scikit-learn |

Top permutation importances: `band_mid_ratio` 0.180 (the bearing-defect frequency
band), `vib_std` 0.116, `current_rms_a` 0.072, `vib_kurtosis` 0.062.

Sensor-group ablation — vibration alone 94.0%, current + temperature alone 82.7%,
everything 98.4%. Both groups contribute and neither is sufficient, which is what
makes the sensor fusion worth having (and what makes the demo respond to the
accelerometer alone).

---

## Troubleshooting

**`static_assert` failure about the feature spec.** You changed
`feature_spec.py` without retraining. Run `py train_random_forest.py && py
export_to_c.py`.

**Wokwi says it cannot find the firmware.** `wokwi.toml` points at
`.pio/build/rpipicow/firmware.uf2`, relative to `firmware/`. Build first, and make
sure `firmware/` is the open workspace folder.

**`ImportError: DLL load failed ... Application Control policy has blocked this
file`** (Windows 11 with Smart App Control). Windows blocks freshly-downloaded
unsigned binaries until its reputation service catches up. Wait a few minutes and
retry — it generally clears on its own. Do **not** disable Smart App Control to
work around this; once off it cannot be re-enabled without reinstalling Windows.

**OLED stays blank.** Check that the MPU6050 and SSD1306 are on the same bus
(GP4/GP5) and that the display address is `0x3C`. The boot screen prints
`MPU6050: ok` or `MISSING`.

---

## Limitations, stated plainly

- **The training data is synthetic.** The physics model is grounded (shaft
  harmonics, BPFO with load-zone sidebands, resonance ring-down, cage slip), but
  no real bearing was harmed. The honest next step is validating against the NASA
  IMS or Case Western Reserve run-to-failure datasets. Accuracy on a simulator you
  wrote yourself is, by itself, a statement about your simulator.
- **The degradation index is a heuristic, not an RUL model.** It is a documented
  weighted sum of five normalised features. It is labelled as such everywhere and
  is auditable by a maintenance engineer. A real RUL estimate needs run-to-failure
  history.
- **The supervised forest cannot flag novel faults.** It has three output slots
  and will confidently use one. This is the specific gap the Tier-2 notebook
  addresses, and the notebook demonstrates it concretely on an unseen inner-race
  fault.
- **A true 2 kHz stream from a real MPU6050** needs FIFO burst reads rather than
  one I2C transaction per sample. Noted in `vib_source_begin()`; it is the main
  hardware bring-up task.
