#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "dsp/ExportNorm.h"
#include "presets/FactoryPresets.h"

#include <cstring>
#include <vector>

namespace
{
const juce::Identifier kPatchName { "PATCH_NAME" };
const juce::Identifier kSamplePath { "SAMPLE" };
constexpr const char* kInitPatch = "Init Patch";
constexpr const char* kPatchExt = ".snr3p";

juce::File libraryRoot()
{
    return juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
        .getChildFile ("Stupid Audio")
        .getChildFile ("SNR3");
}

juce::File ensureDir (juce::File dir)
{
    dir.createDirectory();
    return dir;
}

juce::String legalStem (const juce::String& name)
{
    auto stem = juce::File::createLegalFileName (name.trim());
    if (stem.endsWithIgnoreCase (kPatchExt))
        stem = stem.dropLastCharacters ((int) std::strlen (kPatchExt));
    return stem;
}

void setParam (juce::AudioProcessorValueTreeState& apvts, const char* id, float value)
{
    if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*> (apvts.getParameter (id)))
    {
        ranged->beginChangeGesture();
        ranged->setValueNotifyingHost (ranged->convertTo0to1 (value));
        ranged->endChangeGesture();
    }
}

void applyFactory (SNR3AudioProcessor& proc, const snr3::FactoryPreset& preset)
{
    proc.initPatch();
    auto& apvts = proc.apvts;
    setParam (apvts, ParamID::tune, preset.tune);
    setParam (apvts, ParamID::shellDec, preset.shellDec);
    setParam (apvts, ParamID::shellTone, preset.shellTone);
    setParam (apvts, ParamID::wiresDec, preset.wiresDec);
    setParam (apvts, ParamID::wiresTone, preset.wiresTone);
    setParam (apvts, ParamID::stickDec, preset.stickDec);
    setParam (apvts, ParamID::stickTone, preset.stickTone);
    setParam (apvts, ParamID::lyGain[0], preset.shellGain);
    setParam (apvts, ParamID::lyGain[1], preset.wiresGain);
    setParam (apvts, ParamID::lyGain[2], preset.stickGain);
    setParam (apvts, ParamID::lyGain[3], 0.f);
    setParam (apvts, ParamID::level, preset.level);
    setParam (apvts, ParamID::duckDepth, preset.duck);
    setParam (apvts, ParamID::duckTime, preset.duckTime);
    setParam (apvts, ParamID::length, (float) preset.length);

    if (const auto* fx = snr3::findFactoryFx (preset.name))
    {
        setParam (apvts, ParamID::fxDry, fx->dry);
        setParam (apvts, ParamID::fxWet, fx->wet);
        if (fx->dst >= 0.f)
        {
            setParam (apvts, ParamID::fxDstOn, 1.f);
            setParam (apvts, ParamID::fxDstDrive, fx->dst);
            setParam (apvts, ParamID::fxDstType, (float) fx->dstType);
        }
        if (fx->dlyDiv >= 0)
        {
            setParam (apvts, ParamID::fxDlyOn, 1.f);
            setParam (apvts, ParamID::fxDlyDiv, (float) fx->dlyDiv);
            setParam (apvts, ParamID::fxDlyFb, fx->dlyFb);
        }
        if (fx->rev >= 0.f)
        {
            setParam (apvts, ParamID::fxRevOn, 1.f);
            setParam (apvts, ParamID::fxRevSize, fx->rev);
            setParam (apvts, ParamID::fxRevDamp, fx->revDamp);
        }
    }

    if (const auto* eq = snr3::findFactoryEq (preset.name))
    {
        setParam (apvts, ParamID::postEqOn, 1.f);
        setParam (apvts, ParamID::postEqType[0], (float) eq->type);
        setParam (apvts, ParamID::postEqFreq[0], eq->hz);
        setParam (apvts, ParamID::postEqGain[0], eq->db);
        setParam (apvts, ParamID::postEqQ[0], eq->q);
        if (eq->type2 > 0)
        {
            setParam (apvts, ParamID::postEqType[1], (float) eq->type2);
            setParam (apvts, ParamID::postEqFreq[1], eq->hz2);
            setParam (apvts, ParamID::postEqGain[1], eq->db2);
            setParam (apvts, ParamID::postEqQ[1], eq->q2);
        }
    }

    apvts.state.setProperty (kPatchName, preset.name, nullptr);
}
} // namespace

