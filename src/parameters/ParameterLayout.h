#pragma once

#include "ParameterIDs.h"
#include "../dsp/SnareRender.h"

#include <JuceHeader.h>

inline juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    using namespace juce;

    AudioProcessorValueTreeState::ParameterLayout layout;

    auto pct = [] (NormalisableRange<float> range)
    {
        return AudioParameterFloatAttributes()
            .withStringFromValueFunction ([range] (float v, int)
            {
                return String (roundToInt (range.convertTo0to1 (v) * 100.f)) + "%";
            })
            .withValueFromStringFunction ([range] (const String& t)
            {
                return range.convertFrom0to1 (jlimit (0.f, 1.f, t.getFloatValue() / 100.f));
            });
    };

    auto hzAttrs = [] (NormalisableRange<float> range)
    {
        return AudioParameterFloatAttributes()
            .withStringFromValueFunction ([] (float v, int)
            {
                return String (roundToInt (v)) + " Hz";
            })
            .withValueFromStringFunction ([range] (const String& t)
            {
                return jlimit (range.start, range.end, t.getFloatValue());
            });
    };

    auto unit = NormalisableRange<float> (0.f, 1.f);
    auto tuneR = NormalisableRange<float> (80.f, 400.f, 0.1f);
    auto shellDecR = NormalisableRange<float> (0.04f, 0.8f, 0.001f, 0.4f);
    auto wiresDecR = NormalisableRange<float> (0.04f, 1.2f, 0.001f, 0.4f);
    auto stickMsR = NormalisableRange<float> (0.5f, 40.f, 0.1f, 0.4f);
    auto toneR = NormalisableRange<float> (400.f, 12000.f, 1.f, 0.3f);
    auto sampleHpR = NormalisableRange<float> (80.f, 8000.f, 1.f, 0.3f);
    auto sampleLpR = NormalisableRange<float> (200.f, 16000.f, 1.f, 0.3f);
    auto sampleLenR = NormalisableRange<float> (0.f, 2000.f, 1.f, 0.4f);

    layout.add (std::make_unique<AudioParameterFloat> (
        ParameterID { ParamID::tune, 1 }, "Tune", tuneR, 185.f,
        AudioParameterFloatAttributes()
            .withStringFromValueFunction ([] (float v, int)
            {
                const int n = snr3::hzToMidi (v);
                return String (snr3::midiPcName (n)) + String (snr3::midiOctave (n)) + " "
                       + String (roundToInt (v)) + " Hz";
            })
            .withValueFromStringFunction ([tuneR] (const String& t)
            {
                auto s = t.upToFirstOccurrenceOf ("Hz", false, false).trim();
                const int sp = s.lastIndexOfChar (' ');
                if (sp >= 0)
                    s = s.substring (sp + 1);
                return jlimit (tuneR.start, tuneR.end, s.getFloatValue());
            })));
    layout.add (std::make_unique<AudioParameterFloat> (
        ParameterID { ParamID::shellDec, 1 }, "Shell Decay", shellDecR, 0.18f, pct (shellDecR)));
    layout.add (std::make_unique<AudioParameterFloat> (
        ParameterID { ParamID::shellTone, 1 }, "Shell Tone", toneR, 3500.f, hzAttrs (toneR)));
    layout.add (std::make_unique<AudioParameterFloat> (
        ParameterID { ParamID::wiresDec, 1 }, "Wires Decay", wiresDecR, 0.22f, pct (wiresDecR)));
    layout.add (std::make_unique<AudioParameterFloat> (
        ParameterID { ParamID::wiresTone, 1 }, "Wires Tone", toneR, 3500.f, hzAttrs (toneR)));
    layout.add (std::make_unique<AudioParameterFloat> (
        ParameterID { ParamID::stickDec, 1 }, "Stick Decay", stickMsR, 8.f,
        AudioParameterFloatAttributes()
            .withStringFromValueFunction ([] (float v, int)
            {
                return String (v, v < 10.f ? 1 : 0) + " ms";
            })
            .withValueFromStringFunction ([stickMsR] (const String& t)
            {
                return jlimit (stickMsR.start, stickMsR.end, t.getFloatValue());
            })));
    layout.add (std::make_unique<AudioParameterFloat> (
        ParameterID { ParamID::stickTone, 1 }, "Stick Tone", toneR, 2500.f, hzAttrs (toneR)));
    layout.add (std::make_unique<AudioParameterFloat> (
        ParameterID { ParamID::sampleHp, 1 }, "Sample HPF", sampleHpR, 80.f, hzAttrs (sampleHpR)));
    layout.add (std::make_unique<AudioParameterFloat> (
        ParameterID { ParamID::sampleLp, 1 }, "Sample LPF", sampleLpR, 16000.f, hzAttrs (sampleLpR)));
    layout.add (std::make_unique<AudioParameterFloat> (
        ParameterID { ParamID::sampleLen, 1 }, "Sample Length", sampleLenR, 0.f,
        AudioParameterFloatAttributes()
            .withStringFromValueFunction ([] (float v, int)
            {
                if (v <= 0.f)
                    return String ("Auto");
                return String (roundToInt (v)) + " ms";
            })
            .withValueFromStringFunction ([sampleLenR] (const String& t)
            {
                if (t.containsIgnoreCase ("auto"))
                    return 0.f;
                return jlimit (sampleLenR.start, sampleLenR.end, t.getFloatValue());
            })));
    layout.add (std::make_unique<AudioParameterFloat> (ParameterID { ParamID::level, 1 },
                                                        "Volume", unit, 0.75f, pct (unit)));
    layout.add (std::make_unique<AudioParameterChoice> (ParameterID { ParamID::length, 1 },
                                                         "Length",
                                                         StringArray { "Free", "1/4", "1/8", "1/16" },
                                                         0));
    layout.add (std::make_unique<AudioParameterChoice> (ParameterID { ParamID::holdMode, 1 },
                                                         "Hold",
                                                         StringArray { "One-shot", "Hold" },
                                                         0));
    layout.add (std::make_unique<AudioParameterFloat> (
        ParameterID { ParamID::velAmt, 1 }, "Vel", unit, 0.f, pct (unit)));
    layout.add (std::make_unique<AudioParameterFloat> (
        ParameterID { ParamID::width, 1 }, "Width", unit, 0.f, pct (unit)));
    layout.add (std::make_unique<AudioParameterFloat> (
        ParameterID { ParamID::duckDepth, 1 }, "Duck", unit, 0.25f, pct (unit)));
    auto duckMsR = NormalisableRange<float> (0.5f, 8.f, 0.1f);
    layout.add (std::make_unique<AudioParameterFloat> (
        ParameterID { ParamID::duckTime, 1 }, "Duck Time", duckMsR, 2.f,
        AudioParameterFloatAttributes()
            .withStringFromValueFunction ([] (float v, int)
            {
                return String (v, 1) + " ms";
            })
            .withValueFromStringFunction ([duckMsR] (const String& t)
            {
                return jlimit (duckMsR.start, duckMsR.end, t.getFloatValue());
            })));

    auto addOn = [&] (const char* id, const char* name, bool def = false)
    {
        layout.add (std::make_unique<AudioParameterBool> (ParameterID { id, 1 }, name, def));
    };
    auto addUnit = [&] (const char* id, const char* name, float def)
    {
        layout.add (std::make_unique<AudioParameterFloat> (
            ParameterID { id, 1 }, name, unit, def, pct (unit)));
    };

    addUnit (ParamID::fxDry, "Dry", 1.f);
    addUnit (ParamID::fxWet, "FX", 0.f);
    addOn (ParamID::fxDstOn, "Dist On");
    layout.add (std::make_unique<AudioParameterChoice> (
        ParameterID { ParamID::fxDstType, 1 }, "Dist Type",
        StringArray { "Soft", "Hard", "Fold", "Diode" }, 0));
    addUnit (ParamID::fxDstDrive, "Dist Drive", 0.4f);
    addUnit (ParamID::fxDstTone, "Dist Tone", 1.f);
    addUnit (ParamID::fxDstShape, "Dist Shape", 0.5f);
    addOn (ParamID::fxDlyOn, "Delay On");
    addUnit (ParamID::fxDlyTime, "Delay Time", 0.35f);
    addUnit (ParamID::fxDlyFb, "Delay Fb", 0.35f);
    addUnit (ParamID::fxDlyCut, "Delay Cut", 0.7f);
    addOn (ParamID::fxDlySync, "Delay Sync", true);
    layout.add (std::make_unique<AudioParameterChoice> (
        ParameterID { ParamID::fxDlyDiv, 1 }, "Delay Div",
        StringArray { "1/16", "1/8", "1/4", "1/2" }, 1));
    addOn (ParamID::fxRevOn, "Reverb On");
    addUnit (ParamID::fxRevSize, "Reverb Size", 0.45f);
    addUnit (ParamID::fxRevDamp, "Reverb Damp", 0.4f);
    addUnit (ParamID::fxRevWidth, "Reverb Width", 0.8f);
    auto preR = NormalisableRange<float> (0.f, 200.f, 0.1f);
    layout.add (std::make_unique<AudioParameterFloat> (
        ParameterID { ParamID::fxRevPredelay, 1 }, "Reverb Predelay", preR, 0.f,
        AudioParameterFloatAttributes()
            .withStringFromValueFunction ([] (float v, int)
            {
                return String (roundToInt (v)) + " ms";
            })
            .withValueFromStringFunction ([preR] (const String& t)
            {
                return jlimit (preR.start, preR.end, t.getFloatValue());
            })));
    addOn (ParamID::fxRevSync, "Reverb Sync");
    layout.add (std::make_unique<AudioParameterChoice> (
        ParameterID { ParamID::fxRevDiv, 1 }, "Reverb Div",
        StringArray { "1/64", "1/32", "1/16", "1/8" }, 1));

    const float eqHzDef[ParamID::eqBandCount] = { 80.f, 250.f, 1000.f, 4000.f, 10000.f };
    auto eqHz = NormalisableRange<float> (20.f, 20000.f, 0.01f, 0.3f);
    auto eqQ = NormalisableRange<float> (0.1f, 10.f, 0.01f, 0.4f);
    auto eqGain = NormalisableRange<float> (-12.f, 12.f, 0.01f);
    auto eqTypes = StringArray { "Off", "Peak", "Low S", "High S", "HP", "LP" };
    addOn (ParamID::postEqOn, "Post EQ On");
    for (int b = 0; b < ParamID::eqBandCount; ++b)
    {
        const String n = "Post EQ " + String (b + 1);
        layout.add (std::make_unique<AudioParameterChoice> (
            ParameterID { ParamID::postEqType[b], 1 }, n + " Type", eqTypes, 1));
        layout.add (std::make_unique<AudioParameterFloat> (
            ParameterID { ParamID::postEqFreq[b], 1 }, n + " Freq", eqHz, eqHzDef[b]));
        layout.add (std::make_unique<AudioParameterFloat> (
            ParameterID { ParamID::postEqGain[b], 1 }, n + " Gain", eqGain, 0.f));
        layout.add (std::make_unique<AudioParameterFloat> (
            ParameterID { ParamID::postEqQ[b], 1 }, n + " Q", eqQ, 0.7f));
    }

    auto delayR = NormalisableRange<float> (0.f, 80.f, 0.1f);
    auto msAttrs = [] (NormalisableRange<float> range)
    {
        return AudioParameterFloatAttributes()
            .withStringFromValueFunction ([] (float v, int)
            {
                return String (v, v < 10.f ? 1 : 0) + " ms";
            })
            .withValueFromStringFunction ([range] (const String& t)
            {
                return jlimit (range.start, range.end, t.getFloatValue());
            });
    };
    const char* who[] = { "Shell", "Wires", "Stick", "Sample" };
    for (int i = 0; i < ParamID::layerCount; ++i)
    {
        addUnit (ParamID::lyGain[i], (String (who[i]) + " Gain").toRawUTF8(), 1.f);
        addOn (ParamID::lyMute[i], (String (who[i]) + " Mute").toRawUTF8());
        addOn (ParamID::lySolo[i], (String (who[i]) + " Solo").toRawUTF8());
        addOn (ParamID::lyFlip[i], (String (who[i]) + " Phase").toRawUTF8());
        addUnit (ParamID::lySend[i], (String (who[i]) + " Send").toRawUTF8(), 1.f);
        layout.add (std::make_unique<AudioParameterFloat> (
            ParameterID { ParamID::lyDelay[i], 1 }, String (who[i]) + " Delay", delayR, 0.f, msAttrs (delayR)));
    }

    for (int layer = 0; layer < ParamID::layerCount; ++layer)
    {
        addOn (ParamID::lyEqOn[layer], (String (who[layer]) + " EQ On").toRawUTF8());
        for (int b = 0; b < ParamID::eqBandCount; ++b)
        {
            const String n = String (who[layer]) + " EQ " + String (b + 1);
            layout.add (std::make_unique<AudioParameterChoice> (
                ParameterID { ParamID::lyEqType[layer][b], 1 }, n + " Type", eqTypes, 1));
            layout.add (std::make_unique<AudioParameterFloat> (
                ParameterID { ParamID::lyEqFreq[layer][b], 1 }, n + " Freq", eqHz, eqHzDef[b]));
            layout.add (std::make_unique<AudioParameterFloat> (
                ParameterID { ParamID::lyEqGain[layer][b], 1 }, n + " Gain", eqGain, 0.f));
            layout.add (std::make_unique<AudioParameterFloat> (
                ParameterID { ParamID::lyEqQ[layer][b], 1 }, n + " Q", eqQ, 0.7f));
        }
    }

    return layout;
}
