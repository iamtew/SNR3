#include "dsp/SnareRender.h"

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

    std::printf ("ok\n");
    return 0;
}