SNR3AudioProcessor::SNR3AudioProcessor()
    : AudioProcessor (BusesProperties()
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Snare", juce::AudioChannelSet::stereo(), false)
                          .withOutput ("Slot 1", juce::AudioChannelSet::stereo(), false)
                          .withOutput ("Slot 2", juce::AudioChannelSet::stereo(), false)
                          .withOutput ("Slot 3", juce::AudioChannelSet::stereo(), false)),
      apvts (*this, nullptr, "PARAMS", createParameterLayout())
{
    apvts.state.setProperty (kPatchName, kInitPatch, nullptr);
}

void SNR3AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    stack.prepare (sampleRate);
    for (auto& m : layerMeters)
        m.prepare (sampleRate);
    const int cap = juce::jmax (samplesPerBlock, 8192);
    layerScratch.setSize (snr3::kMixLayers, cap, false, false, true);
    for (int i = 0; i < snr3::kMixLayers; ++i)
    {
        layerEq[i].prepare (sampleRate, cap, 1);
        layerAnalysers[i].prepare (sampleRate);
    }
    analyser.prepare (sampleRate);
    meter.prepare (sampleRate);
    const int ch = juce::jmax (1, getMainBusNumOutputChannels());
    sendScratch.setSize (ch, cap, false, false, true);
    postFx.prepare (sampleRate, samplesPerBlock, ch);
    postEq.prepare (sampleRate, samplesPerBlock, ch);
    stereoWidth.prepare (sampleRate);
}

bool SNR3AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (! layouts.getMainInputChannelSet().isDisabled())
        return false;
    const auto main = layouts.getMainOutputChannelSet();
    if (main != juce::AudioChannelSet::stereo() && main != juce::AudioChannelSet::mono())
        return false;
    for (int i = 1; i < layouts.outputBuses.size(); ++i)
    {
        const auto set = layouts.getChannelSet (false, i);
        if (! set.isDisabled()
            && set != juce::AudioChannelSet::stereo()
            && set != juce::AudioChannelSet::mono())
            return false;
    }
    return true;
}

float SNR3AudioProcessor::raw (const char* id) const
{
    if (auto* p = apvts.getRawParameterValue (id))
        return p->load();
    return 0.f;
}

int SNR3AudioProcessor::lengthChoice() const
{
    return juce::roundToInt (raw (ParamID::length));
}

float SNR3AudioProcessor::hostBpm() const
{
    if (auto* ph = getPlayHead())
        if (auto pos = ph->getPosition())
            if (auto b = pos->getBpm())
                if (*b > 1.0)
                    return (float) *b;
    return 120.f;
}

void SNR3AudioProcessor::bindSample (snr3::SampleVoice::Params& p) const
{
    const int live = userSample.live.load (std::memory_order_acquire);
    auto& buf = userSample.buf[live];
    p.data = buf.getNumSamples() > 1 ? buf.getReadPointer (0) : nullptr;
    p.n = buf.getNumSamples();
    p.fileSr = userSample.fileSr[live];
    p.hpfHz = raw (ParamID::sampleHp);
    p.lpfHz = raw (ParamID::sampleLp);
    p.delayMs = raw (ParamID::lyDelay[3]);
    const float ms = raw (ParamID::sampleLen);
    const float lengthSec = snr3::lengthSeconds (lengthChoice(), hostBpm());
    p.lengthSec = ms > 0.f ? ms * 0.001f : lengthSec;
    p.attackMs = 0.f;
}

