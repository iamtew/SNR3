#pragma once

#include "dsp/SnareRender.h"
#include "dsp/SnareVoice.h"
#include "dsp/OutputScope.h"
#include "dsp/StereoWidth.h"
#include "dsp/ParametricEq.h"
#include "dsp/PostFx.h"
#include "parameters/ParameterLayout.h"

#include <JuceHeader.h>
#include <atomic>

class SNR3AudioProcessor : public juce::AudioProcessor
{
public:
    SNR3AudioProcessor();
    ~SNR3AudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "SNR3"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

    void requestTrigger() { pendingTrig.store (true, std::memory_order_relaxed); }
    float hostBpm() const;
    snr3::SnareParams currentParams() const;
    void renderShot (float* dest, int n, double sampleRate) const;
    void renderLayerChans (float* const* chan, int n, double sampleRate) const;
    void renderBars (juce::AudioBuffer<float>& dest, double sampleRate, float bpm, int bars);
    void setLooping (bool on, float bpm);
    void renderWithPost (juce::AudioBuffer<float>& dest, double sampleRate);
    snr3::EqAnalyser& eqAnalyser() { return analyser; }
    snr3::OutputMeter& outputMeter() { return meter; }
    snr3::OutputMeter& dryMeter() { return postFx.dryMeter; }
    snr3::OutputMeter& fxMeter() { return postFx.fxMeter; }
    snr3::OutputMeter& layerMeter (int layer) { return layerMeters[layer]; }
    snr3::EqAnalyser& layerAnalyser (int layer) { return layerAnalysers[layer]; }

    juce::File patchesDir() const;
    juce::StringArray patchNames() const;
    juce::String patchName() const;
    void initPatch();
    bool loadPatchByName (const juce::String& name);
    bool savePatch();
    bool savePatchAs (const juce::String& name);
    bool exportPatch (const juce::File& dest);
    bool importPatch (const juce::File& src);
    bool loadSample (const juce::File& file);
    void clearSample();
    juce::String samplePath() const;
    void reloadSample();

    float uiScale = 1.0f;
    double wavSr = 44100.0;
    bool wavNorm = true;

private:
    float raw (const char* id) const;
    int lengthChoice() const;
    void fillParams (snr3::SnareParams& p) const;
    void fillAmps (float* out) const;
    void fillSends (float* out) const;
    void applyVelocity (snr3::SnareParams& p, float* amp, int vel) const;
    void fillWide (bool* out) const;
    void bindSample (snr3::SampleVoice::Params& p) const;

    std::atomic<bool> loopOn { false };
    std::atomic<float> loopBpm { 140.f };
    std::atomic<bool> loopRestart { false };
    int loopWait = 0;

    snr3::SnareStack stack;
    snr3::OutputMeter layerMeters[snr3::kMixLayers];
    snr3::ParametricEq layerEq[snr3::kMixLayers];
    snr3::EqAnalyser layerAnalysers[snr3::kMixLayers];
    juce::AudioBuffer<float> layerScratch;
    juce::AudioBuffer<float> sendScratch;
    // ponytail: two buffers; a third load while the previous-previous still plays can tear.
    struct SampleBuf
    {
        juce::AudioBuffer<float> buf[2];
        double fileSr[2] = { 44100.0, 44100.0 };
        std::atomic<int> live { 0 };
    };
    SampleBuf userSample;
    snr3::StereoWidth stereoWidth;
    snr3::PostFx postFx;
    snr3::ParametricEq postEq;
    snr3::EqAnalyser analyser;
    snr3::OutputMeter meter;
    std::atomic<bool> pendingTrig { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SNR3AudioProcessor)
};
