#pragma once

#include "Distortion.h"

#include <cmath>
#include <cstdint>

namespace snr3
{

inline constexpr int kMixLayers = 4; // Shell, Wires, Stick, Sample
inline constexpr float kWiresHpMul = 0.35f;
inline constexpr float kWiresLpMul = 2.2f;

inline float wiresHpFromTone (float toneHz) { return toneHz * kWiresHpMul; }
inline float wiresLpFromTone (float toneHz) { return toneHz * kWiresLpMul; }

// Auto (0 ms) follows the global Length snap.
inline float layerLifeSec (float lengthMs, float globalSec)
{
    return lengthMs > 0.f ? lengthMs * 0.001f : globalSec;
}

// vel 127 or amt 0 = 1.
inline float velScale (int vel, float amt)
{
    if (vel < 0)
        vel = 0;
    if (vel > 127)
        vel = 127;
    if (amt < 0.f)
        amt = 0.f;
    if (amt > 1.f)
        amt = 1.f;
    return 1.f - amt * (1.f - (float) vel / 127.f);
}

struct LayerClock
{
    void prepare (double sampleRate)
    {
        sr = sampleRate > 1.0 ? sampleRate : 44100.0;
    }

    void arm (float delayMs, float lifeSec, float attackMs = 0.f)
    {
        pos = 0;
        delayLeft = (int) (sr * (double) delayMs * 0.001);
        if (delayLeft < 0)
            delayLeft = 0;
        waiting = delayLeft > 0;
        if (lifeSec < 0.005f)
            lifeSec = 0.005f;
        totalN = (int) (sr * (double) lifeSec);
        if (totalN < 8)
            totalN = 8;
        fadeN = (int) (sr * 0.01);
        if (fadeN < 4)
            fadeN = 4;
        if (fadeN > totalN / 4)
            fadeN = totalN / 4;
        if (fadeN < 1)
            fadeN = 1;
        attackN = (int) (sr * (double) attackMs * 0.001);
        if (attackN < 0)
            attackN = 0;
        active = true;
    }

    void silence()
    {
        active = false;
        waiting = false;
    }

    bool tick()
    {
        if (! active)
            return false;
        if (waiting)
        {
            --delayLeft;
            if (delayLeft > 0)
                return false;
            waiting = false;
        }
        return true;
    }

    float fade() const
    {
        if (pos < totalN - fadeN)
            return 1.f;
        const int left = totalN - pos;
        const float u = left <= 0 ? 0.f : (float) left / (float) fadeN;
        return 0.5f * (1.f - std::cos (3.141592653589793f * u));
    }

    float attack() const
    {
        if (attackN <= 0 || pos >= attackN)
            return 1.f;
        const float u = (float) pos / (float) attackN;
        return 0.5f * (1.f - std::cos (3.141592653589793f * u));
    }

    void advance()
    {
        ++pos;
        if (pos >= totalN)
            active = false;
    }

    bool active = false;
    double sr = 44100.0;
    int pos = 0;

private:
    int totalN = 0;
    int fadeN = 4;
    int attackN = 0;
    int delayLeft = 0;
    bool waiting = false;
};

inline void layerSortBand (float& hp, float& lp)
{
    if (hp > lp)
    {
        const float s = hp;
        hp = lp;
        lp = s;
    }
}

inline float layerOnePole (float hz, double sr)
{
    const float ny = (float) (sr * 0.45);
    if (hz < 1.f)
        hz = 1.f;
    if (hz > ny)
        hz = ny;
    return 1.f - std::exp (-6.283185307179586f * hz / (float) sr);
}

inline std::uint32_t noiseStep (std::uint32_t& n)
{
    n = n * 1664525u + 1013904223u;
    return n;
}

inline float noiseUni (std::uint32_t& n)
{
    return ((float) (int) (noiseStep (n) >> 8) * (1.f / 8388608.f)) - 1.f;
}

// Pitched body. Small start chirp is baked in — not Kick Drop / P.Hold.
struct ShellVoice
{
    struct Params
    {
        float tuneHz = 185.f;
        float decay = 0.18f;
        float toneHz = 3500.f;
        float delayMs = 0.f;
        float lengthSec = 0.f;
        float attackMs = 0.4f;
        bool holdNote = false;
        int bodyShape = 0;   // 0 sine, 1 tri
        float harmonic = 0.f; // user Harm + Style 2nd
    };