void SNR3AudioProcessor::fillParams (snr3::SnareParams& p) const
{
    const float lengthSec = snr3::lengthSeconds (lengthChoice(), hostBpm());
    p.shell.tuneHz = raw (ParamID::tune);
    p.shell.decay = raw (ParamID::shellDec);
    p.shell.toneHz = raw (ParamID::shellTone);
    p.shell.delayMs = raw (ParamID::lyDelay[0]);
    p.shell.lengthSec = lengthSec;
    p.shell.holdNote = juce::roundToInt (raw (ParamID::holdMode)) != 0;
    p.wires.decay = raw (ParamID::wiresDec);
    p.wires.toneHz = raw (ParamID::wiresTone);
    p.wires.delayMs = raw (ParamID::lyDelay[1]);
    p.wires.lengthSec = lengthSec;
    p.stick.decayMs = raw (ParamID::stickDec);
    p.stick.toneHz = raw (ParamID::stickTone);
    p.stick.delayMs = raw (ParamID::lyDelay[2]);
    p.stick.lengthSec = lengthSec;
    bindSample (p.sample);
    p.duckDepth = raw (ParamID::duckDepth);
    p.duckTimeMs = raw (ParamID::duckTime);
}

snr3::SnareParams SNR3AudioProcessor::currentParams() const
{
    snr3::SnareParams p;
    fillParams (p);
    return p;
}

void SNR3AudioProcessor::fillAmps (float* out) const
{
    float gain[snr3::kMixLayers];
    bool mute[snr3::kMixLayers], solo[snr3::kMixLayers], enabled[snr3::kMixLayers], flip[snr3::kMixLayers];
    for (int i = 0; i < snr3::kMixLayers; ++i)
    {
        enabled[i] = true;
        gain[i] = raw (ParamID::lyGain[i]);
        mute[i] = raw (ParamID::lyMute[i]) >= 0.5f;
        solo[i] = raw (ParamID::lySolo[i]) >= 0.5f;
        flip[i] = raw (ParamID::lyFlip[i]) >= 0.5f;
    }
    snr3::layerAmps (gain, mute, solo, enabled, flip, out);
}

void SNR3AudioProcessor::fillWide (bool* out) const
{
    out[0] = false; // Shell
    out[1] = true;  // Wires
    out[2] = true;  // Stick
    out[3] = false; // Sample
}

void SNR3AudioProcessor::applyVelocity (snr3::SnareParams& p, float* amp, int vel) const
{
    const float s = snr3::velScale (vel, raw (ParamID::velAmt));
    p.stick.decayMs *= s;
    p.wires.decay *= s;
    p.shell.decay *= s;
    if (amp == nullptr)
        return;
    for (int i = 1; i < snr3::kMixLayers; ++i)
        amp[i] *= s;
}

void SNR3AudioProcessor::fillSends (float* out) const
{
    for (int i = 0; i < snr3::kMixLayers; ++i)
        out[i] = raw (ParamID::lySend[i]);
}

void SNR3AudioProcessor::renderLayerChans (float* const* chan, int n, double sampleRate) const
{
    auto p = currentParams();
    applyVelocity (p, nullptr, 127);

    snr3::SnareStack local;
    local.prepare (sampleRate);
    local.trigger (p);

    for (int i = 0; i < n; ++i)
    {
        float layer[snr3::kMixLayers];
        local.tickInto (layer);
        for (int L = 0; L < snr3::kMixLayers; ++L)
            if (chan[L] != nullptr)
                chan[L][i] = layer[L];
    }

    const int block = 512;
    for (int L = 0; L < snr3::kMixLayers; ++L)
    {
        if (chan[L] == nullptr)
            continue;
        snr3::ParametricEq eq;
        eq.prepare (sampleRate, block, 1);
        snr3::EqBandParam bands[snr3::eqBandCount];
        const auto ids = snr3::layerEqIds (L);
        snr3::readEqBands (apvts, ids, bands);
        const bool on = snr3::eqEnabled (apvts, ids.on);
        for (int pos = 0; pos < n; )
        {
            const int nThis = juce::jmin (block, n - pos);
            float* ptrs[1] = { chan[L] + pos };
            juce::AudioBuffer<float> view (ptrs, 1, nThis);
            eq.process (view, on, bands);
            pos += nThis;
        }
    }
}

