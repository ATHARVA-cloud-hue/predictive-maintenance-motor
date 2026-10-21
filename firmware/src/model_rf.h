/* -----------------------------------------------------------------------------
 * model_rf.h -- GENERATED FILE. DO NOT EDIT BY HAND.
 *
 * A scikit-learn RandomForestClassifier, expressed as nested if/else statements.
 * Produced by ml/export_to_c.py from ml/artifacts/rf_model.joblib.
 *
 *   generated     : 2026-10-21 10:37:04
 *   trees         : 15
 *   total nodes   : 1455 (720 decisions, 735 leaves)
 *   test accuracy : 98.87%
 *   classes       : 0=HEALTHY, 1=EARLY_FAULT, 2=CRITICAL
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
#define MODEL_FEATURE_SPEC_HASH 0x5BC1E2EDu

/* --- tree 0 (95 nodes) ---------------------------------------------- */
static uint8_t rf_tree_0(const float *f)
{
    if (f[6] <= 0.268944204f) {  /* band_low_ratio */
        if (f[5] <= 56.640625f) {  /* dom_freq_hz */
            if (f[1] <= 0.146559f) {  /* vib_peak */
                if (f[3] <= 3.43345118f) {  /* vib_kurtosis */
                    if (f[7] <= 0.173493505f) {  /* band_mid_ratio */
                        return 0; /* HEALTHY, 100% pure, n=1 */
                    } else {
                        return 0; /* HEALTHY, 50% pure, n=1 */
                    }
                } else {
                    if (f[3] <= 3.61746836f) {  /* vib_kurtosis */
                        return 0; /* HEALTHY, 50% pure, n=1 */
                    } else {
                        return 1; /* EARLY_FAULT, 100% pure, n=1 */
                    }
                }
            } else {
                if (f[3] <= 3.33105087f) {  /* vib_kurtosis */
                    if (f[2] <= 0.0976675376f) {  /* vib_std */
                        if (f[1] <= 0.160550386f) {  /* vib_peak */
                            return 0; /* HEALTHY, 100% pure, n=1 */
                        } else {
                            return 0; /* HEALTHY, 50% pure, n=1 */
                        }
                    } else {
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    }
                } else {
                    if (f[2] <= 0.13760142f) {  /* vib_std */
                        if (f[10] <= 80.4592209f) {  /* temp_c */
                            if (f[0] <= 1.05585492f) {  /* vib_rms */
                                if (f[9] <= 17.8951454f) {  /* current_rms_a */
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 75% pure, n=1 */
                                }
                            } else {
                                if (f[8] <= 0.706680417f) {  /* band_high_ratio */
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 50% pure, n=1 */
                                }
                            }
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    } else {
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    }
                }
            }
        } else {
            if (f[2] <= 0.104158938f) {  /* vib_std */
                if (f[2] <= 0.0707684532f) {  /* vib_std */
                    if (f[4] <= 3.12597847f) {  /* vib_crest */
                        if (f[6] <= 0.202081412f) {  /* band_low_ratio */
                            return 1; /* EARLY_FAULT, 57% pure, n=1 */
                        } else {
                            return 1; /* EARLY_FAULT, 100% pure, n=1 */
                        }
                    } else {
                        if (f[10] <= 79.7899323f) {  /* temp_c */
                            if (f[0] <= 1.03241301f) {  /* vib_rms */
                                return 1; /* EARLY_FAULT, 100% pure, n=1 */
                            } else {
                                if (f[9] <= 12.5426617f) {  /* current_rms_a */
                                    return 1; /* EARLY_FAULT, 80% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                }
                            }
                        } else {
                            return 1; /* EARLY_FAULT, 67% pure, n=1 */
                        }
                    }
                } else {
                    if (f[7] <= 0.535040855f) {  /* band_mid_ratio */
                        if (f[10] <= 80.0766754f) {  /* temp_c */
                            if (f[2] <= 0.0712189525f) {  /* vib_std */
                                return 2; /* CRITICAL, 67% pure, n=1 */
                            } else {
                                if (f[4] <= 4.44307709f) {  /* vib_crest */
                                    return 1; /* EARLY_FAULT, 77% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 91% pure, n=1 */
                                }
                            }
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    } else {
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    }
                }
            } else {
                if (f[2] <= 0.145340711f) {  /* vib_std */
                    if (f[10] <= 79.9360504f) {  /* temp_c */
                        if (f[9] <= 18.0460396f) {  /* current_rms_a */
                            if (f[10] <= 58.785347f) {  /* temp_c */
                                if (f[4] <= 4.46899796f) {  /* vib_crest */
                                    return 2; /* CRITICAL, 96% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 50% pure, n=1 */
                                }
                            } else {
                                if (f[8] <= 0.569007635f) {  /* band_high_ratio */
                                    return 1; /* EARLY_FAULT, 52% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 93% pure, n=1 */
                                }
                            }
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    } else {
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    }
                } else {
                    if (f[4] <= 5.55285931f) {  /* vib_crest */
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    } else {
                        return 2; /* CRITICAL, 89% pure, n=1 */
                    }
                }
            }
        }
    } else {
        if (f[1] <= 0.182696506f) {  /* vib_peak */
            if (f[3] <= 3.23621535f) {  /* vib_kurtosis */
                if (f[7] <= 0.158355355f) {  /* band_mid_ratio */
                    if (f[7] <= 0.12873441f) {  /* band_mid_ratio */
                        return 0; /* HEALTHY, 100% pure, n=1 */
                    } else {
                        if (f[3] <= 2.69598627f) {  /* vib_kurtosis */
                            return 0; /* HEALTHY, 60% pure, n=1 */
                        } else {
                            if (f[9] <= 14.4256783f) {  /* current_rms_a */
                                return 0; /* HEALTHY, 100% pure, n=1 */
                            } else {
                                return 0; /* HEALTHY, 88% pure, n=1 */
                            }
                        }
                    }
                } else {
                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                }
            } else {
                if (f[8] <= 0.574492455f) {  /* band_high_ratio */
                    if (f[10] <= 49.4807129f) {  /* temp_c */
                        return 0; /* HEALTHY, 86% pure, n=1 */
                    } else {
                        if (f[7] <= 0.135931298f) {  /* band_mid_ratio */
                            return 1; /* EARLY_FAULT, 80% pure, n=1 */
                        } else {
                            return 1; /* EARLY_FAULT, 100% pure, n=1 */
                        }
                    }
                } else {
                    if (f[1] <= 0.10612233f) {  /* vib_peak */
                        return 0; /* HEALTHY, 100% pure, n=1 */
                    } else {
                        return 1; /* EARLY_FAULT, 60% pure, n=1 */
                    }
                }
            }
        } else {
            if (f[6] <= 0.404162794f) {  /* band_low_ratio */
                if (f[10] <= 78.5593414f) {  /* temp_c */
                    if (f[3] <= 3.04233932f) {  /* vib_kurtosis */
                        return 1; /* EARLY_FAULT, 73% pure, n=1 */
                    } else {
                        if (f[2] <= 0.0740679651f) {  /* vib_std */
                            return 1; /* EARLY_FAULT, 100% pure, n=1 */
                        } else {
                            if (f[9] <= 16.8717804f) {  /* current_rms_a */
                                return 1; /* EARLY_FAULT, 100% pure, n=1 */
                            } else {
                                return 1; /* EARLY_FAULT, 83% pure, n=1 */
                            }
                        }
                    }
                } else {
                    return 2; /* CRITICAL, 86% pure, n=1 */
                }
            } else {
                return 0; /* HEALTHY, 50% pure, n=1 */
            }
        }
    }
}

/* --- tree 1 (69 nodes) ---------------------------------------------- */
static uint8_t rf_tree_1(const float *f)
{
    if (f[7] <= 0.120805703f) {  /* band_mid_ratio */
        if (f[3] <= 3.61825728f) {  /* vib_kurtosis */
            if (f[10] <= 89.9921341f) {  /* temp_c */
                if (f[6] <= 0.138134778f) {  /* band_low_ratio */
                    return 0; /* HEALTHY, 50% pure, n=1 */
                } else {
                    return 0; /* HEALTHY, 100% pure, n=1 */
                }
            } else {
                return 2; /* CRITICAL, 100% pure, n=1 */
            }
        } else {
            if (f[9] <= 18.3325195f) {  /* current_rms_a */
                if (f[2] <= 0.147085592f) {  /* vib_std */
                    if (f[2] <= 0.119810686f) {  /* vib_std */
                        return 1; /* EARLY_FAULT, 100% pure, n=1 */
                    } else {
                        return 1; /* EARLY_FAULT, 86% pure, n=1 */
                    }
                } else {
                    return 2; /* CRITICAL, 100% pure, n=1 */
                }
            } else {
                return 2; /* CRITICAL, 100% pure, n=1 */
            }
        }
    } else {
        if (f[9] <= 18.0180054f) {  /* current_rms_a */
            if (f[2] <= 0.120784871f) {  /* vib_std */
                if (f[10] <= 48.0387917f) {  /* temp_c */
                    if (f[5] <= 70.3125f) {  /* dom_freq_hz */
                        if (f[4] <= 3.49281073f) {  /* vib_crest */
                            return 0; /* HEALTHY, 100% pure, n=1 */
                        } else {
                            return 0; /* HEALTHY, 50% pure, n=1 */
                        }
                    } else {
                        return 2; /* CRITICAL, 92% pure, n=1 */
                    }
                } else {
                    if (f[3] <= 3.10685062f) {  /* vib_kurtosis */
                        if (f[8] <= 0.525465012f) {  /* band_high_ratio */
                            if (f[1] <= 0.0893647298f) {  /* vib_peak */
                                return 0; /* HEALTHY, 86% pure, n=1 */
                            } else {
                                if (f[5] <= 56.640625f) {  /* dom_freq_hz */
                                    return 1; /* EARLY_FAULT, 85% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 97% pure, n=1 */
                                }
                            }
                        } else {
                            if (f[2] <= 0.0507117957f) {  /* vib_std */
                                if (f[6] <= 0.322095335f) {  /* band_low_ratio */
                                    return 0; /* HEALTHY, 100% pure, n=1 */
                                } else {
                                    return 0; /* HEALTHY, 60% pure, n=1 */
                                }
                            } else {
                                return 2; /* CRITICAL, 86% pure, n=1 */
                            }
                        }
                    } else {
                        if (f[0] <= 1.07960391f) {  /* vib_rms */
                            if (f[1] <= 0.384398043f) {  /* vib_peak */
                                if (f[1] <= 0.31174475f) {  /* vib_peak */
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 96% pure, n=1 */
                                }
                            } else {
                                if (f[10] <= 80.4026489f) {  /* temp_c */
                                    return 1; /* EARLY_FAULT, 94% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                }
                            }
                        } else {
                            return 2; /* CRITICAL, 57% pure, n=1 */
                        }
                    }
                }
            } else {
                if (f[3] <= 5.76858854f) {  /* vib_kurtosis */
                    if (f[8] <= 0.606355309f) {  /* band_high_ratio */
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    } else {
                        if (f[2] <= 0.137310132f) {  /* vib_std */
                            if (f[0] <= 0.994362116f) {  /* vib_rms */
                                return 1; /* EARLY_FAULT, 57% pure, n=1 */
                            } else {
                                return 2; /* CRITICAL, 80% pure, n=1 */
                            }
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    }
                } else {
                    if (f[1] <= 0.69878006f) {  /* vib_peak */
                        if (f[10] <= 75.4444122f) {  /* temp_c */
                            if (f[10] <= 59.174572f) {  /* temp_c */
                                if (f[2] <= 0.133618101f) {  /* vib_std */
                                    return 2; /* CRITICAL, 80% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 75% pure, n=1 */
                                }
                            } else {
                                if (f[0] <= 0.995149374f) {  /* vib_rms */
                                    return 1; /* EARLY_FAULT, 75% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                }
                            }
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    } else {
                        if (f[8] <= 0.626673698f) {  /* band_high_ratio */
                            if (f[2] <= 0.191416696f) {  /* vib_std */
                                return 2; /* CRITICAL, 67% pure, n=1 */
                            } else {
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            }
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    }
                }
            }
        } else {
            if (f[2] <= 0.0558758378f) {  /* vib_std */
                return 0; /* HEALTHY, 100% pure, n=1 */
            } else {
                return 2; /* CRITICAL, 100% pure, n=1 */
            }
        }
    }
}

/* --- tree 2 (107 nodes) ---------------------------------------------- */
static uint8_t rf_tree_2(const float *f)
{
    if (f[3] <= 3.09447813f) {  /* vib_kurtosis */
        if (f[2] <= 0.0656513274f) {  /* vib_std */
            if (f[7] <= 0.178444237f) {  /* band_mid_ratio */
                if (f[9] <= 14.7269506f) {  /* current_rms_a */
                    return 0; /* HEALTHY, 100% pure, n=1 */
                } else {
                    if (f[2] <= 0.0306126606f) {  /* vib_std */
                        return 0; /* HEALTHY, 100% pure, n=1 */
                    } else {
                        if (f[1] <= 0.097516343f) {  /* vib_peak */
                            return 0; /* HEALTHY, 78% pure, n=1 */
                        } else {
                            return 0; /* HEALTHY, 100% pure, n=1 */
                        }
                    }
                }
            } else {
                if (f[8] <= 0.506897211f) {  /* band_high_ratio */
                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                } else {
                    return 1; /* EARLY_FAULT, 71% pure, n=1 */
                }
            }
        } else {
            if (f[10] <= 76.5810852f) {  /* temp_c */
                if (f[2] <= 0.0910704806f) {  /* vib_std */
                    if (f[9] <= 12.684741f) {  /* current_rms_a */
                        return 1; /* EARLY_FAULT, 83% pure, n=1 */
                    } else {
                        return 1; /* EARLY_FAULT, 100% pure, n=1 */
                    }
                } else {
                    if (f[8] <= 0.54483813f) {  /* band_high_ratio */
                        if (f[1] <= 0.356188446f) {  /* vib_peak */
                            return 1; /* EARLY_FAULT, 50% pure, n=1 */
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    } else {
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    }
                }
            } else {
                if (f[6] <= 0.188939959f) {  /* band_low_ratio */
                    return 2; /* CRITICAL, 100% pure, n=1 */
                } else {
                    return 2; /* CRITICAL, 75% pure, n=1 */
                }
            }
        }
    } else {
        if (f[6] <= 0.103042275f) {  /* band_low_ratio */
            if (f[9] <= 17.9666862f) {  /* current_rms_a */
                if (f[8] <= 0.650194049f) {  /* band_high_ratio */
                    if (f[10] <= 79.6085358f) {  /* temp_c */
                        if (f[0] <= 1.04567218f) {  /* vib_rms */
                            if (f[1] <= 0.546679258f) {  /* vib_peak */
                                if (f[10] <= 48.411335f) {  /* temp_c */
                                    return 2; /* CRITICAL, 67% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 97% pure, n=1 */
                                }
                            } else {
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            }
                        } else {
                            if (f[2] <= 0.113894984f) {  /* vib_std */
                                return 1; /* EARLY_FAULT, 100% pure, n=1 */
                            } else {
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            }
                        }
                    } else {
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    }
                } else {
                    if (f[1] <= 0.668235302f) {  /* vib_peak */
                        if (f[6] <= 0.101774856f) {  /* band_low_ratio */
                            if (f[2] <= 0.0959175229f) {  /* vib_std */
                                return 1; /* EARLY_FAULT, 100% pure, n=1 */
                            } else {
                                if (f[6] <= 0.0757858604f) {  /* band_low_ratio */
                                    return 1; /* EARLY_FAULT, 73% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 96% pure, n=1 */
                                }
                            }
                        } else {
                            return 2; /* CRITICAL, 60% pure, n=1 */
                        }
                    } else {
                        if (f[2] <= 0.140183985f) {  /* vib_std */
                            return 1; /* EARLY_FAULT, 50% pure, n=1 */
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    }
                }
            } else {
                return 2; /* CRITICAL, 100% pure, n=1 */
            }
        } else {
            if (f[4] <= 3.70296335f) {  /* vib_crest */
                if (f[1] <= 0.315775156f) {  /* vib_peak */
                    if (f[2] <= 0.0265504681f) {  /* vib_std */
                        if (f[5] <= 25.390625f) {  /* dom_freq_hz */
                            return 0; /* HEALTHY, 100% pure, n=1 */
                        } else {
                            if (f[9] <= 13.5058928f) {  /* current_rms_a */
                                return 0; /* HEALTHY, 100% pure, n=1 */
                            } else {
                                return 0; /* HEALTHY, 83% pure, n=1 */
                            }
                        }
                    } else {
                        if (f[9] <= 11.6704807f) {  /* current_rms_a */
                            return 0; /* HEALTHY, 100% pure, n=1 */
                        } else {
                            if (f[10] <= 50.7139893f) {  /* temp_c */
                                if (f[7] <= 0.155233145f) {  /* band_mid_ratio */
                                    return 0; /* HEALTHY, 100% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 87% pure, n=1 */
                                }
                            } else {
                                if (f[10] <= 79.4580383f) {  /* temp_c */
                                    return 1; /* EARLY_FAULT, 97% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 57% pure, n=1 */
                                }
                            }
                        }
                    }
                } else {
                    if (f[1] <= 0.401625991f) {  /* vib_peak */
                        if (f[3] <= 3.45888042f) {  /* vib_kurtosis */
                            return 1; /* EARLY_FAULT, 71% pure, n=1 */
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    } else {
                        if (f[8] <= 0.38468951f) {  /* band_high_ratio */
                            if (f[2] <= 0.174983442f) {  /* vib_std */
                                return 2; /* CRITICAL, 75% pure, n=1 */
                            } else {
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            }
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    }
                }
            } else {
                if (f[2] <= 0.12598443f) {  /* vib_std */
                    if (f[1] <= 0.0766410083f) {  /* vib_peak */
                        if (f[7] <= 0.147323728f) {  /* band_mid_ratio */
                            return 0; /* HEALTHY, 100% pure, n=1 */
                        } else {
                            return 1; /* EARLY_FAULT, 60% pure, n=1 */
                        }
                    } else {
                        if (f[7] <= 0.423229694f) {  /* band_mid_ratio */
                            if (f[2] <= 0.0738934949f) {  /* vib_std */
                                if (f[5] <= 70.3125f) {  /* dom_freq_hz */
                                    return 1; /* EARLY_FAULT, 92% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 99% pure, n=1 */
                                }
                            } else {
                                if (f[2] <= 0.0746464208f) {  /* vib_std */
                                    return 2; /* CRITICAL, 75% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 79% pure, n=1 */
                                }
                            }
                        } else {
                            if (f[1] <= 0.288202584f) {  /* vib_peak */
                                return 1; /* EARLY_FAULT, 100% pure, n=1 */
                            } else {
                                if (f[7] <= 0.451552689f) {  /* band_mid_ratio */
                                    return 2; /* CRITICAL, 89% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 62% pure, n=1 */
                                }
                            }
                        }
                    }
                } else {
                    if (f[9] <= 16.7098045f) {  /* current_rms_a */
                        if (f[10] <= 75.2301331f) {  /* temp_c */
                            if (f[10] <= 61.0653992f) {  /* temp_c */
                                if (f[4] <= 4.81336498f) {  /* vib_crest */
                                    return 2; /* CRITICAL, 92% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 60% pure, n=1 */
                                }
                            } else {
                                if (f[2] <= 0.145891815f) {  /* vib_std */
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 60% pure, n=1 */
                                }
                            }
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    } else {
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    }
                }
            }
        }
    }
}

/* --- tree 3 (101 nodes) ---------------------------------------------- */
static uint8_t rf_tree_3(const float *f)
{
    if (f[2] <= 0.095900923f) {  /* vib_std */
        if (f[3] <= 3.22165394f) {  /* vib_kurtosis */
            if (f[1] <= 0.185935438f) {  /* vib_peak */
                if (f[5] <= 70.3125f) {  /* dom_freq_hz */
                    if (f[1] <= 0.0928680152f) {  /* vib_peak */
                        if (f[0] <= 1.05920887f) {  /* vib_rms */
                            if (f[9] <= 12.9946747f) {  /* current_rms_a */
                                return 0; /* HEALTHY, 100% pure, n=1 */
                            } else {
                                if (f[4] <= 3.27079582f) {  /* vib_crest */
                                    return 0; /* HEALTHY, 100% pure, n=1 */
                                } else {
                                    return 0; /* HEALTHY, 98% pure, n=1 */
                                }
                            }
                        } else {
                            if (f[7] <= 0.115344509f) {  /* band_mid_ratio */
                                return 0; /* HEALTHY, 100% pure, n=1 */
                            } else {
                                return 0; /* HEALTHY, 80% pure, n=1 */
                            }
                        }
                    } else {
                        if (f[7] <= 0.169877499f) {  /* band_mid_ratio */
                            if (f[10] <= 72.3464737f) {  /* temp_c */
                                if (f[1] <= 0.0934586152f) {  /* vib_peak */
                                    return 0; /* HEALTHY, 86% pure, n=1 */
                                } else {
                                    return 0; /* HEALTHY, 100% pure, n=1 */
                                }
                            } else {
                                if (f[4] <= 2.72279668f) {  /* vib_crest */
                                    return 0; /* HEALTHY, 50% pure, n=1 */
                                } else {
                                    return 0; /* HEALTHY, 100% pure, n=1 */
                                }
                            }
                        } else {
                            return 1; /* EARLY_FAULT, 100% pure, n=1 */
                        }
                    }
                } else {
                    if (f[8] <= 0.460557818f) {  /* band_high_ratio */
                        return 1; /* EARLY_FAULT, 100% pure, n=1 */
                    } else {
                        return 1; /* EARLY_FAULT, 86% pure, n=1 */
                    }
                }
            } else {
                if (f[6] <= 0.131905198f) {  /* band_low_ratio */
                    return 2; /* CRITICAL, 73% pure, n=1 */
                } else {
                    if (f[7] <= 0.152611136f) {  /* band_mid_ratio */
                        return 0; /* HEALTHY, 80% pure, n=1 */
                    } else {
                        if (f[9] <= 12.3393841f) {  /* current_rms_a */
                            return 1; /* EARLY_FAULT, 80% pure, n=1 */
                        } else {
                            if (f[9] <= 17.1827679f) {  /* current_rms_a */
                                return 1; /* EARLY_FAULT, 100% pure, n=1 */
                            } else {
                                return 1; /* EARLY_FAULT, 80% pure, n=1 */
                            }
                        }
                    }
                }
            }
        } else {
            if (f[9] <= 18.0731392f) {  /* current_rms_a */
                if (f[10] <= 47.2198486f) {  /* temp_c */
                    return 0; /* HEALTHY, 100% pure, n=1 */
                } else {
                    if (f[9] <= 11.7631855f) {  /* current_rms_a */
                        return 0; /* HEALTHY, 100% pure, n=1 */
                    } else {
                        if (f[6] <= 0.042338118f) {  /* band_low_ratio */
                            return 1; /* EARLY_FAULT, 70% pure, n=1 */
                        } else {
                            if (f[1] <= 0.0821455121f) {  /* vib_peak */
                                return 1; /* EARLY_FAULT, 57% pure, n=1 */
                            } else {
                                if (f[1] <= 0.345094264f) {  /* vib_peak */
                                    return 1; /* EARLY_FAULT, 99% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 92% pure, n=1 */
                                }
                            }
                        }
                    }
                }
            } else {
                if (f[3] <= 3.43127441f) {  /* vib_kurtosis */
                    return 0; /* HEALTHY, 62% pure, n=1 */
                } else {
                    return 2; /* CRITICAL, 100% pure, n=1 */
                }
            }
        }
    } else {
        if (f[1] <= 0.700379729f) {  /* vib_peak */
            if (f[4] <= 4.04192543f) {  /* vib_crest */
                if (f[9] <= 17.7933598f) {  /* current_rms_a */
                    if (f[3] <= 4.90579033f) {  /* vib_kurtosis */
                        if (f[6] <= 0.207644045f) {  /* band_low_ratio */
                            if (f[1] <= 0.41161567f) {  /* vib_peak */
                                if (f[3] <= 3.21076059f) {  /* vib_kurtosis */
                                    return 2; /* CRITICAL, 96% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                }
                            } else {
                                if (f[4] <= 3.86415005f) {  /* vib_crest */
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 95% pure, n=1 */
                                }
                            }
                        } else {
                            return 1; /* EARLY_FAULT, 82% pure, n=1 */
                        }
                    } else {
                        return 1; /* EARLY_FAULT, 100% pure, n=1 */
                    }
                } else {
                    return 2; /* CRITICAL, 100% pure, n=1 */
                }
            } else {
                if (f[7] <= 0.224538088f) {  /* band_mid_ratio */
                    if (f[9] <= 18.043293f) {  /* current_rms_a */
                        if (f[7] <= 0.152420893f) {  /* band_mid_ratio */
                            if (f[10] <= 76.8329773f) {  /* temp_c */
                                return 1; /* EARLY_FAULT, 100% pure, n=1 */
                            } else {
                                return 1; /* EARLY_FAULT, 83% pure, n=1 */
                            }
                        } else {
                            if (f[3] <= 7.96138239f) {  /* vib_kurtosis */
                                if (f[10] <= 75.6334686f) {  /* temp_c */
                                    return 1; /* EARLY_FAULT, 94% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 56% pure, n=1 */
                                }
                            } else {
                                return 2; /* CRITICAL, 60% pure, n=1 */
                            }
                        }
                    } else {
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    }
                } else {
                    if (f[3] <= 6.41362f) {  /* vib_kurtosis */
                        if (f[6] <= 0.0889919028f) {  /* band_low_ratio */
                            if (f[3] <= 5.95213652f) {  /* vib_kurtosis */
                                if (f[9] <= 14.9682446f) {  /* current_rms_a */
                                    return 2; /* CRITICAL, 67% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                }
                            } else {
                                if (f[10] <= 64.1267319f) {  /* temp_c */
                                    return 1; /* EARLY_FAULT, 67% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 88% pure, n=1 */
                                }
                            }
                        } else {
                            if (f[9] <= 17.973465f) {  /* current_rms_a */
                                if (f[8] <= 0.487211913f) {  /* band_high_ratio */
                                    return 2; /* CRITICAL, 64% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 79% pure, n=1 */
                                }
                            } else {
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            }
                        }
                    } else {
                        if (f[2] <= 0.132858574f) {  /* vib_std */
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        } else {
                            if (f[2] <= 0.136998802f) {  /* vib_std */
                                return 1; /* EARLY_FAULT, 50% pure, n=1 */
                            } else {
                                return 2; /* CRITICAL, 88% pure, n=1 */
                            }
                        }
                    }
                }
            }
        } else {
            if (f[4] <= 5.55227709f) {  /* vib_crest */
                return 2; /* CRITICAL, 100% pure, n=1 */
            } else {
                if (f[9] <= 18.1054745f) {  /* current_rms_a */
                    return 2; /* CRITICAL, 57% pure, n=1 */
                } else {
                    return 2; /* CRITICAL, 100% pure, n=1 */
                }
            }
        }
    }
}

/* --- tree 4 (105 nodes) ---------------------------------------------- */
static uint8_t rf_tree_4(const float *f)
{
    if (f[7] <= 0.13268052f) {  /* band_mid_ratio */
        if (f[6] <= 0.214517921f) {  /* band_low_ratio */
            if (f[2] <= 0.0271862354f) {  /* vib_std */
                return 0; /* HEALTHY, 100% pure, n=1 */
            } else {
                if (f[3] <= 4.34703827f) {  /* vib_kurtosis */
                    if (f[8] <= 0.749277592f) {  /* band_high_ratio */
                        if (f[10] <= 77.9226532f) {  /* temp_c */
                            if (f[7] <= 0.124971367f) {  /* band_mid_ratio */
                                return 0; /* HEALTHY, 100% pure, n=1 */
                            } else {
                                return 0; /* HEALTHY, 60% pure, n=1 */
                            }
                        } else {
                            return 2; /* CRITICAL, 83% pure, n=1 */
                        }
                    } else {
                        if (f[7] <= 0.120688103f) {  /* band_mid_ratio */
                            return 2; /* CRITICAL, 83% pure, n=1 */
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    }
                } else {
                    if (f[9] <= 18.3325195f) {  /* current_rms_a */
                        if (f[10] <= 79.99086f) {  /* temp_c */
                            if (f[3] <= 7.61135674f) {  /* vib_kurtosis */
                                return 1; /* EARLY_FAULT, 100% pure, n=1 */
                            } else {
                                if (f[4] <= 5.09642982f) {  /* vib_crest */
                                    return 1; /* EARLY_FAULT, 80% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                }
                            }
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    } else {
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    }
                }
            }
        } else {
            if (f[1] <= 0.187197328f) {  /* vib_peak */
                if (f[7] <= 0.121372283f) {  /* band_mid_ratio */
                    if (f[4] <= 4.13190079f) {  /* vib_crest */
                        return 0; /* HEALTHY, 100% pure, n=1 */
                    } else {
                        return 0; /* HEALTHY, 50% pure, n=1 */
                    }
                } else {
                    if (f[1] <= 0.0907955021f) {  /* vib_peak */
                        return 0; /* HEALTHY, 100% pure, n=1 */
                    } else {
                        if (f[10] <= 50.9561424f) {  /* temp_c */
                            return 0; /* HEALTHY, 100% pure, n=1 */
                        } else {
                            return 1; /* EARLY_FAULT, 90% pure, n=1 */
                        }
                    }
                }
            } else {
                if (f[3] <= 3.22402096f) {  /* vib_kurtosis */
                    return 0; /* HEALTHY, 71% pure, n=1 */
                } else {
                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                }
            }
        }
    } else {
        if (f[2] <= 0.104191989f) {  /* vib_std */
            if (f[1] <= 0.0869206339f) {  /* vib_peak */
                if (f[1] <= 0.0695104897f) {  /* vib_peak */
                    return 0; /* HEALTHY, 100% pure, n=1 */
                } else {
                    if (f[8] <= 0.504765511f) {  /* band_high_ratio */
                        return 1; /* EARLY_FAULT, 100% pure, n=1 */
                    } else {
                        if (f[1] <= 0.0721274614f) {  /* vib_peak */
                            return 0; /* HEALTHY, 60% pure, n=1 */
                        } else {
                            return 0; /* HEALTHY, 100% pure, n=1 */
                        }
                    }
                }
            } else {
                if (f[10] <= 80.1103973f) {  /* temp_c */
                    if (f[2] <= 0.0884044841f) {  /* vib_std */
                        if (f[7] <= 0.154545918f) {  /* band_mid_ratio */
                            if (f[1] <= 0.146866038f) {  /* vib_peak */
                                if (f[2] <= 0.0294306912f) {  /* vib_std */
                                    return 1; /* EARLY_FAULT, 88% pure, n=1 */
                                } else {
                                    return 0; /* HEALTHY, 67% pure, n=1 */
                                }
                            } else {
                                if (f[8] <= 0.522499442f) {  /* band_high_ratio */
                                    return 1; /* EARLY_FAULT, 83% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                }
                            }
                        } else {
                            if (f[2] <= 0.0710566938f) {  /* vib_std */
                                if (f[6] <= 0.249729097f) {  /* band_low_ratio */
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 99% pure, n=1 */
                                }
                            } else {
                                if (f[3] <= 2.84883356f) {  /* vib_kurtosis */
                                    return 1; /* EARLY_FAULT, 60% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 96% pure, n=1 */
                                }
                            }
                        }
                    } else {
                        if (f[6] <= 0.0613380596f) {  /* band_low_ratio */
                            if (f[2] <= 0.0944834799f) {  /* vib_std */
                                if (f[7] <= 0.346444309f) {  /* band_mid_ratio */
                                    return 1; /* EARLY_FAULT, 75% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 83% pure, n=1 */
                                }
                            } else {
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            }
                        } else {
                            if (f[0] <= 1.04957378f) {  /* vib_rms */
                                if (f[6] <= 0.0841974914f) {  /* band_low_ratio */
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 73% pure, n=1 */
                                }
                            } else {
                                return 2; /* CRITICAL, 89% pure, n=1 */
                            }
                        }
                    }
                } else {
                    if (f[10] <= 87.4182053f) {  /* temp_c */
                        return 2; /* CRITICAL, 80% pure, n=1 */
                    } else {
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    }
                }
            }
        } else {
            if (f[9] <= 15.339222f) {  /* current_rms_a */
                if (f[2] <= 0.145614862f) {  /* vib_std */
                    if (f[7] <= 0.342760354f) {  /* band_mid_ratio */
                        if (f[3] <= 3.20138025f) {  /* vib_kurtosis */
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        } else {
                            if (f[4] <= 5.29071522f) {  /* vib_crest */
                                if (f[0] <= 1.03147006f) {  /* vib_rms */
                                    return 1; /* EARLY_FAULT, 82% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 53% pure, n=1 */
                                }
                            } else {
                                return 2; /* CRITICAL, 90% pure, n=1 */
                            }
                        }
                    } else {
                        if (f[10] <= 57.8389435f) {  /* temp_c */
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        } else {
                            return 1; /* EARLY_FAULT, 57% pure, n=1 */
                        }
                    }
                } else {
                    return 2; /* CRITICAL, 100% pure, n=1 */
                }
            } else {
                if (f[1] <= 0.672029674f) {  /* vib_peak */
                    if (f[9] <= 17.5947571f) {  /* current_rms_a */
                        if (f[4] <= 4.71101665f) {  /* vib_crest */
                            if (f[2] <= 0.122252636f) {  /* vib_std */
                                if (f[10] <= 76.4108734f) {  /* temp_c */
                                    return 1; /* EARLY_FAULT, 90% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 88% pure, n=1 */
                                }
                            } else {
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            }
                        } else {
                            if (f[4] <= 5.22361469f) {  /* vib_crest */
                                return 1; /* EARLY_FAULT, 100% pure, n=1 */
                            } else {
                                return 1; /* EARLY_FAULT, 83% pure, n=1 */
                            }
                        }
                    } else {
                        if (f[9] <= 17.9734764f) {  /* current_rms_a */
                            return 2; /* CRITICAL, 92% pure, n=1 */
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    }
                } else {
                    return 2; /* CRITICAL, 100% pure, n=1 */
                }
            }
        }
    }
}

/* --- tree 5 (79 nodes) ---------------------------------------------- */
static uint8_t rf_tree_5(const float *f)
{
    if (f[2] <= 0.094139263f) {  /* vib_std */
        if (f[3] <= 3.2273016f) {  /* vib_kurtosis */
            if (f[7] <= 0.17883426f) {  /* band_mid_ratio */
                if (f[7] <= 0.12873441f) {  /* band_mid_ratio */
                    return 0; /* HEALTHY, 100% pure, n=1 */
                } else {
                    if (f[2] <= 0.0436310656f) {  /* vib_std */
                        if (f[7] <= 0.129771888f) {  /* band_mid_ratio */
                            return 0; /* HEALTHY, 75% pure, n=1 */
                        } else {
                            if (f[3] <= 3.16336679f) {  /* vib_kurtosis */
                                return 0; /* HEALTHY, 100% pure, n=1 */
                            } else {
                                return 0; /* HEALTHY, 80% pure, n=1 */
                            }
                        }
                    } else {
                        return 0; /* HEALTHY, 75% pure, n=1 */
                    }
                }
            } else {
                if (f[2] <= 0.0881520063f) {  /* vib_std */
                    if (f[2] <= 0.0827855617f) {  /* vib_std */
                        return 1; /* EARLY_FAULT, 100% pure, n=1 */
                    } else {
                        return 1; /* EARLY_FAULT, 80% pure, n=1 */
                    }
                } else {
                    return 2; /* CRITICAL, 86% pure, n=1 */
                }
            }
        } else {
            if (f[9] <= 18.0864067f) {  /* current_rms_a */
                if (f[9] <= 11.9926205f) {  /* current_rms_a */
                    return 0; /* HEALTHY, 100% pure, n=1 */
                } else {
                    if (f[8] <= 0.563741326f) {  /* band_high_ratio */
                        if (f[6] <= 0.0863265544f) {  /* band_low_ratio */
                            if (f[4] <= 4.3191433f) {  /* vib_crest */
                                return 1; /* EARLY_FAULT, 100% pure, n=1 */
                            } else {
                                return 1; /* EARLY_FAULT, 64% pure, n=1 */
                            }
                        } else {
                            if (f[1] <= 0.345094264f) {  /* vib_peak */
                                if (f[3] <= 3.23428774f) {  /* vib_kurtosis */
                                    return 1; /* EARLY_FAULT, 80% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 99% pure, n=1 */
                                }
                            } else {
                                if (f[2] <= 0.0849843472f) {  /* vib_std */
                                    return 2; /* CRITICAL, 60% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 94% pure, n=1 */
                                }
                            }
                        }
                    } else {
                        if (f[3] <= 3.38978219f) {  /* vib_kurtosis */
                            return 0; /* HEALTHY, 50% pure, n=1 */
                        } else {
                            if (f[1] <= 0.105840385f) {  /* vib_peak */
                                return 1; /* EARLY_FAULT, 80% pure, n=1 */
                            } else {
                                if (f[1] <= 0.344619811f) {  /* vib_peak */
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 98% pure, n=1 */
                                }
                            }
                        }
                    }
                }
            } else {
                if (f[6] <= 0.211244494f) {  /* band_low_ratio */
                    return 2; /* CRITICAL, 100% pure, n=1 */
                } else {
                    return 0; /* HEALTHY, 50% pure, n=1 */
                }
            }
        }
    } else {
        if (f[10] <= 79.5288239f) {  /* temp_c */
            if (f[2] <= 0.145614862f) {  /* vib_std */
                if (f[3] <= 4.96345234f) {  /* vib_kurtosis */
                    if (f[6] <= 0.216040581f) {  /* band_low_ratio */
                        if (f[9] <= 17.8882179f) {  /* current_rms_a */
                            if (f[9] <= 13.8930607f) {  /* current_rms_a */
                                if (f[4] <= 4.01875544f) {  /* vib_crest */
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 71% pure, n=1 */
                                }
                            } else {
                                if (f[6] <= 0.0812756792f) {  /* band_low_ratio */
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 64% pure, n=1 */
                                }
                            }
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    } else {
                        if (f[10] <= 63.841465f) {  /* temp_c */
                            return 1; /* EARLY_FAULT, 100% pure, n=1 */
                        } else {
                            return 1; /* EARLY_FAULT, 80% pure, n=1 */
                        }
                    }
                } else {
                    if (f[9] <= 17.9786186f) {  /* current_rms_a */
                        if (f[6] <= 0.0876339749f) {  /* band_low_ratio */
                            if (f[4] <= 5.10723782f) {  /* vib_crest */
                                if (f[7] <= 0.33262378f) {  /* band_mid_ratio */
                                    return 1; /* EARLY_FAULT, 88% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 86% pure, n=1 */
                                }
                            } else {
                                return 2; /* CRITICAL, 64% pure, n=1 */
                            }
                        } else {
                            if (f[9] <= 14.8873663f) {  /* current_rms_a */
                                if (f[10] <= 52.5466423f) {  /* temp_c */
                                    return 1; /* EARLY_FAULT, 83% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                }
                            } else {
                                if (f[4] <= 4.49612427f) {  /* vib_crest */
                                    return 1; /* EARLY_FAULT, 79% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 97% pure, n=1 */
                                }
                            }
                        }
                    } else {
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    }
                }
            } else {
                return 2; /* CRITICAL, 100% pure, n=1 */
            }
        } else {
            if (f[6] <= 0.175656885f) {  /* band_low_ratio */
                return 2; /* CRITICAL, 100% pure, n=1 */
            } else {
                if (f[2] <= 0.101192385f) {  /* vib_std */
                    return 2; /* CRITICAL, 89% pure, n=1 */
                } else {
                    return 2; /* CRITICAL, 100% pure, n=1 */
                }
            }
        }
    }
}

/* --- tree 6 (117 nodes) ---------------------------------------------- */
static uint8_t rf_tree_6(const float *f)
{
    if (f[2] <= 0.0951114595f) {  /* vib_std */
        if (f[1] <= 0.148877576f) {  /* vib_peak */
            if (f[3] <= 3.39352345f) {  /* vib_kurtosis */
                if (f[7] <= 0.171022713f) {  /* band_mid_ratio */
                    if (f[1] <= 0.0928601101f) {  /* vib_peak */
                        return 0; /* HEALTHY, 100% pure, n=1 */
                    } else {
                        if (f[3] <= 3.17232466f) {  /* vib_kurtosis */
                            if (f[0] <= 0.996960878f) {  /* vib_rms */
                                return 0; /* HEALTHY, 100% pure, n=1 */
                            } else {
                                if (f[2] <= 0.0308287125f) {  /* vib_std */
                                    return 0; /* HEALTHY, 93% pure, n=1 */
                                } else {
                                    return 0; /* HEALTHY, 100% pure, n=1 */
                                }
                            }
                        } else {
                            if (f[9] <= 13.512886f) {  /* current_rms_a */
                                return 0; /* HEALTHY, 100% pure, n=1 */
                            } else {
                                return 0; /* HEALTHY, 50% pure, n=1 */
                            }
                        }
                    }
                } else {
                    if (f[6] <= 0.16756095f) {  /* band_low_ratio */
                        if (f[7] <= 0.425690711f) {  /* band_mid_ratio */
                            return 1; /* EARLY_FAULT, 80% pure, n=1 */
                        } else {
                            return 1; /* EARLY_FAULT, 100% pure, n=1 */
                        }
                    } else {
                        return 1; /* EARLY_FAULT, 100% pure, n=1 */
                    }
                }
            } else {
                if (f[9] <= 12.1249838f) {  /* current_rms_a */
                    return 0; /* HEALTHY, 75% pure, n=1 */
                } else {
                    if (f[3] <= 3.51539564f) {  /* vib_kurtosis */
                        if (f[6] <= 0.231457204f) {  /* band_low_ratio */
                            return 1; /* EARLY_FAULT, 57% pure, n=1 */
                        } else {
                            return 1; /* EARLY_FAULT, 85% pure, n=1 */
                        }
                    } else {
                        if (f[2] <= 0.0230540056f) {  /* vib_std */
                            return 1; /* EARLY_FAULT, 75% pure, n=1 */
                        } else {
                            return 1; /* EARLY_FAULT, 100% pure, n=1 */
                        }
                    }
                }
            }
        } else {
            if (f[3] <= 3.07131386f) {  /* vib_kurtosis */
                if (f[5] <= 58.59375f) {  /* dom_freq_hz */
                    if (f[10] <= 64.4569092f) {  /* temp_c */
                        if (f[6] <= 0.237902135f) {  /* band_low_ratio */
                            return 0; /* HEALTHY, 86% pure, n=1 */
                        } else {
                            return 0; /* HEALTHY, 100% pure, n=1 */
                        }
                    } else {
                        return 0; /* HEALTHY, 57% pure, n=1 */
                    }
                } else {
                    if (f[9] <= 19.0282974f) {  /* current_rms_a */
                        if (f[2] <= 0.0865897238f) {  /* vib_std */
                            return 1; /* EARLY_FAULT, 100% pure, n=1 */
                        } else {
                            return 1; /* EARLY_FAULT, 83% pure, n=1 */
                        }
                    } else {
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    }
                }
            } else {
                if (f[9] <= 18.1941795f) {  /* current_rms_a */
                    if (f[1] <= 0.344619811f) {  /* vib_peak */
                        if (f[9] <= 12.0683441f) {  /* current_rms_a */
                            return 0; /* HEALTHY, 50% pure, n=1 */
                        } else {
                            if (f[6] <= 0.453087628f) {  /* band_low_ratio */
                                if (f[10] <= 48.6141243f) {  /* temp_c */
                                    return 1; /* EARLY_FAULT, 87% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                }
                            } else {
                                return 0; /* HEALTHY, 50% pure, n=1 */
                            }
                        }
                    } else {
                        if (f[10] <= 86.4663391f) {  /* temp_c */
                            if (f[7] <= 0.211146206f) {  /* band_mid_ratio */
                                return 1; /* EARLY_FAULT, 100% pure, n=1 */
                            } else {
                                if (f[9] <= 12.8107653f) {  /* current_rms_a */
                                    return 1; /* EARLY_FAULT, 85% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 97% pure, n=1 */
                                }
                            }
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    }
                } else {
                    return 2; /* CRITICAL, 100% pure, n=1 */
                }
            }
        }
    } else {
        if (f[2] <= 0.139864117f) {  /* vib_std */
            if (f[4] <= 4.04192543f) {  /* vib_crest */
                if (f[10] <= 80.0059204f) {  /* temp_c */
                    if (f[0] <= 1.03701663f) {  /* vib_rms */
                        if (f[9] <= 17.6907921f) {  /* current_rms_a */
                            if (f[9] <= 15.1821346f) {  /* current_rms_a */
                                if (f[6] <= 0.185009673f) {  /* band_low_ratio */
                                    return 2; /* CRITICAL, 90% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 75% pure, n=1 */
                                }
                            } else {
                                if (f[1] <= 0.416589379f) {  /* vib_peak */
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 62% pure, n=1 */
                                }
                            }
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    } else {
                        if (f[6] <= 0.106839739f) {  /* band_low_ratio */
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        } else {
                            return 2; /* CRITICAL, 86% pure, n=1 */
                        }
                    }
                } else {
                    return 2; /* CRITICAL, 100% pure, n=1 */
                }
            } else {
                if (f[7] <= 0.192041487f) {  /* band_mid_ratio */
                    if (f[8] <= 0.709327638f) {  /* band_high_ratio */
                        return 1; /* EARLY_FAULT, 100% pure, n=1 */
                    } else {
                        if (f[10] <= 77.7726746f) {  /* temp_c */
                            if (f[8] <= 0.721067131f) {  /* band_high_ratio */
                                return 2; /* CRITICAL, 57% pure, n=1 */
                            } else {
                                if (f[9] <= 12.6886129f) {  /* current_rms_a */
                                    return 1; /* EARLY_FAULT, 67% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 97% pure, n=1 */
                                }
                            }
                        } else {
                            if (f[0] <= 0.993667126f) {  /* vib_rms */
                                return 2; /* CRITICAL, 60% pure, n=1 */
                            } else {
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            }
                        }
                    }
                } else {
                    if (f[8] <= 0.489886314f) {  /* band_high_ratio */
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    } else {
                        if (f[9] <= 17.9786186f) {  /* current_rms_a */
                            if (f[2] <= 0.113320217f) {  /* vib_std */
                                if (f[3] <= 6.73213053f) {  /* vib_kurtosis */
                                    return 1; /* EARLY_FAULT, 98% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 75% pure, n=1 */
                                }
                            } else {
                                if (f[8] <= 0.572404027f) {  /* band_high_ratio */
                                    return 2; /* CRITICAL, 91% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 57% pure, n=1 */
                                }
                            }
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    }
                }
            }
        } else {
            if (f[9] <= 13.9787216f) {  /* current_rms_a */
                if (f[10] <= 57.9407272f) {  /* temp_c */
                    if (f[1] <= 0.751653314f) {  /* vib_peak */
                        if (f[8] <= 0.595542312f) {  /* band_high_ratio */
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        } else {
                            return 2; /* CRITICAL, 75% pure, n=1 */
                        }
                    } else {
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    }
                } else {
                    return 2; /* CRITICAL, 80% pure, n=1 */
                }
            } else {
                if (f[7] <= 0.121421948f) {  /* band_mid_ratio */
                    if (f[8] <= 0.738333523f) {  /* band_high_ratio */
                        return 2; /* CRITICAL, 83% pure, n=1 */
                    } else {
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    }
                } else {
                    if (f[2] <= 0.153229058f) {  /* vib_std */
                        if (f[10] <= 73.7796631f) {  /* temp_c */
                            if (f[0] <= 1.04616392f) {  /* vib_rms */
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            } else {
                                return 2; /* CRITICAL, 83% pure, n=1 */
                            }
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    } else {
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    }
                }
            }
        }
    }
}

/* --- tree 7 (113 nodes) ---------------------------------------------- */
static uint8_t rf_tree_7(const float *f)
{
    if (f[6] <= 0.247326314f) {  /* band_low_ratio */
        if (f[3] <= 3.31910229f) {  /* vib_kurtosis */
            if (f[1] <= 0.284191161f) {  /* vib_peak */
                if (f[8] <= 0.600629151f) {  /* band_high_ratio */
                    if (f[1] <= 0.257230282f) {  /* vib_peak */
                        return 1; /* EARLY_FAULT, 100% pure, n=1 */
                    } else {
                        if (f[9] <= 16.1187496f) {  /* current_rms_a */
                            return 1; /* EARLY_FAULT, 83% pure, n=1 */
                        } else {
                            return 2; /* CRITICAL, 80% pure, n=1 */
                        }
                    }
                } else {
                    return 0; /* HEALTHY, 100% pure, n=1 */
                }
            } else {
                if (f[6] <= 0.215249851f) {  /* band_low_ratio */
                    if (f[9] <= 16.2963715f) {  /* current_rms_a */
                        if (f[10] <= 61.3581161f) {  /* temp_c */
                            if (f[1] <= 0.334391415f) {  /* vib_peak */
                                return 2; /* CRITICAL, 86% pure, n=1 */
                            } else {
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            }
                        } else {
                            if (f[4] <= 3.49459457f) {  /* vib_crest */
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            } else {
                                if (f[1] <= 0.480644852f) {  /* vib_peak */
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                }
                            }
                        }
                    } else {
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    }
                } else {
                    return 1; /* EARLY_FAULT, 60% pure, n=1 */
                }
            }
        } else {
            if (f[5] <= 25.390625f) {  /* dom_freq_hz */
                if (f[1] <= 0.614432812f) {  /* vib_peak */
                    if (f[2] <= 0.0285250582f) {  /* vib_std */
                        if (f[3] <= 4.22188473f) {  /* vib_kurtosis */
                            return 0; /* HEALTHY, 100% pure, n=1 */
                        } else {
                            return 1; /* EARLY_FAULT, 100% pure, n=1 */
                        }
                    } else {
                        if (f[10] <= 79.3572998f) {  /* temp_c */
                            if (f[3] <= 4.10501862f) {  /* vib_kurtosis */
                                if (f[3] <= 3.70587492f) {  /* vib_kurtosis */
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 67% pure, n=1 */
                                }
                            } else {
                                if (f[9] <= 17.9086933f) {  /* current_rms_a */
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 62% pure, n=1 */
                                }
                            }
                        } else {
                            return 2; /* CRITICAL, 80% pure, n=1 */
                        }
                    }
                } else {
                    if (f[4] <= 5.14394951f) {  /* vib_crest */
                        if (f[0] <= 0.997394443f) {  /* vib_rms */
                            return 2; /* CRITICAL, 80% pure, n=1 */
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    } else {
                        return 2; /* CRITICAL, 56% pure, n=1 */
                    }
                }
            } else {
                if (f[6] <= 0.101328939f) {  /* band_low_ratio */
                    if (f[10] <= 78.0570221f) {  /* temp_c */
                        if (f[8] <= 0.590340734f) {  /* band_high_ratio */
                            if (f[1] <= 0.415016502f) {  /* vib_peak */
                                if (f[2] <= 0.0898400918f) {  /* vib_std */
                                    return 1; /* EARLY_FAULT, 98% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 60% pure, n=1 */
                                }
                            } else {
                                if (f[1] <= 0.479555339f) {  /* vib_peak */
                                    return 2; /* CRITICAL, 69% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                }
                            }
                        } else {
                            if (f[6] <= 0.043504402f) {  /* band_low_ratio */
                                if (f[6] <= 0.0302230977f) {  /* band_low_ratio */
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 68% pure, n=1 */
                                }
                            } else {
                                if (f[1] <= 0.590907931f) {  /* vib_peak */
                                    return 1; /* EARLY_FAULT, 95% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 82% pure, n=1 */
                                }
                            }
                        }
                    } else {
                        if (f[10] <= 79.8903198f) {  /* temp_c */
                            if (f[2] <= 0.105157346f) {  /* vib_std */
                                if (f[6] <= 0.068584457f) {  /* band_low_ratio */
                                    return 1; /* EARLY_FAULT, 75% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                }
                            } else {
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            }
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    }
                } else {
                    if (f[10] <= 80.1103973f) {  /* temp_c */
                        if (f[1] <= 0.614788294f) {  /* vib_peak */
                            if (f[2] <= 0.0707657561f) {  /* vib_std */
                                if (f[1] <= 0.107074574f) {  /* vib_peak */
                                    return 1; /* EARLY_FAULT, 81% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                }
                            } else {
                                if (f[3] <= 5.43038034f) {  /* vib_kurtosis */
                                    return 1; /* EARLY_FAULT, 61% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 90% pure, n=1 */
                                }
                            }
                        } else {
                            if (f[8] <= 0.690357387f) {  /* band_high_ratio */
                                if (f[6] <= 0.105164334f) {  /* band_low_ratio */
                                    return 2; /* CRITICAL, 83% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                }
                            } else {
                                return 2; /* CRITICAL, 78% pure, n=1 */
                            }
                        }
                    } else {
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    }
                }
            }
        }
    } else {
        if (f[1] <= 0.183977365f) {  /* vib_peak */
            if (f[7] <= 0.155194432f) {  /* band_mid_ratio */
                if (f[4] <= 4.18343496f) {  /* vib_crest */
                    if (f[7] <= 0.135329992f) {  /* band_mid_ratio */
                        if (f[7] <= 0.128459573f) {  /* band_mid_ratio */
                            return 0; /* HEALTHY, 100% pure, n=1 */
                        } else {
                            if (f[1] <= 0.0825245082f) {  /* vib_peak */
                                return 0; /* HEALTHY, 100% pure, n=1 */
                            } else {
                                return 0; /* HEALTHY, 75% pure, n=1 */
                            }
                        }
                    } else {
                        if (f[9] <= 13.0078888f) {  /* current_rms_a */
                            return 0; /* HEALTHY, 100% pure, n=1 */
                        } else {
                            if (f[1] <= 0.0700335056f) {  /* vib_peak */
                                return 0; /* HEALTHY, 100% pure, n=1 */
                            } else {
                                return 1; /* EARLY_FAULT, 83% pure, n=1 */
                            }
                        }
                    }
                } else {
                    if (f[2] <= 0.0311055593f) {  /* vib_std */
                        return 1; /* EARLY_FAULT, 100% pure, n=1 */
                    } else {
                        return 1; /* EARLY_FAULT, 75% pure, n=1 */
                    }
                }
            } else {
                if (f[6] <= 0.254180253f) {  /* band_low_ratio */
                    return 0; /* HEALTHY, 78% pure, n=1 */
                } else {
                    if (f[1] <= 0.0807820708f) {  /* vib_peak */
                        return 1; /* EARLY_FAULT, 83% pure, n=1 */
                    } else {
                        return 1; /* EARLY_FAULT, 100% pure, n=1 */
                    }
                }
            }
        } else {
            if (f[9] <= 18.0331001f) {  /* current_rms_a */
                if (f[6] <= 0.407735348f) {  /* band_low_ratio */
                    if (f[0] <= 1.03538847f) {  /* vib_rms */
                        return 1; /* EARLY_FAULT, 100% pure, n=1 */
                    } else {
                        if (f[9] <= 15.3051376f) {  /* current_rms_a */
                            return 1; /* EARLY_FAULT, 75% pure, n=1 */
                        } else {
                            return 1; /* EARLY_FAULT, 83% pure, n=1 */
                        }
                    }
                } else {
                    return 1; /* EARLY_FAULT, 64% pure, n=1 */
                }
            } else {
                return 2; /* CRITICAL, 100% pure, n=1 */
            }
        }
    }
}

/* --- tree 8 (103 nodes) ---------------------------------------------- */
static uint8_t rf_tree_8(const float *f)
{
    if (f[3] <= 3.20883179f) {  /* vib_kurtosis */
        if (f[5] <= 56.640625f) {  /* dom_freq_hz */
            if (f[6] <= 0.160356611f) {  /* band_low_ratio */
                if (f[2] <= 0.0736194476f) {  /* vib_std */
                    return 0; /* HEALTHY, 100% pure, n=1 */
                } else {
                    return 2; /* CRITICAL, 100% pure, n=1 */
                }
            } else {
                if (f[7] <= 0.177412122f) {  /* band_mid_ratio */
                    if (f[2] <= 0.0831422955f) {  /* vib_std */
                        if (f[2] <= 0.0596978068f) {  /* vib_std */
                            if (f[3] <= 2.98440886f) {  /* vib_kurtosis */
                                return 0; /* HEALTHY, 100% pure, n=1 */
                            } else {
                                if (f[8] <= 0.415969133f) {  /* band_high_ratio */
                                    return 0; /* HEALTHY, 93% pure, n=1 */
                                } else {
                                    return 0; /* HEALTHY, 100% pure, n=1 */
                                }
                            }
                        } else {
                            return 0; /* HEALTHY, 62% pure, n=1 */
                        }
                    } else {
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    }
                } else {
                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                }
            }
        } else {
            if (f[2] <= 0.0881520063f) {  /* vib_std */
                if (f[9] <= 12.118206f) {  /* current_rms_a */
                    return 0; /* HEALTHY, 50% pure, n=1 */
                } else {
                    if (f[9] <= 17.9318924f) {  /* current_rms_a */
                        return 1; /* EARLY_FAULT, 100% pure, n=1 */
                    } else {
                        return 2; /* CRITICAL, 60% pure, n=1 */
                    }
                }
            } else {
                if (f[0] <= 0.960474551f) {  /* vib_rms */
                    return 2; /* CRITICAL, 71% pure, n=1 */
                } else {
                    if (f[4] <= 3.4976902f) {  /* vib_crest */
                        if (f[4] <= 2.73991609f) {  /* vib_crest */
                            return 2; /* CRITICAL, 92% pure, n=1 */
                        } else {
                            if (f[9] <= 15.2955256f) {  /* current_rms_a */
                                if (f[0] <= 1.00429487f) {  /* vib_rms */
                                    return 2; /* CRITICAL, 89% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                }
                            } else {
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            }
                        }
                    } else {
                        if (f[2] <= 0.11468742f) {  /* vib_std */
                            return 2; /* CRITICAL, 78% pure, n=1 */
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    }
                }
            }
        }
    } else {
        if (f[2] <= 0.10532631f) {  /* vib_std */
            if (f[2] <= 0.0763343796f) {  /* vib_std */
                if (f[10] <= 47.8948288f) {  /* temp_c */
                    if (f[7] <= 0.123153612f) {  /* band_mid_ratio */
                        return 0; /* HEALTHY, 100% pure, n=1 */
                    } else {
                        return 0; /* HEALTHY, 75% pure, n=1 */
                    }
                } else {
                    if (f[9] <= 11.8451233f) {  /* current_rms_a */
                        return 0; /* HEALTHY, 100% pure, n=1 */
                    } else {
                        if (f[9] <= 18.1941414f) {  /* current_rms_a */
                            if (f[6] <= 0.433390945f) {  /* band_low_ratio */
                                if (f[6] <= 0.287751019f) {  /* band_low_ratio */
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 95% pure, n=1 */
                                }
                            } else {
                                return 0; /* HEALTHY, 67% pure, n=1 */
                            }
                        } else {
                            return 2; /* CRITICAL, 80% pure, n=1 */
                        }
                    }
                }
            } else {
                if (f[5] <= 58.59375f) {  /* dom_freq_hz */
                    if (f[9] <= 17.8196754f) {  /* current_rms_a */
                        return 1; /* EARLY_FAULT, 100% pure, n=1 */
                    } else {
                        return 1; /* EARLY_FAULT, 60% pure, n=1 */
                    }
                } else {
                    if (f[9] <= 17.9902992f) {  /* current_rms_a */
                        if (f[10] <= 80.3604126f) {  /* temp_c */
                            if (f[0] <= 0.995437741f) {  /* vib_rms */
                                if (f[3] <= 4.137393f) {  /* vib_kurtosis */
                                    return 1; /* EARLY_FAULT, 54% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 91% pure, n=1 */
                                }
                            } else {
                                if (f[10] <= 56.5014496f) {  /* temp_c */
                                    return 1; /* EARLY_FAULT, 86% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 99% pure, n=1 */
                                }
                            }
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    } else {
                        if (f[7] <= 0.204594225f) {  /* band_mid_ratio */
                            if (f[0] <= 1.00151062f) {  /* vib_rms */
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            } else {
                                return 2; /* CRITICAL, 80% pure, n=1 */
                            }
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    }
                }
            }
        } else {
            if (f[1] <= 0.700229168f) {  /* vib_peak */
                if (f[10] <= 79.875824f) {  /* temp_c */
                    if (f[7] <= 0.412789881f) {  /* band_mid_ratio */
                        if (f[2] <= 0.122071117f) {  /* vib_std */
                            if (f[10] <= 49.7696762f) {  /* temp_c */
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            } else {
                                if (f[9] <= 17.6941261f) {  /* current_rms_a */
                                    return 1; /* EARLY_FAULT, 95% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                }
                            }
                        } else {
                            if (f[9] <= 16.7600174f) {  /* current_rms_a */
                                if (f[8] <= 0.68501538f) {  /* band_high_ratio */
                                    return 2; /* CRITICAL, 63% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 88% pure, n=1 */
                                }
                            } else {
                                if (f[7] <= 0.168188199f) {  /* band_mid_ratio */
                                    return 2; /* CRITICAL, 83% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                }
                            }
                        }
                    } else {
                        if (f[1] <= 0.472939759f) {  /* vib_peak */
                            if (f[0] <= 1.02100551f) {  /* vib_rms */
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            } else {
                                return 2; /* CRITICAL, 75% pure, n=1 */
                            }
                        } else {
                            if (f[3] <= 4.6459341f) {  /* vib_kurtosis */
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            } else {
                                if (f[10] <= 69.0373001f) {  /* temp_c */
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 75% pure, n=1 */
                                }
                            }
                        }
                    }
                } else {
                    return 2; /* CRITICAL, 100% pure, n=1 */
                }
            } else {
                if (f[4] <= 5.68788338f) {  /* vib_crest */
                    if (f[10] <= 73.6716461f) {  /* temp_c */
                        if (f[2] <= 0.153218418f) {  /* vib_std */
                            return 2; /* CRITICAL, 75% pure, n=1 */
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    } else {
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    }
                } else {
                    return 1; /* EARLY_FAULT, 62% pure, n=1 */
                }
            }
        }
    }
}

/* --- tree 9 (81 nodes) ---------------------------------------------- */
static uint8_t rf_tree_9(const float *f)
{
    if (f[4] <= 3.60650063f) {  /* vib_crest */
        if (f[7] <= 0.160393983f) {  /* band_mid_ratio */
            if (f[6] <= 0.133958131f) {  /* band_low_ratio */
                if (f[1] <= 0.45720315f) {  /* vib_peak */
                    return 2; /* CRITICAL, 67% pure, n=1 */
                } else {
                    return 2; /* CRITICAL, 100% pure, n=1 */
                }
            } else {
                if (f[0] <= 1.08725643f) {  /* vib_rms */
                    if (f[10] <= 87.7033997f) {  /* temp_c */
                        if (f[10] <= 77.4687729f) {  /* temp_c */
                            if (f[7] <= 0.125606686f) {  /* band_mid_ratio */
                                return 0; /* HEALTHY, 100% pure, n=1 */
                            } else {
                                if (f[1] <= 0.15014708f) {  /* vib_peak */
                                    return 0; /* HEALTHY, 97% pure, n=1 */
                                } else {
                                    return 0; /* HEALTHY, 57% pure, n=1 */
                                }
                            }
                        } else {
                            if (f[3] <= 2.88189006f) {  /* vib_kurtosis */
                                return 0; /* HEALTHY, 100% pure, n=1 */
                            } else {
                                if (f[6] <= 0.334728181f) {  /* band_low_ratio */
                                    return 2; /* CRITICAL, 67% pure, n=1 */
                                } else {
                                    return 0; /* HEALTHY, 100% pure, n=1 */
                                }
                            }
                        }
                    } else {
                        return 2; /* CRITICAL, 86% pure, n=1 */
                    }
                } else {
                    return 2; /* CRITICAL, 83% pure, n=1 */
                }
            }
        } else {
            if (f[9] <= 17.947916f) {  /* current_rms_a */
                if (f[2] <= 0.120131858f) {  /* vib_std */
                    if (f[8] <= 0.573366165f) {  /* band_high_ratio */
                        if (f[7] <= 0.470373452f) {  /* band_mid_ratio */
                            return 1; /* EARLY_FAULT, 100% pure, n=1 */
                        } else {
                            if (f[4] <= 3.55420017f) {  /* vib_crest */
                                return 1; /* EARLY_FAULT, 100% pure, n=1 */
                            } else {
                                return 2; /* CRITICAL, 60% pure, n=1 */
                            }
                        }
                    } else {
                        if (f[2] <= 0.066555351f) {  /* vib_std */
                            return 1; /* EARLY_FAULT, 70% pure, n=1 */
                        } else {
                            if (f[10] <= 78.6521988f) {  /* temp_c */
                                return 1; /* EARLY_FAULT, 50% pure, n=1 */
                            } else {
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            }
                        }
                    }
                } else {
                    return 2; /* CRITICAL, 100% pure, n=1 */
                }
            } else {
                return 2; /* CRITICAL, 100% pure, n=1 */
            }
        }
    } else {
        if (f[9] <= 18.0185719f) {  /* current_rms_a */
            if (f[2] <= 0.126933843f) {  /* vib_std */
                if (f[1] <= 0.086789012f) {  /* vib_peak */
                    if (f[3] <= 3.24392366f) {  /* vib_kurtosis */
                        return 0; /* HEALTHY, 100% pure, n=1 */
                    } else {
                        return 0; /* HEALTHY, 50% pure, n=1 */
                    }
                } else {
                    if (f[9] <= 11.9926205f) {  /* current_rms_a */
                        return 0; /* HEALTHY, 100% pure, n=1 */
                    } else {
                        if (f[2] <= 0.103896096f) {  /* vib_std */
                            if (f[10] <= 80.4026489f) {  /* temp_c */
                                if (f[1] <= 0.1500808f) {  /* vib_peak */
                                    return 1; /* EARLY_FAULT, 87% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 99% pure, n=1 */
                                }
                            } else {
                                if (f[2] <= 0.0827341154f) {  /* vib_std */
                                    return 2; /* CRITICAL, 80% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                }
                            }
                        } else {
                            if (f[3] <= 5.48454952f) {  /* vib_kurtosis */
                                if (f[8] <= 0.385994911f) {  /* band_high_ratio */
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 50% pure, n=1 */
                                }
                            } else {
                                if (f[6] <= 0.0543911979f) {  /* band_low_ratio */
                                    return 2; /* CRITICAL, 73% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 88% pure, n=1 */
                                }
                            }
                        }
                    }
                }
            } else {
                if (f[2] <= 0.143852606f) {  /* vib_std */
                    if (f[7] <= 0.330536187f) {  /* band_mid_ratio */
                        if (f[8] <= 0.6839692f) {  /* band_high_ratio */
                            if (f[10] <= 82.806221f) {  /* temp_c */
                                if (f[7] <= 0.26911962f) {  /* band_mid_ratio */
                                    return 2; /* CRITICAL, 57% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 83% pure, n=1 */
                                }
                            } else {
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            }
                        } else {
                            if (f[6] <= 0.0855230764f) {  /* band_low_ratio */
                                return 1; /* EARLY_FAULT, 100% pure, n=1 */
                            } else {
                                return 1; /* EARLY_FAULT, 75% pure, n=1 */
                            }
                        }
                    } else {
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    }
                } else {
                    if (f[3] <= 6.08346653f) {  /* vib_kurtosis */
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    } else {
                        if (f[7] <= 0.308830053f) {  /* band_mid_ratio */
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        } else {
                            if (f[10] <= 72.5891876f) {  /* temp_c */
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            } else {
                                return 2; /* CRITICAL, 60% pure, n=1 */
                            }
                        }
                    }
                }
            }
        } else {
            if (f[2] <= 0.0495986603f) {  /* vib_std */
                return 0; /* HEALTHY, 100% pure, n=1 */
            } else {
                return 2; /* CRITICAL, 100% pure, n=1 */
            }
        }
    }
}

/* --- tree 10 (71 nodes) ---------------------------------------------- */
static uint8_t rf_tree_10(const float *f)
{
    if (f[7] <= 0.117161632f) {  /* band_mid_ratio */
        if (f[8] <= 0.709126592f) {  /* band_high_ratio */
            if (f[3] <= 3.67521286f) {  /* vib_kurtosis */
                if (f[1] <= 0.191871643f) {  /* vib_peak */
                    return 0; /* HEALTHY, 100% pure, n=1 */
                } else {
                    return 0; /* HEALTHY, 83% pure, n=1 */
                }
            } else {
                return 1; /* EARLY_FAULT, 100% pure, n=1 */
            }
        } else {
            if (f[4] <= 3.74289608f) {  /* vib_crest */
                if (f[9] <= 13.8443661f) {  /* current_rms_a */
                    return 0; /* HEALTHY, 100% pure, n=1 */
                } else {
                    return 2; /* CRITICAL, 67% pure, n=1 */
                }
            } else {
                if (f[0] <= 1.03059101f) {  /* vib_rms */
                    if (f[2] <= 0.135360375f) {  /* vib_std */
                        if (f[10] <= 74.1603241f) {  /* temp_c */
                            return 1; /* EARLY_FAULT, 100% pure, n=1 */
                        } else {
                            return 1; /* EARLY_FAULT, 77% pure, n=1 */
                        }
                    } else {
                        return 2; /* CRITICAL, 60% pure, n=1 */
                    }
                } else {
                    if (f[2] <= 0.116705239f) {  /* vib_std */
                        return 1; /* EARLY_FAULT, 100% pure, n=1 */
                    } else {
                        return 2; /* CRITICAL, 92% pure, n=1 */
                    }
                }
            }
        }
    } else {
        if (f[9] <= 17.9654198f) {  /* current_rms_a */
            if (f[10] <= 80.0565338f) {  /* temp_c */
                if (f[2] <= 0.121147968f) {  /* vib_std */
                    if (f[3] <= 3.16874528f) {  /* vib_kurtosis */
                        if (f[1] <= 0.144683957f) {  /* vib_peak */
                            if (f[8] <= 0.393252373f) {  /* band_high_ratio */
                                if (f[6] <= 0.392792881f) {  /* band_low_ratio */
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 75% pure, n=1 */
                                }
                            } else {
                                if (f[1] <= 0.0922768116f) {  /* vib_peak */
                                    return 0; /* HEALTHY, 100% pure, n=1 */
                                } else {
                                    return 0; /* HEALTHY, 84% pure, n=1 */
                                }
                            }
                        } else {
                            if (f[10] <= 52.5284805f) {  /* temp_c */
                                if (f[1] <= 0.334391415f) {  /* vib_peak */
                                    return 0; /* HEALTHY, 67% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                }
                            } else {
                                if (f[6] <= 0.128195196f) {  /* band_low_ratio */
                                    return 1; /* EARLY_FAULT, 78% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 99% pure, n=1 */
                                }
                            }
                        }
                    } else {
                        if (f[1] <= 0.0907267481f) {  /* vib_peak */
                            if (f[9] <= 12.4190941f) {  /* current_rms_a */
                                return 0; /* HEALTHY, 100% pure, n=1 */
                            } else {
                                return 1; /* EARLY_FAULT, 91% pure, n=1 */
                            }
                        } else {
                            if (f[10] <= 48.0387917f) {  /* temp_c */
                                return 2; /* CRITICAL, 92% pure, n=1 */
                            } else {
                                if (f[6] <= 0.060183689f) {  /* band_low_ratio */
                                    return 1; /* EARLY_FAULT, 92% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 98% pure, n=1 */
                                }
                            }
                        }
                    }
                } else {
                    if (f[2] <= 0.145953372f) {  /* vib_std */
                        if (f[7] <= 0.335240602f) {  /* band_mid_ratio */
                            if (f[3] <= 5.0509491f) {  /* vib_kurtosis */
                                return 2; /* CRITICAL, 93% pure, n=1 */
                            } else {
                                if (f[1] <= 0.615793288f) {  /* vib_peak */
                                    return 1; /* EARLY_FAULT, 91% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 53% pure, n=1 */
                                }
                            }
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    } else {
                        if (f[4] <= 5.47096443f) {  /* vib_crest */
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        } else {
                            return 2; /* CRITICAL, 71% pure, n=1 */
                        }
                    }
                }
            } else {
                return 2; /* CRITICAL, 100% pure, n=1 */
            }
        } else {
            if (f[5] <= 56.640625f) {  /* dom_freq_hz */
                if (f[10] <= 70.7369461f) {  /* temp_c */
                    if (f[1] <= 0.523600936f) {  /* vib_peak */
                        return 0; /* HEALTHY, 71% pure, n=1 */
                    } else {
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    }
                } else {
                    if (f[2] <= 0.0606861077f) {  /* vib_std */
                        return 0; /* HEALTHY, 100% pure, n=1 */
                    } else {
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    }
                }
            } else {
                if (f[9] <= 18.0165272f) {  /* current_rms_a */
                    return 2; /* CRITICAL, 89% pure, n=1 */
                } else {
                    return 2; /* CRITICAL, 100% pure, n=1 */
                }
            }
        }
    }
}

/* --- tree 11 (129 nodes) ---------------------------------------------- */
static uint8_t rf_tree_11(const float *f)
{
    if (f[6] <= 0.247326314f) {  /* band_low_ratio */
        if (f[1] <= 0.484895527f) {  /* vib_peak */
            if (f[4] <= 3.18130016f) {  /* vib_crest */
                if (f[7] <= 0.168936849f) {  /* band_mid_ratio */
                    if (f[10] <= 74.8495026f) {  /* temp_c */
                        return 0; /* HEALTHY, 100% pure, n=1 */
                    } else {
                        if (f[0] <= 1.00289202f) {  /* vib_rms */
                            return 0; /* HEALTHY, 100% pure, n=1 */
                        } else {
                            return 2; /* CRITICAL, 57% pure, n=1 */
                        }
                    }
                } else {
                    if (f[2] <= 0.0823852271f) {  /* vib_std */
                        return 1; /* EARLY_FAULT, 100% pure, n=1 */
                    } else {
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    }
                }
            } else {
                if (f[2] <= 0.0877358392f) {  /* vib_std */
                    if (f[1] <= 0.0954223722f) {  /* vib_peak */
                        if (f[8] <= 0.634073019f) {  /* band_high_ratio */
                            return 1; /* EARLY_FAULT, 100% pure, n=1 */
                        } else {
                            if (f[8] <= 0.711959004f) {  /* band_high_ratio */
                                return 0; /* HEALTHY, 100% pure, n=1 */
                            } else {
                                return 0; /* HEALTHY, 70% pure, n=1 */
                            }
                        }
                    } else {
                        if (f[4] <= 3.77057076f) {  /* vib_crest */
                            if (f[8] <= 0.651404321f) {  /* band_high_ratio */
                                if (f[9] <= 17.9428768f) {  /* current_rms_a */
                                    return 1; /* EARLY_FAULT, 98% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 86% pure, n=1 */
                                }
                            } else {
                                if (f[10] <= 59.3845215f) {  /* temp_c */
                                    return 0; /* HEALTHY, 100% pure, n=1 */
                                } else {
                                    return 0; /* HEALTHY, 50% pure, n=1 */
                                }
                            }
                        } else {
                            if (f[2] <= 0.0720692575f) {  /* vib_std */
                                if (f[2] <= 0.0655089021f) {  /* vib_std */
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 97% pure, n=1 */
                                }
                            } else {
                                if (f[9] <= 19.2769756f) {  /* current_rms_a */
                                    return 1; /* EARLY_FAULT, 96% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                }
                            }
                        }
                    }
                } else {
                    if (f[7] <= 0.455243677f) {  /* band_mid_ratio */
                        if (f[9] <= 18.0899563f) {  /* current_rms_a */
                            if (f[10] <= 85.0075226f) {  /* temp_c */
                                if (f[10] <= 48.7865868f) {  /* temp_c */
                                    return 2; /* CRITICAL, 75% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 92% pure, n=1 */
                                }
                            } else {
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            }
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    } else {
                        if (f[0] <= 0.953177452f) {  /* vib_rms */
                            return 2; /* CRITICAL, 62% pure, n=1 */
                        } else {
                            if (f[6] <= 0.117091313f) {  /* band_low_ratio */
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            } else {
                                return 2; /* CRITICAL, 80% pure, n=1 */
                            }
                        }
                    }
                }
            }
        } else {
            if (f[3] <= 5.75348997f) {  /* vib_kurtosis */
                if (f[2] <= 0.143819138f) {  /* vib_std */
                    if (f[7] <= 0.324813962f) {  /* band_mid_ratio */
                        if (f[10] <= 62.5981636f) {  /* temp_c */
                            return 1; /* EARLY_FAULT, 70% pure, n=1 */
                        } else {
                            if (f[8] <= 0.642232716f) {  /* band_high_ratio */
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            } else {
                                return 2; /* CRITICAL, 75% pure, n=1 */
                            }
                        }
                    } else {
                        if (f[7] <= 0.444193304f) {  /* band_mid_ratio */
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        } else {
                            if (f[6] <= 0.104168668f) {  /* band_low_ratio */
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            } else {
                                return 2; /* CRITICAL, 80% pure, n=1 */
                            }
                        }
                    }
                } else {
                    return 2; /* CRITICAL, 100% pure, n=1 */
                }
            } else {
                if (f[5] <= 25.390625f) {  /* dom_freq_hz */
                    if (f[0] <= 1.0028193f) {  /* vib_rms */
                        if (f[2] <= 0.121386342f) {  /* vib_std */
                            return 1; /* EARLY_FAULT, 100% pure, n=1 */
                        } else {
                            return 1; /* EARLY_FAULT, 50% pure, n=1 */
                        }
                    } else {
                        if (f[7] <= 0.125514552f) {  /* band_mid_ratio */
                            if (f[6] <= 0.136250824f) {  /* band_low_ratio */
                                return 2; /* CRITICAL, 67% pure, n=1 */
                            } else {
                                if (f[6] <= 0.162121862f) {  /* band_low_ratio */
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 75% pure, n=1 */
                                }
                            }
                        } else {
                            if (f[9] <= 18.3611374f) {  /* current_rms_a */
                                return 2; /* CRITICAL, 55% pure, n=1 */
                            } else {
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            }
                        }
                    }
                } else {
                    if (f[10] <= 75.0101776f) {  /* temp_c */
                        if (f[0] <= 1.03730738f) {  /* vib_rms */
                            if (f[9] <= 18.3160553f) {  /* current_rms_a */
                                if (f[6] <= 0.0411406793f) {  /* band_low_ratio */
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 62% pure, n=1 */
                                }
                            } else {
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            }
                        } else {
                            if (f[2] <= 0.135903448f) {  /* vib_std */
                                if (f[8] <= 0.698488891f) {  /* band_high_ratio */
                                    return 2; /* CRITICAL, 88% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 71% pure, n=1 */
                                }
                            } else {
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            }
                        }
                    } else {
                        if (f[1] <= 0.694294453f) {  /* vib_peak */
                            if (f[1] <= 0.660497189f) {  /* vib_peak */
                                if (f[6] <= 0.0882124156f) {  /* band_low_ratio */
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 89% pure, n=1 */
                                }
                            } else {
                                return 2; /* CRITICAL, 60% pure, n=1 */
                            }
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    }
                }
            }
        }
    } else {
        if (f[7] <= 0.154966891f) {  /* band_mid_ratio */
            if (f[6] <= 0.332943559f) {  /* band_low_ratio */
                if (f[1] <= 0.186319083f) {  /* vib_peak */
                    if (f[4] <= 4.05578899f) {  /* vib_crest */
                        if (f[4] <= 3.65063667f) {  /* vib_crest */
                            return 0; /* HEALTHY, 100% pure, n=1 */
                        } else {
                            if (f[10] <= 50.4020233f) {  /* temp_c */
                                return 0; /* HEALTHY, 100% pure, n=1 */
                            } else {
                                return 0; /* HEALTHY, 60% pure, n=1 */
                            }
                        }
                    } else {
                        if (f[9] <= 13.6677589f) {  /* current_rms_a */
                            return 1; /* EARLY_FAULT, 67% pure, n=1 */
                        } else {
                            return 1; /* EARLY_FAULT, 100% pure, n=1 */
                        }
                    }
                } else {
                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                }
            } else {
                if (f[9] <= 13.6134996f) {  /* current_rms_a */
                    if (f[3] <= 3.33168983f) {  /* vib_kurtosis */
                        return 0; /* HEALTHY, 100% pure, n=1 */
                    } else {
                        return 0; /* HEALTHY, 57% pure, n=1 */
                    }
                } else {
                    if (f[3] <= 3.17534447f) {  /* vib_kurtosis */
                        if (f[8] <= 0.411449075f) {  /* band_high_ratio */
                            return 0; /* HEALTHY, 100% pure, n=1 */
                        } else {
                            if (f[2] <= 0.0540482253f) {  /* vib_std */
                                if (f[8] <= 0.419706285f) {  /* band_high_ratio */
                                    return 0; /* HEALTHY, 89% pure, n=1 */
                                } else {
                                    return 0; /* HEALTHY, 100% pure, n=1 */
                                }
                            } else {
                                return 0; /* HEALTHY, 83% pure, n=1 */
                            }
                        }
                    } else {
                        if (f[10] <= 64.6830139f) {  /* temp_c */
                            return 1; /* EARLY_FAULT, 100% pure, n=1 */
                        } else {
                            return 1; /* EARLY_FAULT, 83% pure, n=1 */
                        }
                    }
                }
            }
        } else {
            if (f[8] <= 0.577837169f) {  /* band_high_ratio */
                if (f[9] <= 18.0670853f) {  /* current_rms_a */
                    if (f[1] <= 0.0787258521f) {  /* vib_peak */
                        return 1; /* EARLY_FAULT, 86% pure, n=1 */
                    } else {
                        return 1; /* EARLY_FAULT, 100% pure, n=1 */
                    }
                } else {
                    return 2; /* CRITICAL, 100% pure, n=1 */
                }
            } else {
                return 0; /* HEALTHY, 100% pure, n=1 */
            }
        }
    }
}

/* --- tree 12 (89 nodes) ---------------------------------------------- */
static uint8_t rf_tree_12(const float *f)
{
    if (f[1] <= 0.157799661f) {  /* vib_peak */
        if (f[3] <= 3.2273016f) {  /* vib_kurtosis */
            if (f[7] <= 0.188653469f) {  /* band_mid_ratio */
                if (f[3] <= 2.95245695f) {  /* vib_kurtosis */
                    if (f[4] <= 2.59011531f) {  /* vib_crest */
                        if (f[1] <= 0.105701506f) {  /* vib_peak */
                            return 0; /* HEALTHY, 100% pure, n=1 */
                        } else {
                            return 0; /* HEALTHY, 71% pure, n=1 */
                        }
                    } else {
                        return 0; /* HEALTHY, 100% pure, n=1 */
                    }
                } else {
                    if (f[9] <= 14.4474564f) {  /* current_rms_a */
                        return 0; /* HEALTHY, 100% pure, n=1 */
                    } else {
                        if (f[2] <= 0.0245061368f) {  /* vib_std */
                            return 0; /* HEALTHY, 100% pure, n=1 */
                        } else {
                            if (f[0] <= 1.00846291f) {  /* vib_rms */
                                return 0; /* HEALTHY, 83% pure, n=1 */
                            } else {
                                return 0; /* HEALTHY, 50% pure, n=1 */
                            }
                        }
                    }
                }
            } else {
                return 1; /* EARLY_FAULT, 100% pure, n=1 */
            }
        } else {
            if (f[7] <= 0.132894218f) {  /* band_mid_ratio */
                if (f[3] <= 4.22702312f) {  /* vib_kurtosis */
                    if (f[4] <= 4.14629793f) {  /* vib_crest */
                        return 0; /* HEALTHY, 100% pure, n=1 */
                    } else {
                        return 0; /* HEALTHY, 67% pure, n=1 */
                    }
                } else {
                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                }
            } else {
                if (f[8] <= 0.695400894f) {  /* band_high_ratio */
                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                } else {
                    return 1; /* EARLY_FAULT, 80% pure, n=1 */
                }
            }
        }
    } else {
        if (f[1] <= 0.41267094f) {  /* vib_peak */
            if (f[10] <= 79.851944f) {  /* temp_c */
                if (f[6] <= 0.350978494f) {  /* band_low_ratio */
                    if (f[7] <= 0.620636284f) {  /* band_mid_ratio */
                        if (f[10] <= 48.0476151f) {  /* temp_c */
                            if (f[10] <= 46.1972122f) {  /* temp_c */
                                return 2; /* CRITICAL, 62% pure, n=1 */
                            } else {
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            }
                        } else {
                            if (f[9] <= 18.0973587f) {  /* current_rms_a */
                                if (f[3] <= 3.07131386f) {  /* vib_kurtosis */
                                    return 1; /* EARLY_FAULT, 85% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                }
                            } else {
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            }
                        }
                    } else {
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    }
                } else {
                    if (f[1] <= 0.212689966f) {  /* vib_peak */
                        if (f[3] <= 3.13633895f) {  /* vib_kurtosis */
                            return 0; /* HEALTHY, 100% pure, n=1 */
                        } else {
                            return 1; /* EARLY_FAULT, 100% pure, n=1 */
                        }
                    } else {
                        return 1; /* EARLY_FAULT, 100% pure, n=1 */
                    }
                }
            } else {
                if (f[3] <= 6.43580008f) {  /* vib_kurtosis */
                    if (f[6] <= 0.125977129f) {  /* band_low_ratio */
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    } else {
                        if (f[7] <= 0.452019483f) {  /* band_mid_ratio */
                            if (f[10] <= 83.8555298f) {  /* temp_c */
                                return 2; /* CRITICAL, 88% pure, n=1 */
                            } else {
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            }
                        } else {
                            return 2; /* CRITICAL, 67% pure, n=1 */
                        }
                    }
                } else {
                    return 2; /* CRITICAL, 83% pure, n=1 */
                }
            }
        } else {
            if (f[10] <= 80.0293732f) {  /* temp_c */
                if (f[2] <= 0.126943171f) {  /* vib_std */
                    if (f[6] <= 0.068038933f) {  /* band_low_ratio */
                        if (f[9] <= 17.9161186f) {  /* current_rms_a */
                            if (f[6] <= 0.0417672098f) {  /* band_low_ratio */
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            } else {
                                if (f[7] <= 0.197477683f) {  /* band_mid_ratio */
                                    return 1; /* EARLY_FAULT, 94% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 58% pure, n=1 */
                                }
                            }
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    } else {
                        if (f[9] <= 18.1372337f) {  /* current_rms_a */
                            if (f[6] <= 0.120860696f) {  /* band_low_ratio */
                                if (f[8] <= 0.568915725f) {  /* band_high_ratio */
                                    return 2; /* CRITICAL, 54% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 92% pure, n=1 */
                                }
                            } else {
                                if (f[8] <= 0.471176147f) {  /* band_high_ratio */
                                    return 1; /* EARLY_FAULT, 83% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 99% pure, n=1 */
                                }
                            }
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    }
                } else {
                    if (f[7] <= 0.326579332f) {  /* band_mid_ratio */
                        if (f[2] <= 0.145614862f) {  /* vib_std */
                            if (f[1] <= 0.586188436f) {  /* vib_peak */
                                if (f[4] <= 3.38239861f) {  /* vib_crest */
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 80% pure, n=1 */
                                }
                            } else {
                                if (f[9] <= 15.3262329f) {  /* current_rms_a */
                                    return 1; /* EARLY_FAULT, 83% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 95% pure, n=1 */
                                }
                            }
                        } else {
                            if (f[8] <= 0.58904314f) {  /* band_high_ratio */
                                if (f[7] <= 0.273705572f) {  /* band_mid_ratio */
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 60% pure, n=1 */
                                }
                            } else {
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            }
                        }
                    } else {
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    }
                }
            } else {
                return 2; /* CRITICAL, 100% pure, n=1 */
            }
        }
    }
}

/* --- tree 13 (85 nodes) ---------------------------------------------- */
static uint8_t rf_tree_13(const float *f)
{
    if (f[7] <= 0.120006047f) {  /* band_mid_ratio */
        if (f[6] <= 0.173359007f) {  /* band_low_ratio */
            if (f[10] <= 82.4787445f) {  /* temp_c */
                if (f[2] <= 0.18479456f) {  /* vib_std */
                    if (f[3] <= 3.9079814f) {  /* vib_kurtosis */
                        return 0; /* HEALTHY, 100% pure, n=1 */
                    } else {
                        if (f[2] <= 0.115575187f) {  /* vib_std */
                            if (f[7] <= 0.117062367f) {  /* band_mid_ratio */
                                return 1; /* EARLY_FAULT, 100% pure, n=1 */
                            } else {
                                return 1; /* EARLY_FAULT, 89% pure, n=1 */
                            }
                        } else {
                            return 1; /* EARLY_FAULT, 71% pure, n=1 */
                        }
                    }
                } else {
                    return 2; /* CRITICAL, 100% pure, n=1 */
                }
            } else {
                return 2; /* CRITICAL, 75% pure, n=1 */
            }
        } else {
            if (f[8] <= 0.61798656f) {  /* band_high_ratio */
                if (f[7] <= 0.108094081f) {  /* band_mid_ratio */
                    if (f[6] <= 0.328044355f) {  /* band_low_ratio */
                        if (f[6] <= 0.324776858f) {  /* band_low_ratio */
                            return 0; /* HEALTHY, 100% pure, n=1 */
                        } else {
                            return 0; /* HEALTHY, 83% pure, n=1 */
                        }
                    } else {
                        return 0; /* HEALTHY, 100% pure, n=1 */
                    }
                } else {
                    if (f[1] <= 0.173905045f) {  /* vib_peak */
                        if (f[10] <= 72.6146851f) {  /* temp_c */
                            return 0; /* HEALTHY, 100% pure, n=1 */
                        } else {
                            return 0; /* HEALTHY, 71% pure, n=1 */
                        }
                    } else {
                        return 1; /* EARLY_FAULT, 100% pure, n=1 */
                    }
                }
            } else {
                if (f[7] <= 0.0888260081f) {  /* band_mid_ratio */
                    if (f[1] <= 0.122140899f) {  /* vib_peak */
                        return 0; /* HEALTHY, 100% pure, n=1 */
                    } else {
                        return 0; /* HEALTHY, 83% pure, n=1 */
                    }
                } else {
                    if (f[2] <= 0.0444755256f) {  /* vib_std */
                        if (f[6] <= 0.272195458f) {  /* band_low_ratio */
                            if (f[2] <= 0.0285425466f) {  /* vib_std */
                                return 0; /* HEALTHY, 100% pure, n=1 */
                            } else {
                                if (f[4] <= 4.06164932f) {  /* vib_crest */
                                    return 0; /* HEALTHY, 100% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                }
                            }
                        } else {
                            return 1; /* EARLY_FAULT, 60% pure, n=1 */
                        }
                    } else {
                        if (f[3] <= 3.36025882f) {  /* vib_kurtosis */
                            return 0; /* HEALTHY, 100% pure, n=1 */
                        } else {
                            return 1; /* EARLY_FAULT, 100% pure, n=1 */
                        }
                    }
                }
            }
        }
    } else {
        if (f[10] <= 80.0293732f) {  /* temp_c */
            if (f[0] <= 1.0562284f) {  /* vib_rms */
                if (f[9] <= 17.9930496f) {  /* current_rms_a */
                    if (f[3] <= 3.14763951f) {  /* vib_kurtosis */
                        if (f[6] <= 0.130742297f) {  /* band_low_ratio */
                            if (f[3] <= 2.70449495f) {  /* vib_kurtosis */
                                return 2; /* CRITICAL, 57% pure, n=1 */
                            } else {
                                if (f[0] <= 0.986087739f) {  /* vib_rms */
                                    return 2; /* CRITICAL, 67% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                }
                            }
                        } else {
                            if (f[1] <= 0.0925300121f) {  /* vib_peak */
                                if (f[10] <= 63.6495056f) {  /* temp_c */
                                    return 0; /* HEALTHY, 99% pure, n=1 */
                                } else {
                                    return 0; /* HEALTHY, 67% pure, n=1 */
                                }
                            } else {
                                if (f[10] <= 48.4087944f) {  /* temp_c */
                                    return 0; /* HEALTHY, 100% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 85% pure, n=1 */
                                }
                            }
                        }
                    } else {
                        if (f[6] <= 0.0613430664f) {  /* band_low_ratio */
                            if (f[10] <= 48.8373795f) {  /* temp_c */
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            } else {
                                if (f[8] <= 0.524079621f) {  /* band_high_ratio */
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 66% pure, n=1 */
                                }
                            }
                        } else {
                            if (f[10] <= 48.0312347f) {  /* temp_c */
                                if (f[5] <= 56.640625f) {  /* dom_freq_hz */
                                    return 0; /* HEALTHY, 100% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                }
                            } else {
                                if (f[7] <= 0.525206804f) {  /* band_mid_ratio */
                                    return 1; /* EARLY_FAULT, 91% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 64% pure, n=1 */
                                }
                            }
                        }
                    }
                } else {
                    if (f[5] <= 58.59375f) {  /* dom_freq_hz */
                        if (f[6] <= 0.186711758f) {  /* band_low_ratio */
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        } else {
                            return 0; /* HEALTHY, 57% pure, n=1 */
                        }
                    } else {
                        if (f[2] <= 0.0889361054f) {  /* vib_std */
                            return 2; /* CRITICAL, 91% pure, n=1 */
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    }
                }
            } else {
                if (f[2] <= 0.0920911804f) {  /* vib_std */
                    if (f[6] <= 0.24180156f) {  /* band_low_ratio */
                        return 1; /* EARLY_FAULT, 100% pure, n=1 */
                    } else {
                        return 1; /* EARLY_FAULT, 80% pure, n=1 */
                    }
                } else {
                    return 2; /* CRITICAL, 100% pure, n=1 */
                }
            }
        } else {
            if (f[2] <= 0.0545421615f) {  /* vib_std */
                return 0; /* HEALTHY, 100% pure, n=1 */
            } else {
                return 2; /* CRITICAL, 100% pure, n=1 */
            }
        }
    }
}

/* --- tree 14 (111 nodes) ---------------------------------------------- */
static uint8_t rf_tree_14(const float *f)
{
    if (f[7] <= 0.121120267f) {  /* band_mid_ratio */
        if (f[6] <= 0.172757566f) {  /* band_low_ratio */
            if (f[3] <= 4.1671977f) {  /* vib_kurtosis */
                if (f[1] <= 0.257756472f) {  /* vib_peak */
                    return 0; /* HEALTHY, 100% pure, n=1 */
                } else {
                    return 2; /* CRITICAL, 100% pure, n=1 */
                }
            } else {
                if (f[2] <= 0.148646921f) {  /* vib_std */
                    if (f[7] <= 0.117090307f) {  /* band_mid_ratio */
                        if (f[6] <= 0.131429702f) {  /* band_low_ratio */
                            return 1; /* EARLY_FAULT, 100% pure, n=1 */
                        } else {
                            if (f[2] <= 0.102009125f) {  /* vib_std */
                                return 1; /* EARLY_FAULT, 100% pure, n=1 */
                            } else {
                                return 1; /* EARLY_FAULT, 67% pure, n=1 */
                            }
                        }
                    } else {
                        if (f[8] <= 0.769716501f) {  /* band_high_ratio */
                            return 1; /* EARLY_FAULT, 100% pure, n=1 */
                        } else {
                            return 1; /* EARLY_FAULT, 60% pure, n=1 */
                        }
                    }
                } else {
                    return 2; /* CRITICAL, 100% pure, n=1 */
                }
            }
        } else {
            if (f[4] <= 3.99689198f) {  /* vib_crest */
                if (f[1] <= 0.204968989f) {  /* vib_peak */
                    return 0; /* HEALTHY, 100% pure, n=1 */
                } else {
                    return 1; /* EARLY_FAULT, 71% pure, n=1 */
                }
            } else {
                if (f[9] <= 12.7997389f) {  /* current_rms_a */
                    return 1; /* EARLY_FAULT, 71% pure, n=1 */
                } else {
                    if (f[7] <= 0.109044582f) {  /* band_mid_ratio */
                        return 1; /* EARLY_FAULT, 100% pure, n=1 */
                    } else {
                        if (f[10] <= 65.764801f) {  /* temp_c */
                            return 1; /* EARLY_FAULT, 75% pure, n=1 */
                        } else {
                            return 1; /* EARLY_FAULT, 100% pure, n=1 */
                        }
                    }
                }
            }
        }
    } else {
        if (f[2] <= 0.100515604f) {  /* vib_std */
            if (f[3] <= 3.17011929f) {  /* vib_kurtosis */
                if (f[7] <= 0.171145111f) {  /* band_mid_ratio */
                    if (f[7] <= 0.126453668f) {  /* band_mid_ratio */
                        if (f[10] <= 55.3243408f) {  /* temp_c */
                            return 0; /* HEALTHY, 100% pure, n=1 */
                        } else {
                            return 0; /* HEALTHY, 57% pure, n=1 */
                        }
                    } else {
                        return 0; /* HEALTHY, 100% pure, n=1 */
                    }
                } else {
                    if (f[6] <= 0.0805859193f) {  /* band_low_ratio */
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    } else {
                        if (f[10] <= 80.9488144f) {  /* temp_c */
                            if (f[8] <= 0.54855597f) {  /* band_high_ratio */
                                return 1; /* EARLY_FAULT, 100% pure, n=1 */
                            } else {
                                return 1; /* EARLY_FAULT, 44% pure, n=1 */
                            }
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    }
                }
            } else {
                if (f[9] <= 18.0864067f) {  /* current_rms_a */
                    if (f[10] <= 80.3363113f) {  /* temp_c */
                        if (f[3] <= 3.27925062f) {  /* vib_kurtosis */
                            if (f[2] <= 0.0256357715f) {  /* vib_std */
                                if (f[8] <= 0.598193169f) {  /* band_high_ratio */
                                    return 1; /* EARLY_FAULT, 60% pure, n=1 */
                                } else {
                                    return 0; /* HEALTHY, 100% pure, n=1 */
                                }
                            } else {
                                if (f[8] <= 0.497357666f) {  /* band_high_ratio */
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 57% pure, n=1 */
                                }
                            }
                        } else {
                            if (f[2] <= 0.0973519981f) {  /* vib_std */
                                if (f[8] <= 0.808981121f) {  /* band_high_ratio */
                                    return 1; /* EARLY_FAULT, 100% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 67% pure, n=1 */
                                }
                            } else {
                                if (f[0] <= 0.999017f) {  /* vib_rms */
                                    return 2; /* CRITICAL, 67% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 92% pure, n=1 */
                                }
                            }
                        }
                    } else {
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    }
                } else {
                    return 2; /* CRITICAL, 100% pure, n=1 */
                }
            }
        } else {
            if (f[7] <= 0.360930383f) {  /* band_mid_ratio */
                if (f[1] <= 0.593714893f) {  /* vib_peak */
                    if (f[1] <= 0.45658952f) {  /* vib_peak */
                        if (f[9] <= 17.7735863f) {  /* current_rms_a */
                            if (f[3] <= 3.29621458f) {  /* vib_kurtosis */
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            } else {
                                if (f[7] <= 0.299381137f) {  /* band_mid_ratio */
                                    return 1; /* EARLY_FAULT, 90% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 83% pure, n=1 */
                                }
                            }
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    } else {
                        if (f[10] <= 80.2217407f) {  /* temp_c */
                            if (f[4] <= 3.78317118f) {  /* vib_crest */
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            } else {
                                if (f[9] <= 18.1824989f) {  /* current_rms_a */
                                    return 1; /* EARLY_FAULT, 79% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                }
                            }
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    }
                } else {
                    if (f[9] <= 16.4563141f) {  /* current_rms_a */
                        if (f[6] <= 0.154722407f) {  /* band_low_ratio */
                            if (f[2] <= 0.139291927f) {  /* vib_std */
                                if (f[2] <= 0.124433696f) {  /* vib_std */
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                } else {
                                    return 1; /* EARLY_FAULT, 88% pure, n=1 */
                                }
                            } else {
                                if (f[9] <= 15.4719982f) {  /* current_rms_a */
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 95% pure, n=1 */
                                }
                            }
                        } else {
                            if (f[1] <= 0.82598567f) {  /* vib_peak */
                                return 1; /* EARLY_FAULT, 64% pure, n=1 */
                            } else {
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            }
                        }
                    } else {
                        if (f[2] <= 0.143607587f) {  /* vib_std */
                            if (f[0] <= 1.04957402f) {  /* vib_rms */
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            } else {
                                return 2; /* CRITICAL, 75% pure, n=1 */
                            }
                        } else {
                            return 2; /* CRITICAL, 100% pure, n=1 */
                        }
                    }
                }
            } else {
                if (f[0] <= 0.970176399f) {  /* vib_rms */
                    if (f[6] <= 0.083220765f) {  /* band_low_ratio */
                        return 2; /* CRITICAL, 100% pure, n=1 */
                    } else {
                        return 2; /* CRITICAL, 80% pure, n=1 */
                    }
                } else {
                    if (f[6] <= 0.212724119f) {  /* band_low_ratio */
                        if (f[4] <= 4.78223038f) {  /* vib_crest */
                            if (f[7] <= 0.434843898f) {  /* band_mid_ratio */
                                if (f[2] <= 0.114332683f) {  /* vib_std */
                                    return 2; /* CRITICAL, 80% pure, n=1 */
                                } else {
                                    return 2; /* CRITICAL, 100% pure, n=1 */
                                }
                            } else {
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            }
                        } else {
                            if (f[3] <= 4.9770813f) {  /* vib_kurtosis */
                                return 2; /* CRITICAL, 75% pure, n=1 */
                            } else {
                                return 2; /* CRITICAL, 100% pure, n=1 */
                            }
                        }
                    } else {
                        return 2; /* CRITICAL, 75% pure, n=1 */
                    }
                }
            }
        }
    }
}

/* -----------------------------------------------------------------------------
 * Forest inference.
 *
 * `votes` receives the per-class tally, which the firmware turns into a
 * confidence figure for the OLED and the telemetry line. A 15-tree forest
 * returning 8/15 for CRITICAL is a materially different situation from one
 * returning 15/15, and a maintenance operator deserves to see which.
 * -------------------------------------------------------------------------- */
#define RF_N_TREES 15

static uint8_t rf_predict(const float *f, uint8_t *votes)
{
    for (uint8_t i = 0; i < N_CLASSES; i++) {
        votes[i] = 0;
    }

    votes[rf_tree_0(f)]++;
    votes[rf_tree_1(f)]++;
    votes[rf_tree_2(f)]++;
    votes[rf_tree_3(f)]++;
    votes[rf_tree_4(f)]++;
    votes[rf_tree_5(f)]++;
    votes[rf_tree_6(f)]++;
    votes[rf_tree_7(f)]++;
    votes[rf_tree_8(f)]++;
    votes[rf_tree_9(f)]++;
    votes[rf_tree_10(f)]++;
    votes[rf_tree_11(f)]++;
    votes[rf_tree_12(f)]++;
    votes[rf_tree_13(f)]++;
    votes[rf_tree_14(f)]++;

    uint8_t best = 0;
    for (uint8_t i = 1; i < N_CLASSES; i++) {
        if (votes[i] > votes[best]) {
            best = i;
        }
    }
    return best;
}

#endif /* MODEL_RF_H */