    void prepare (double sampleRate)
    {
        sr = sampleRate > 1.0 ? sampleRate : 44100.0;
        clock.prepare (sampleRate);
    }

    void trigger (const Params& p)
    {
        params = p;
        phase = 0.0;
        phase2 = 0.0;
        lpZ = 0.f;
        float life = params.decay * 8.f;
        if (life < 0.05f)
            life = 0.05f;
        if (params.holdNote && params.lengthSec <= 0.f)
            life = 32.f;
        if (params.lengthSec > 0.f)
            life = params.lengthSec;
        clock.arm (params.delayMs, life, params.attackMs);
    }

    void silence() { clock.silence(); }
    bool active() const { return clock.active; }

    float process (float bodyDuck = 1.f)
    {
        if (! clock.tick())
            return 0.f;
        const double t = (double) clock.pos / clock.sr;
        // ponytail: ~8 ms chirp, one octave at t=0. Drop knobs if a Meat Bag wants Kick-style pitch.
        const float chirp = std::exp ((float) (-t / 0.008));
        const float freq = params.tuneHz * (1.f + 0.35f * chirp);
        const float freq2 = freq * 1.76f;
        phase += (double) freq / clock.sr;
        phase2 += (double) freq2 / clock.sr;
        if (phase >= 1.0)
            phase -= std::floor (phase);
        if (phase2 >= 1.0)
            phase2 -= std::floor (phase2);
        const float twopi = 6.283185307179586f;
        const float ph = (float) (phase * (double) twopi);
        const float ph2 = (float) (phase2 * (double) twopi);
        float p1 = std::sin (ph);
        float p2 = std::sin (ph2);
        if (params.bodyShape == 1)
        {
            if (p1 > 1.f) p1 = 1.f;
            if (p1 < -1.f) p1 = -1.f;
            if (p2 > 1.f) p2 = 1.f;
            if (p2 < -1.f) p2 = -1.f;
            p1 = 0.636619772f * std::asin (p1);
            p2 = 0.636619772f * std::asin (p2);
        }
        const float body = p1 + 0.45f * p2 + params.harmonic * std::sin (ph + ph);
        const float env = std::exp ((float) (-t / (double) params.decay));
        if (bodyDuck < 0.f)
            bodyDuck = 0.f;
        float x = body * env * bodyDuck * clock.attack() * clock.fade();
        const float lpC = layerOnePole (params.toneHz, clock.sr);
        lpZ += lpC * (x - lpZ);
        clock.advance();
        return lpZ;
    }

    Params params;

private:
    LayerClock clock;
    double sr = 44100.0;
    double phase = 0.0, phase2 = 0.0;
    float lpZ = 0.f;
};

struct WiresVoice
{
    struct Params
    {
        float decay = 0.22f;
        float hpHz = 1225.f;
        float lpHz = 7700.f;
        float delayMs = 0.f;
        float lengthSec = 0.f;
        float attackMs = 0.3f;
        float buzz = 0.f;
        float color = 0.f;
    };

    void prepare (double sampleRate)
    {
        clock.prepare (sampleRate);
        buzzD = (int) (clock.sr * 0.0012);
        if (buzzD < 1)
            buzzD = 1;
        if (buzzD >= kBuzzN)
            buzzD = kBuzzN - 1;
    }

    void trigger (const Params& p)
    {
        params = p;
        hpZ = lpZ = pinkZ = 0.f;
        noise = 0xBEEFu;
        buzzW = 0;
        for (int i = 0; i < kBuzzN; ++i)
            buzzBuf[i] = 0.f;
        float life = params.decay * 8.f;
        if (life < 0.05f)
            life = 0.05f;
        if (params.lengthSec > 0.f)
            life = params.lengthSec;
        clock.arm (params.delayMs, life, params.attackMs);
    }

    void silence() { clock.silence(); }
    bool active() const { return clock.active; }