void SNR3AudioProcessor::setLooping (bool on, float bpm)
{
    if (bpm < 40.f)
        bpm = 40.f;
    if (bpm > 240.f)
        bpm = 240.f;
    loopBpm.store (bpm, std::memory_order_relaxed);
    loopOn.store (on, std::memory_order_relaxed);
    if (on)
        loopRestart.store (true, std::memory_order_relaxed);
}

void SNR3AudioProcessor::renderBars (juce::AudioBuffer<float>& dest, double sampleRate, float bpm, int bars)
{
    if (bpm < 40.f)
        bpm = 40.f;
    if (bars < 1)
        bars = 1;
    const int period = juce::jmax (1, (int) (sampleRate * 60.0 / (double) bpm));
    const int n = period * 4 * bars;
    dest.setSize (juce::jmax (1, dest.getNumChannels()), n, false, true, true);
    dest.clear();

    auto p = currentParams();
    float amp[snr3::kMixLayers];
    fillAmps (amp);
    applyVelocity (p, amp, 127);
    float sendAmt[snr3::kMixLayers];
    fillSends (sendAmt);

    snr3::SnareStack local;
    local.prepare (sampleRate);
    std::vector<float> storage[snr3::kMixLayers];
    for (int L = 0; L < snr3::kMixLayers; ++L)
        storage[L].assign ((size_t) n, 0.f);

    int wait = 0;
    for (int i = 0; i < n; ++i)
    {
        if (wait <= 0)
        {
            local.trigger (p);
            wait = period;
        }
        float layer[snr3::kMixLayers];
        local.tickInto (layer);
        for (int L = 0; L < snr3::kMixLayers; ++L)
            storage[L][(size_t) i] = layer[L];
        --wait;
    }

    const int block = 512;
    for (int L = 0; L < snr3::kMixLayers; ++L)
    {
        snr3::ParametricEq eq;
        eq.prepare (sampleRate, block, 1);
        snr3::EqBandParam bands[snr3::eqBandCount];
        const auto ids = snr3::layerEqIds (L);
        snr3::readEqBands (apvts, ids, bands);
        const bool on = snr3::eqEnabled (apvts, ids.on);
        for (int pos = 0; pos < n; )
        {
            const int nThis = juce::jmin (block, n - pos);
            float* ptrs[1] = { storage[L].data() + pos };
            juce::AudioBuffer<float> view (ptrs, 1, nThis);
            eq.process (view, on, bands);
            pos += nThis;
        }
    }

    const float master = raw (ParamID::level);
    const float width = raw (ParamID::width);
    bool isWide[snr3::kMixLayers];
    fillWide (isWide);
    auto* l = dest.getWritePointer (0);
    auto* r = dest.getNumChannels() > 1 ? dest.getWritePointer (1) : nullptr;
    juce::AudioBuffer<float> sendBuf (dest.getNumChannels(), n);
    sendBuf.clear();
    auto* s0 = sendBuf.getWritePointer (0);
    snr3::StereoWidth sw;
    sw.prepare (sampleRate);
    for (int i = 0; i < n; ++i)
    {
        float layer[snr3::kMixLayers];
        for (int L = 0; L < snr3::kMixLayers; ++L)
            layer[L] = storage[L][(size_t) i];
        float mid = 0.f, wide = 0.f, send = 0.f;
        snr3::layerSendMixSplit (layer, amp, sendAmt, isWide, mid, wide, send);
        float L = 0.f, R = 0.f;
        sw.process (mid * master, wide * master, width, L, R);
        l[i] = L;
        if (r != nullptr)
            r[i] = R;
        s0[i] = send * master;
    }
    for (int ch = 1; ch < dest.getNumChannels(); ++ch)
        sendBuf.copyFrom (ch, 0, sendBuf, 0, 0, n);

    snr3::PostFx fx;
    snr3::ParametricEq eq;
    fx.prepare (sampleRate, block, dest.getNumChannels());
    eq.prepare (sampleRate, block, dest.getNumChannels());
    for (int pos = 0; pos < n; )
    {
        const int nThis = juce::jmin (block, n - pos);
        juce::AudioBuffer<float> view (dest.getArrayOfWritePointers(), dest.getNumChannels(), pos, nThis);
        juce::AudioBuffer<float> sendView (sendBuf.getArrayOfWritePointers(), sendBuf.getNumChannels(), pos, nThis);
        fx.process (view, bpm, apvts, &sendView);
        snr3::EqBandParam bands[snr3::eqBandCount];
        snr3::readEqBands (apvts, bands);
        eq.process (view, snr3::postEqEnabled (apvts), bands);
        pos += nThis;
    }
    snr3::clipSamples (dest.getArrayOfWritePointers(), dest.getNumChannels(), n);
}

