#pragma once

#include "SnareVoice.h"

#include <cmath>

namespace snr3
{

inline float beatsToSeconds (float beats, float bpm)
{
    const float b = bpm > 1.f ? bpm : 120.f;
    return beats * 60.f / b;
}

inline float lengthSeconds (int lengthChoice, float bpm)
{
    if (bpm < 20.f)
        bpm = 20.f;
    if (lengthChoice == 1) return 60.f / bpm;        // 1/4
    if (lengthChoice == 2) return 30.f / bpm;        // 1/8
    if (lengthChoice == 3) return 15.f / bpm;        // 1/16
    return 0.f; // free — voice uses decay
}

inline float midiNoteToHz (int note)
{
    return 440.f * std::pow (2.f, (float) (note - 69) / 12.f);
}

inline int hzToMidi (float hz)
{
    if (hz < 1.f)
        hz = 1.f;
    return (int) std::lround (69.0 + 12.0 * std::log2 ((double) hz / 440.0));
}

inline const char* midiPcName (int midi)
{
    static const char* pc[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    int p = midi % 12;
    if (p < 0)
        p += 12;
    return pc[p];
}

inline int midiOctave (int midi)
{
    int p = midi % 12;
    int oct = midi / 12 - 1;
    if (p < 0)
        --oct;
    return oct;
}

// Offline one-shot. Same stack as the audio thread. Waveform uses this path.
inline void renderSnare (float* dest, int n, double sampleRate, const SnareParams& p)
{
    SnareStack v;
    v.prepare (sampleRate);
    v.trigger (p);
    float layer[kMixLayers];
    for (int i = 0; i < n; ++i)
    {
        v.tickInto (layer);
        dest[i] = layer[0] + layer[1] + layer[2] + layer[3];
    }
}

} // namespace snr3