    float process()
    {
        if (! clock.tick())
            return 0.f;
        const double t = (double) clock.pos / clock.sr;
        float n = noiseUni (noise);
        pinkZ += 0.05f * (n - pinkZ);
        if (params.color > 0.f)
            n += params.color * (pinkZ - n);
        float hp = params.hpHz, lp = params.lpHz;
        layerSortBand (hp, lp);
        const float hpC = layerOnePole (hp, clock.sr);
        const float lpC = layerOnePole (lp, clock.sr);
        hpZ += hpC * (n - hpZ);
        const float high = n - hpZ;
        lpZ += lpC * (high - lpZ);
        const float env = std::exp ((float) (-t / (double) params.decay));
        float y = lpZ * env * clock.attack() * clock.fade();
        int ri = buzzW - buzzD;
        if (ri < 0)
            ri += kBuzzN;
        const float delayed = buzzBuf[ri];
        buzzBuf[buzzW] = y;
        ++buzzW;
        if (buzzW >= kBuzzN)
            buzzW = 0;
        if (params.buzz > 0.f)
            y += params.buzz * delayed;
        clock.advance();
        return y;
    }

    Params params;

private:
    static constexpr int kBuzzN = 256;
    LayerClock clock;
    std::uint32_t noise = 1;
    float hpZ = 0.f, lpZ = 0.f, pinkZ = 0.f;
    float buzzBuf[kBuzzN] = {};
    int buzzW = 0;
    int buzzD = 53;
};

struct StickVoice
{
    struct Params
    {
        float decayMs = 8.f;
        float toneHz = 2500.f;
        float delayMs = 0.f;
        float lengthSec = 0.f;
        float attackMs = 0.2f;
        float gain = 1.f;
        float hard = 0.f;
        float click = 0.f;
    };

    void prepare (double sampleRate) { clock.prepare (sampleRate); }

    void trigger (const Params& p)
    {
        params = p;
        hpZ = hpHighZ = 0.f;
        noise = 0xAce1u;
        float life = params.decayMs * 0.001f * 8.f;
        if (life < 0.008f)
            life = 0.008f;
        if (params.lengthSec > 0.f)
            life = params.lengthSec;
        clock.arm (params.delayMs, life, params.attackMs);
    }

    void silence() { clock.silence(); }
    bool active() const { return clock.active; }

    float process()
    {
        if (! clock.tick())
            return 0.f;
        const double t = (double) clock.pos / clock.sr;
        const float tau = params.decayMs * 0.001f;
        const float n = noiseUni (noise);
        const float hpC = layerOnePole (params.toneHz, clock.sr);
        hpZ += hpC * (n - hpZ);
        const float env = std::exp ((float) (-t / (double) tau));
        const float fade = params.gain * env * clock.attack() * clock.fade();
        float y = (n - hpZ) * fade;
        if (params.hard > 0.f)
        {
            const float hpHighC = layerOnePole (8000.f, clock.sr);
            hpHighZ += hpHighC * (n - hpHighZ);
            y = (1.f - params.hard) * y + params.hard * (n - hpHighZ) * fade;
        }
        if (params.click > 0.f)
            y += params.click * n * std::exp ((float) (-t / 0.0008)) * params.gain
                 * clock.attack() * clock.fade();
        clock.advance();
        return y;
    }

    Params params;

private:
    LayerClock clock;
    std::uint32_t noise = 1;
    float hpZ = 0.f, hpHighZ = 0.f;
};

// Linear resample, no pitch. Missing buffer = silence.
struct SampleVoice
{
    struct Params
    {
        const float* data = nullptr;
        int n = 0;
        double fileSr = 44100.0;
        float hpfHz = 80.f;
        float lpfHz = 16000.f;
        float delayMs = 0.f;
        float lengthSec = 0.f;
        float attackMs = 0.f;
        float start01 = 0.f;
        bool reverse = false;
        float decaySec = 0.f;
    };

    void prepare (double sampleRate) { clock.prepare (sampleRate); }

    void trigger (const Params& p)
    {
        params = p;
        hpZ = lpZ = 0.f;
        float life = 0.05f;
        if (params.data != nullptr && params.n > 1 && params.fileSr > 1.0)
        {
            float start = params.start01;
            if (start < 0.f)
                start = 0.f;
            if (start > 1.f)
                start = 1.f;
            const int startI = (int) (start * (float) (params.n - 1));
            life = (float) ((double) (params.n - startI) / params.fileSr);
        }
        if (life < 0.005f)
            life = 0.005f;
        if (params.lengthSec > 0.f)
            life = params.lengthSec;
        clock.arm (params.delayMs, life, params.attackMs);
    }

    void silence() { clock.silence(); }
    bool active() const { return clock.active; }