void SNR3AudioProcessor::renderShot (float* dest, int n, double sampleRate) const
{
    std::vector<float> storage[snr3::kMixLayers];
    float* chan[snr3::kMixLayers];
    for (int L = 0; L < snr3::kMixLayers; ++L)
    {
        storage[L].assign ((size_t) n, 0.f);
        chan[L] = storage[L].data();
    }
    renderLayerChans (chan, n, sampleRate);

    float amp[snr3::kMixLayers];
    fillAmps (amp);
    const float master = raw (ParamID::level);
    for (int i = 0; i < n; ++i)
    {
        float acc = 0.f;
        for (int L = 0; L < snr3::kMixLayers; ++L)
            acc += chan[L][i] * amp[L];
        dest[i] = acc * master;
    }
}

void SNR3AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    bool trig = pendingTrig.exchange (false, std::memory_order_relaxed);
    int midiNote = -1;
    int midiVel = 127;
    for (const auto meta : midi)
        if (meta.getMessage().isNoteOn())
        {
            trig = true;
            midiNote = meta.getMessage().getNoteNumber();
            midiVel = meta.getMessage().getVelocity();
        }
    midi.clear();

    auto p = currentParams();
    if (midiNote >= 0)
        p.shell.tuneHz = snr3::midiNoteToHz (midiNote);

    const bool looping = loopOn.load (std::memory_order_relaxed);
    if (looping && loopRestart.exchange (false, std::memory_order_relaxed))
    {
        trig = true;
        loopWait = 0;
    }
    const float bpm = juce::jlimit (40.f, 240.f, loopBpm.load (std::memory_order_relaxed));
    const int period = juce::jmax (1, (int) (getSampleRate() * 60.0 / (double) bpm));

    float amp[snr3::kMixLayers];
    fillAmps (amp);
    applyVelocity (p, amp, midiNote >= 0 ? midiVel : 127);
    float sendAmt[snr3::kMixLayers];
    fillSends (sendAmt);
    auto mainOut = getBusBuffer (buffer, false, 0);
    auto* l = mainOut.getWritePointer (0);
    const int n = buffer.getNumSamples();
    const int nEq = juce::jmin (n, layerScratch.getNumSamples());
    int wait = loopWait;
    bool fire = trig;
    for (int i = 0; i < nEq; ++i)
    {
        if (looping && wait <= 0 && ! fire)
        {
            fire = true;
            wait = period;
        }
        if (fire)
        {
            stack.trigger (p);
            fire = false;
            if (looping && wait <= 0)
                wait = period;
        }
        float layer[snr3::kMixLayers];
        stack.tickInto (layer);
        for (int L = 0; L < snr3::kMixLayers; ++L)
            layerScratch.setSample (L, i, layer[L]);
        if (looping && wait > 0)
            --wait;
    }
    loopWait = wait;
    for (int L = 0; L < snr3::kMixLayers; ++L)
    {
        float* ptrs[1] = { layerScratch.getWritePointer (L) };
        juce::AudioBuffer<float> view (ptrs, 1, nEq);
        snr3::EqBandParam bands[snr3::eqBandCount];
        const auto ids = snr3::layerEqIds (L);
        snr3::readEqBands (apvts, ids, bands);
        layerEq[L].process (view, snr3::eqEnabled (apvts, ids.on), bands);
        layerAnalysers[L].push (view);
    }
    const float g = raw (ParamID::level);
    const float width = raw (ParamID::width);
    bool isWide[snr3::kMixLayers];
    fillWide (isWide);
    float peak[snr3::kMixLayers] = {};
    const int sendN = juce::jmin (nEq, sendScratch.getNumSamples());
    auto* rch = mainOut.getNumChannels() > 1 ? mainOut.getWritePointer (1) : nullptr;
    for (int i = 0; i < nEq; ++i)
    {
        float layer[snr3::kMixLayers];
        for (int L = 0; L < snr3::kMixLayers; ++L)
        {
            layer[L] = layerScratch.getSample (L, i);
            const float a = std::abs (layer[L] * amp[L]);
            if (a > peak[L])
                peak[L] = a;
        }
        float mid = 0.f, wide = 0.f, send = 0.f;
        snr3::layerSendMixSplit (layer, amp, sendAmt, isWide, mid, wide, send);
        float L = 0.f, R = 0.f;
        stereoWidth.process (mid * g, wide * g, width, L, R);
        l[i] = L;
        if (rch != nullptr)
            rch[i] = R;
        if (i < sendN)
            sendScratch.setSample (0, i, send * g);
    }
    for (int i = nEq; i < n; ++i)
    {
        l[i] = 0.f;
        if (rch != nullptr)
            rch[i] = 0.f;
    }
    for (int L = 0; L < snr3::kMixLayers; ++L)
        layerMeters[L].setPeak (peak[L]);

    // Bus 1 = full snare stem (pre-Width, pre-FX). Slot 1–3 stay silent.
    if (getBusCount (false) > 1)
        if (auto* b = getBus (false, 1); b != nullptr && b->isEnabled())
        {
            auto stem = getBusBuffer (buffer, false, 1);
            const int ns = juce::jmin (nEq, stem.getNumSamples());
            const int chs = stem.getNumChannels();
            for (int i = 0; i < ns; ++i)
            {
                float y = 0.f;
                for (int L = 0; L < snr3::kMixLayers; ++L)
                    y += layerScratch.getSample (L, i) * amp[L] * g;
                for (int ch = 0; ch < chs; ++ch)
                    stem.setSample (ch, i, y);
            }
        }

    for (int i = sendN; i < sendScratch.getNumSamples() && i < n; ++i)
        sendScratch.setSample (0, i, 0.f);
    for (int ch = 1; ch < sendScratch.getNumChannels(); ++ch)
        sendScratch.copyFrom (ch, 0, sendScratch, 0, 0, juce::jmin (n, sendScratch.getNumSamples()));

    postFx.process (mainOut, hostBpm(), apvts, &sendScratch);
    snr3::EqBandParam bands[snr3::eqBandCount];
    snr3::readEqBands (apvts, bands);
    postEq.process (mainOut, snr3::postEqEnabled (apvts), bands);

    analyser.push (mainOut);
    meter.push (mainOut);
    snr3::clipSamples (buffer.getArrayOfWritePointers(), buffer.getNumChannels(), n);
}

