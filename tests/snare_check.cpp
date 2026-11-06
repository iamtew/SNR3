#include "dsp/SnareRender.h"
#include "presets/FactoryPresets.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <vector>

static float energy (const float* x, int n)
{
    double s = 0.0;
    for (int i = 0; i < n; ++i)
        s += (double) x[i] * (double) x[i];
    return (float) s;
}

static void renderLayer (int layer, const snr3::SnareParams& p, float* dest, int n, double sr)
{
    snr3::SnareStack v;
    v.prepare (sr);
    v.trigger (p);
    float ly[snr3::kMixLayers];
    for (int i = 0; i < n; ++i)
    {
        v.tickInto (ly);
        dest[i] = ly[layer];
    }
}

int main()
{
    constexpr int n = 44100;
    constexpr double sr = 44100.0;
    std::vector<float> buf ((size_t) n, 0.f);

    snr3::SnareParams p;
    snr3::renderSnare (buf.data(), n, sr, p);
    const float eMix = energy (buf.data(), n);
    if (eMix < 1.f)
    {
        std::printf ("fail: default mix energy %g\n", eMix);
        return 1;
    }

    std::vector<float> shell ((size_t) n), wires ((size_t) n), stick ((size_t) n), sample ((size_t) n);
    renderLayer (0, p, shell.data(), n, sr);
    renderLayer (1, p, wires.data(), n, sr);
    renderLayer (2, p, stick.data(), n, sr);
    renderLayer (3, p, sample.data(), n, sr);
    if (energy (shell.data(), n) < 0.1f || energy (wires.data(), n) < 0.1f || energy (stick.data(), n) < 0.01f)
    {
        std::printf ("fail: layer energy shell %g wires %g stick %g\n",
                     energy (shell.data(), n), energy (wires.data(), n), energy (stick.data(), n));
        return 1;
    }
    if (energy (sample.data(), n) != 0.f)
    {
        std::printf ("fail: missing sample should be silence\n");
        return 1;
    }

    if (std::fabs (snr3::lengthSeconds (1, 120.f) - 0.5f) > 1.0e-5f
        || std::fabs (snr3::lengthSeconds (0, 120.f)) > 1.0e-6f)
    {
        std::printf ("fail: lengthSeconds\n");
        return 1;
    }
    if (std::fabs (snr3::midiNoteToHz (69) - 440.f) > 0.01f)
    {
        std::printf ("fail: midiNoteToHz A4\n");
        return 1;
    }

    p.duckDepth = 1.f;
    p.duckTimeMs = 8.f;
    std::vector<float> ducked ((size_t) n);
    renderLayer (0, p, ducked.data(), n, sr);
    // first ~8 ms of ducked Shell should sit below unducked
    const int win = (int) (sr * 0.008);
    float u = 0.f, d = 0.f;
    for (int i = 0; i < win; ++i)
    {
        u += std::abs (shell[(size_t) i]);
        d += std::abs (ducked[(size_t) i]);
    }
    if (d >= u)
    {
        std::printf ("fail: duck did not dip Shell (%g vs %g)\n", d, u);
        return 1;
    }

    {
        snr3::SnareParams st;
        std::vector<float> classic ((size_t) n), fat ((size_t) n), tight ((size_t) n);
        snr3::renderSnare (classic.data(), n, sr, st);
        st.style = 1;
        snr3::renderSnare (fat.data(), n, sr, st);
        st.style = 3;
        snr3::renderSnare (tight.data(), n, sr, st);
        const int late = (int) (sr * 0.7);
        const float eC = energy (classic.data() + late, n - late);
        const float eF = energy (fat.data() + late, n - late);
        const float eT = energy (tight.data() + late, n - late);
        if (eF <= eC)
        {
            std::printf ("fail: Fat tail %g vs Classic %g\n", eF, eC);
            return 1;
        }
        if (eT >= eC)
        {
            std::printf ("fail: Tight tail %g vs Classic %g\n", eT, eC);
            return 1;
        }
    }

    {
        snr3::SnareParams sh;
        std::vector<float> sine ((size_t) 2048), tri ((size_t) 2048);
        snr3::renderSnare (sine.data(), 2048, sr, sh);
        sh.bodyShape = 1;
        snr3::renderSnare (tri.data(), 2048, sr, sh);
        float d = 0.f;
        for (int i = 0; i < 2048; ++i)
            d = std::max (d, std::abs (sine[(size_t) i] - tri[(size_t) i]));
        if (d <= 0.01f)
        {
            std::printf ("fail: Sine vs Tri d=%g\n", d);
            return 1;
        }
    }

    {
        auto bin2 = [] (const float* x, int n, float hz, double srate) -> float
        {
            const float w = 6.283185307179586f * hz / (float) srate;
            const float coeff = 2.f * std::cos (w);
            float s1 = 0.f, s2 = 0.f;
            for (int i = 0; i < n; ++i)
            {
                const float s0 = x[i] + coeff * s1 - s2;
                s2 = s1;
                s1 = s0;
            }
            return s1 * s1 + s2 * s2 - coeff * s1 * s2;
        };
        snr3::SnareParams h0, h1;
        h1.harm = 1.f;
        std::vector<float> a ((size_t) 4096), b ((size_t) 4096);
        renderLayer (0, h0, a.data(), 4096, sr);
        renderLayer (0, h1, b.data(), 4096, sr);
        const float f2 = h0.shell.tuneHz * 2.f;
        const float e0 = bin2 (a.data(), 4096, f2, sr);
        const float e1 = bin2 (b.data(), 4096, f2, sr);
        if (e1 <= e0 * 1.5f)
        {
            std::printf ("fail: Harm 2x %g vs %g\n", e1, e0);
            return 1;
        }

        snr3::SnareParams w;
        w.wires.hpHz = 3000.f;
        w.wires.lpHz = 4000.f;
        std::vector<float> band ((size_t) n);
        renderLayer (1, w, band.data(), n, sr);
        if (energy (band.data(), n) < 0.01f)
        {
            std::printf ("fail: tight wires band energy %g\n", energy (band.data(), n));
            return 1;
        }
    }

    if (! snr3::factoryBankOk() || snr3::factoryPresetCount < 60
        || snr3::findFactoryPreset ("Punch") == nullptr
        || snr3::findFactoryPreset ("Init Patch") != nullptr)
    {
        std::printf ("fail: factory bank\n");
        return 1;
    }

    std::printf ("ok\n");
    return 0;
}