    float process()
    {
        if (! clock.tick())
            return 0.f;
        float x = 0.f;
        if (params.data != nullptr && params.n > 1 && params.fileSr > 1.0)
        {
            const double last = (double) (params.n - 1);
            double start = (double) params.start01 * last;
            if (start < 0.0)
                start = 0.0;
            if (start > last)
                start = last;
            const double idx = params.reverse
                ? (last - start) - (double) clock.pos * (params.fileSr / clock.sr)
                : start + (double) clock.pos * (params.fileSr / clock.sr);
            if (idx >= 0.0 && idx <= last)
            {
                const int i0 = (int) idx;
                if (i0 >= params.n - 1)
                    x = params.data[params.n - 1];
                else
                {
                    const float frac = (float) (idx - (double) i0);
                    x = params.data[i0] + frac * (params.data[i0 + 1] - params.data[i0]);
                }
            }
        }
        float hp = params.hpfHz;
        float lp = params.lpfHz;
        layerSortBand (hp, lp);
        const float hpC = layerOnePole (hp, clock.sr);
        const float lpC = layerOnePole (lp, clock.sr);
        hpZ += hpC * (x - hpZ);
        const float high = x - hpZ;
        lpZ += lpC * (high - lpZ);
        float env = clock.attack() * clock.fade();
        if (params.decaySec > 0.f)
        {
            const double t = (double) clock.pos / clock.sr;
            env *= std::exp ((float) (-t / (double) params.decaySec));
        }
        const float y = lpZ * env;
        clock.advance();
        return y;
    }

    Params params;

private:
    LayerClock clock;
    float hpZ = 0.f, lpZ = 0.f;
};

struct LayerDuck
{
    void prepare (double sampleRate)
    {
        sr = sampleRate > 1.0 ? sampleRate : 44100.0;
        active = false;
    }

    void arm (float delayMs, float timeMs, float depth)
    {
        if (depth < 0.f)
            depth = 0.f;
        if (depth > 1.f)
            depth = 1.f;
        if (timeMs < 0.5f)
            timeMs = 0.5f;
        delayN = (int) (sr * (double) delayMs * 0.001);
        if (delayN < 0)
            delayN = 0;
        timeN = (int) (sr * (double) timeMs * 0.001);
        if (timeN < 2)
            timeN = 2;
        amt = depth;
        pos = 0;
        active = amt > 0.f;
    }

    float tick()
    {
        if (! active)
            return 1.f;
        float w = 0.f;
        if (pos >= delayN && pos < delayN + timeN)
        {
            const float den = (float) (timeN - 1);
            const float u = den <= 0.f ? 0.f : (float) (pos - delayN) / den;
            w = 0.5f * (1.f - std::cos (6.283185307179586f * u));
        }
        ++pos;
        if (pos >= delayN + timeN)
            active = false;
        return 1.f - amt * w;
    }