void SNR3AudioProcessor::renderWithPost (juce::AudioBuffer<float>& dest, double sampleRate)
{
    const int n = dest.getNumSamples();
    const int chs = dest.getNumChannels();
    dest.clear();
    if (n <= 0 || chs <= 0)
        return;

    std::vector<float> storage[snr3::kMixLayers];
    float* chan[snr3::kMixLayers];
    for (int L = 0; L < snr3::kMixLayers; ++L)
    {
        storage[L].assign ((size_t) n, 0.f);
        chan[L] = storage[L].data();
    }
    renderLayerChans (chan, n, sampleRate);
    float amp[snr3::kMixLayers];
    fillAmps (amp);
    float sendAmt[snr3::kMixLayers];
    fillSends (sendAmt);
    const float master = raw (ParamID::level);
    const float width = raw (ParamID::width);
    bool isWide[snr3::kMixLayers];
    fillWide (isWide);
    juce::AudioBuffer<float> sendBuf (chs, n);
    sendBuf.clear();
    auto* d0 = dest.getWritePointer (0);
    auto* d1 = chs > 1 ? dest.getWritePointer (1) : nullptr;
    auto* s0 = sendBuf.getWritePointer (0);
    snr3::StereoWidth sw;
    sw.prepare (sampleRate);
    for (int i = 0; i < n; ++i)
    {
        float layer[snr3::kMixLayers];
        for (int L = 0; L < snr3::kMixLayers; ++L)
            layer[L] = chan[L][i];
        float mid = 0.f, wide = 0.f, send = 0.f;
        snr3::layerSendMixSplit (layer, amp, sendAmt, isWide, mid, wide, send);
        float L = 0.f, R = 0.f;
        sw.process (mid * master, wide * master, width, L, R);
        d0[i] = L;
        if (d1 != nullptr)
            d1[i] = R;
        s0[i] = send * master;
    }
    for (int ch = 1; ch < chs; ++ch)
        sendBuf.copyFrom (ch, 0, sendBuf, 0, 0, n);

    snr3::PostFx fx;
    snr3::ParametricEq eq;
    const int block = 512;
    fx.prepare (sampleRate, block, chs);
    eq.prepare (sampleRate, block, chs);
    const float bpm = hostBpm();
    for (int pos = 0; pos < n; )
    {
        const int nThis = juce::jmin (block, n - pos);
        juce::AudioBuffer<float> view (dest.getArrayOfWritePointers(), chs, pos, nThis);
        juce::AudioBuffer<float> sendView (sendBuf.getArrayOfWritePointers(), chs, pos, nThis);
        fx.process (view, bpm, apvts, &sendView);
        snr3::EqBandParam bands[snr3::eqBandCount];
        snr3::readEqBands (apvts, bands);
        eq.process (view, snr3::postEqEnabled (apvts), bands);
        pos += nThis;
    }
    snr3::clipSamples (dest.getArrayOfWritePointers(), dest.getNumChannels(), n);
}