    double sr = 44100.0;
    int pos = 0;
    int delayN = 0;
    int timeN = 2;
    float amt = 0.f;
    bool active = false;
};

inline float layerDriveSamp (float x, float drive)
{
    if (drive <= 0.f)
        return x;
    return distortCore (distSoft, x, 1.f + drive * 12.f);
}

inline void panGains (float pan, float& gL, float& gR)
{
    const float a = (pan + 1.f) * 0.7853981633974483f;
    gL = std::cos (a) * 1.41421356237f;
    gR = std::sin (a) * 1.41421356237f;
}

inline void layerSendMix (const float* layer, const float* amp, const float* sendAmt,
                          float& dry, float& send, int n = kMixLayers)
{
    dry = 0.f;
    send = 0.f;
    for (int i = 0; i < n; ++i)
    {
        const float y = layer[i] * amp[i];
        dry += y;
        send += y * sendAmt[i];
    }
}

inline void layerSendMixSplit (const float* layer, const float* amp, const float* sendAmt,
                               const bool* isWide, const float* pan, const float* drive,
                               float& mid, float& wide, float& send,
                               float& extraL, float& extraR, int n = kMixLayers)
{
    mid = 0.f;
    wide = 0.f;
    send = 0.f;
    extraL = 0.f;
    extraR = 0.f;
    for (int i = 0; i < n; ++i)
    {
        const float y = layerDriveSamp (layer[i] * amp[i], drive[i]);
        send += y * sendAmt[i];
        if (isWide[i])
            wide += y;
        else
            mid += y;
        float gL = 1.f, gR = 1.f;
        panGains (pan[i], gL, gR);
        extraL += y * (gL - 1.f);
        extraR += y * (gR - 1.f);
    }
}

inline void layerAmps (const float* gain, const bool* mute, const bool* solo,
                       const bool* enabled, const bool* flip, float* out, int n = kMixLayers)
{
    bool anySolo = false;
    for (int i = 0; i < n; ++i)
        if (enabled[i] && solo[i])
            anySolo = true;
    for (int i = 0; i < n; ++i)
    {
        const bool hear = enabled[i] && ! mute[i] && (! anySolo || solo[i]);
        out[i] = hear ? gain[i] * (flip[i] ? -1.f : 1.f) : 0.f;
    }
}

struct SnareParams
{
    ShellVoice::Params shell;
    WiresVoice::Params wires;
    StickVoice::Params stick;
    SampleVoice::Params sample;
    float duckDepth = 0.f;
    float duckTimeMs = 1.5f;
    int style = 0;     // Classic Fat Snap Tight Hard
    int bodyShape = 0; // 0 sine, 1 tri
    float harm = 0.f;
};

// Knobs stay put. Style scales stick / decays and adds a 2nd on top of Harm.
inline void applyStyle (SnareParams& p)
{
    float stickTauMul = 1.f, stickMul = 1.f, dMul = 1.f, harmonic = 0.f;
    switch (p.style)
    {
        case 1: stickTauMul = 2.f;    stickMul = 0.15f; dMul = 2.2f;  break;
        case 2: stickTauMul = 0.5f;   stickMul = 1.4f;  dMul = 0.55f; harmonic = 0.08f; break;
        case 3: stickTauMul = 0.75f;  stickMul = 1.1f;  dMul = 0.45f; break;
        case 4: stickTauMul = 0.75f;  stickMul = 1.2f;  dMul = 0.8f;  harmonic = 0.25f; break;
        default: break;
    }
    p.stick.decayMs *= stickTauMul;
    if (p.stick.decayMs < 0.5f)
        p.stick.decayMs = 0.5f;
    p.stick.gain = stickMul;
    p.shell.decay *= dMul;
    p.wires.decay *= dMul;
    if (p.shell.decay < 0.02f)
        p.shell.decay = 0.02f;
    if (p.wires.decay < 0.02f)
        p.wires.decay = 0.02f;
    p.shell.harmonic = p.harm + harmonic;
    p.shell.bodyShape = p.bodyShape;
}

struct SnareStack
{
    void prepare (double sampleRate)
    {
        shell.prepare (sampleRate);
        wires.prepare (sampleRate);
        stick.prepare (sampleRate);
        sample.prepare (sampleRate);
        duck.prepare (sampleRate);
        const double sr = sampleRate > 1.0 ? sampleRate : 44100.0;
        fadeN = (int) (sr * 0.005);
        if (fadeN < 4)
            fadeN = 4;
        for (int L = 0; L < kMixLayers; ++L)
        {
            last[L] = 0.f;
            residual[L] = 0.f;
            residualI[L] = 0;
        }
    }

    void trigger (const SnareParams& in)
    {
        SnareParams p = in;
        applyStyle (p);
        duck.arm (p.stick.delayMs, p.duckTimeMs, p.duckDepth);
        for (int L = 0; L < kMixLayers; ++L)
        {
            residual[L] = last[L];
            residualI[L] = fadeN;
        }
        shell.trigger (p.shell);
        wires.trigger (p.wires);
        stick.trigger (p.stick);
        sample.trigger (p.sample);
    }

    void tickInto (float* layer)
    {
        const float dg = duck.tick();
        layer[0] = shell.process (dg);
        layer[1] = wires.process();
        layer[2] = stick.process();
        layer[3] = sample.process();
        for (int L = 0; L < kMixLayers; ++L)
        {
            if (residualI[L] > 0)
            {
                const float u = (float) residualI[L] / (float) fadeN;
                layer[L] += residual[L] * (0.5f * (1.f - std::cos (3.141592653589793f * u)));
                --residualI[L];
            }
            last[L] = layer[L];
        }
    }

    ShellVoice shell;
    WiresVoice wires;
    StickVoice stick;
    SampleVoice sample;
    LayerDuck duck;

private:
    int fadeN = 4;
    float last[kMixLayers] = {};
    float residual[kMixLayers] = {};
    int residualI[kMixLayers] = {};
};

} // namespace snr3