juce::File SNR3AudioProcessor::patchesDir() const
{
    return ensureDir (libraryRoot().getChildFile ("Patches"));
}

juce::StringArray SNR3AudioProcessor::patchNames() const
{
    juce::StringArray names;
    for (const auto& preset : snr3::factoryPresets)
        names.add (preset.name);
    juce::StringArray extra;
    for (const auto& entry : juce::RangedDirectoryIterator (patchesDir(), false, "*.snr3p"))
    {
        const auto stem = entry.getFile().getFileNameWithoutExtension();
        if (! names.contains (stem))
            extra.add (stem);
    }
    extra.sortNatural();
    names.addArray (extra);
    return names;
}

juce::String SNR3AudioProcessor::patchName() const
{
    return apvts.state.getProperty (kPatchName, kInitPatch).toString();
}

void SNR3AudioProcessor::initPatch()
{
    for (auto* base : getParameters())
    {
        auto* param = dynamic_cast<juce::RangedAudioParameter*> (base);
        if (param == nullptr)
            continue;
        param->beginChangeGesture();
        param->setValueNotifyingHost (param->getDefaultValue());
        param->endChangeGesture();
    }
    apvts.state.setProperty (kPatchName, kInitPatch, nullptr);
    clearSample();
}

bool SNR3AudioProcessor::loadSample (const juce::File& file)
{
    juce::AudioFormatManager fm;
    fm.registerBasicFormats();
    std::unique_ptr<juce::AudioFormatReader> reader (fm.createReaderFor (file));
    if (reader == nullptr || reader->lengthInSamples < 2)
        return false;
    const int next = 1 - userSample.live.load (std::memory_order_relaxed);
    const int n = (int) juce::jmin ((juce::int64) reader->lengthInSamples, (juce::int64) 44100 * 8);
    userSample.buf[next].setSize (1, n, false, false, true);
    userSample.buf[next].clear();
    reader->read (&userSample.buf[next], 0, n, 0, true, false);
    userSample.fileSr[next] = reader->sampleRate > 1.0 ? reader->sampleRate : 44100.0;
    userSample.live.store (next, std::memory_order_release);
    apvts.state.setProperty (kSamplePath, file.getFullPathName(), nullptr);
    return true;
}

void SNR3AudioProcessor::clearSample()
{
    const int next = 1 - userSample.live.load (std::memory_order_relaxed);
    userSample.buf[next].setSize (1, 0);
    userSample.fileSr[next] = 44100.0;
    userSample.live.store (next, std::memory_order_release);
    apvts.state.removeProperty (kSamplePath, nullptr);
}

juce::String SNR3AudioProcessor::samplePath() const
{
    return apvts.state.getProperty (kSamplePath).toString();
}

void SNR3AudioProcessor::reloadSample()
{
    const auto path = samplePath();
    if (path.isEmpty() || ! loadSample (juce::File (path)))
    {
        const int next = 1 - userSample.live.load (std::memory_order_relaxed);
        userSample.buf[next].setSize (1, 0);
        userSample.live.store (next, std::memory_order_release);
    }
}

bool SNR3AudioProcessor::loadPatchByName (const juce::String& name)
{
    const auto stem = legalStem (name);
    if (stem.isEmpty() || stem == kInitPatch)
    {
        initPatch();
        return true;
    }
    const auto src = patchesDir().getChildFile (stem + kPatchExt);
    if (src.existsAsFile())
    {
        auto xml = juce::XmlDocument::parse (src);
        if (xml == nullptr || ! xml->hasTagName (apvts.state.getType()))
            return false;
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
        apvts.state.setProperty (kPatchName, stem, nullptr);
        reloadSample();
        return true;
    }
    if (const auto* preset = snr3::findFactoryPreset (stem.toRawUTF8()))
    {
        applyFactory (*this, *preset);
        return true;
    }
    return false;
}

bool SNR3AudioProcessor::savePatchAs (const juce::String& name)
{
    const auto stem = legalStem (name);
    if (stem.isEmpty() || stem == kInitPatch)
        return false;
    apvts.state.setProperty (kPatchName, stem, nullptr);
    auto xml = apvts.copyState().createXml();
    if (xml == nullptr)
        return false;
    return patchesDir().getChildFile (stem + kPatchExt).replaceWithText (xml->toString());
}

bool SNR3AudioProcessor::savePatch()
{
    const auto name = patchName();
    if (name.isEmpty() || name == kInitPatch)
        return false;
    return savePatchAs (name);
}

bool SNR3AudioProcessor::exportPatch (const juce::File& dest)
{
    auto file = dest.hasFileExtension ("snr3p") ? dest : dest.withFileExtension ("snr3p");
    auto xml = apvts.copyState().createXml();
    if (xml == nullptr)
        return false;
    return file.replaceWithText (xml->toString());
}

bool SNR3AudioProcessor::importPatch (const juce::File& src)
{
    const auto stem = legalStem (src.getFileNameWithoutExtension());
    if (stem.isEmpty())
        return false;
    const auto dest = patchesDir().getChildFile (stem + kPatchExt);
    if (src.getFullPathName() != dest.getFullPathName())
    {
        dest.deleteFile();
        if (! src.copyFileTo (dest))
            return false;
    }
    return loadPatchByName (stem);
}

void SNR3AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
    {
        xml->setAttribute ("uiScale", uiScale);
        xml->setAttribute ("wavSr", wavSr);
        xml->setAttribute ("wavNorm", wavNorm ? 1 : 0);
        copyXmlToBinary (*xml, destData);
    }
}

void SNR3AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts.state.getType()))
        {
            uiScale = juce::jlimit (0.4f, 1.5f, (float) xml->getDoubleAttribute ("uiScale", uiScale));
            wavSr = xml->getDoubleAttribute ("wavSr", wavSr) >= 47000.0 ? 48000.0 : 44100.0;
            wavNorm = xml->getIntAttribute ("wavNorm", wavNorm ? 1 : 0) != 0;
            xml->removeAttribute ("uiScale");
            xml->removeAttribute ("wavSr");
            xml->removeAttribute ("wavNorm");
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
            reloadSample();
        }
}

juce::AudioProcessorEditor* SNR3AudioProcessor::createEditor()
{
    return new SNR3AudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SNR3AudioProcessor();
}
