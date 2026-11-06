#pragma once

#include "PluginProcessor.h"
#include "ui/EqPage.h"
#include "ui/LayerMix.h"

#include <atomic>
#include <functional>
#include <vector>

class SNR3AudioProcessorEditor : public juce::AudioProcessorEditor,
                                public juce::DragAndDropContainer,
                                private juce::Timer,
                                private juce::AudioProcessorValueTreeState::Listener
{
public:
    explicit SNR3AudioProcessorEditor (SNR3AudioProcessor&);
    ~SNR3AudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // Everything lives in `content`, laid out at a fixed base size and scaled as
    // one picture. Dragging the corner changes only the scale, so nothing reflows.
    struct Content : public juce::Component
    {
        explicit Content (SNR3AudioProcessorEditor& e) : ed (e) {}
        void paint (juce::Graphics& g) override { ed.paintContent (g); }
        void resized() override { ed.layoutContent(); }
        void mouseUp (const juce::MouseEvent& e) override { ed.contentMouseUp (e); }
        SNR3AudioProcessorEditor& ed;
    };
    void paintContent (juce::Graphics&);
    void layoutContent();
    void contentMouseUp (const juce::MouseEvent&);

    void timerCallback() override;
    void parameterChanged (const juce::String&, float) override;
    void rebuildWave();
    juce::Rectangle<int> titleHitBounds() const;
    void openAbout();
    void exportWav();
    void dragSnareFiles();
    void commitLoopBpm();
    void refreshPatchName();
    void openPatchMenu();
    void cyclePatch (int delta);
    void chooseSaveAs();
    void chooseExport();
    void chooseImport();
    void chooseSample();
    void setEditorPage (int page);
    void selectLayer (int layer);
    void selectMix();
    void bindKnobs();
    void openPageMenu();
    void cyclePage (int delta);
    void applyPageVisibility();

    SNR3AudioProcessor& proc;

    struct PanelLnF : public juce::LookAndFeel_V4
    {
        void drawRotarySlider (juce::Graphics&, int x, int y, int w, int h, float pos,
                               float startAngle, float endAngle, juce::Slider&) override;
        void drawToggleButton (juce::Graphics&, juce::ToggleButton&, bool, bool) override;
        void drawComboBox (juce::Graphics&, int, int, bool, int, int, int, int, juce::ComboBox&) override;
        juce::Font getLabelFont (juce::Label&) override;
        juce::Font getComboBoxFont (juce::ComboBox&) override;
    };

    struct AboutBox : public juce::Component
    {
        AboutBox();
        void paint (juce::Graphics& g) override;
        void resized() override;
        void mouseUp (const juce::MouseEvent& e) override;
        void checkForUpdate();
        void applyResult (const juce::String& text, bool hasUpdate);
        juce::Rectangle<int> cardBounds() const;

        juce::Label status;
        juce::ToggleButton checkBtn { "Check for update" };
        juce::HyperlinkButton releasesBtn {
            "Open releases",
            juce::URL ("https://github.com/iamtew/SNR3/releases/latest")
        };
        juce::ToggleButton closeBtn { "Close" };
        std::atomic<bool> fetching { false };
    };

    struct WaveView : public juce::Component
    {
        void paint (juce::Graphics& g) override;
        void mouseDown (const juce::MouseEvent&) override { dragStarted = false; }
        void mouseDrag (const juce::MouseEvent& e) override;
        std::function<void()> onDragOut;
        bool dragStarted = false;
        struct Trace
        {
            std::vector<float> samples;
            juce::Colour colour;
            float alpha = 0.45f;
            float attackSec = 0.f;
        };
        std::vector<Trace> traces;
        float durationSec = 0.f;
        struct Onset
        {
            juce::Colour colour;
            char letter = 'K';
            float delayMs = 0.f;
            float attackMs = 0.f;
            bool mute = false;
            bool solo = false;
        };
        std::vector<Onset> onsets;
    };

    struct LayerRail : public juce::Component
    {
        explicit LayerRail (SNR3AudioProcessorEditor& e) : ed (e) {}
        void paint (juce::Graphics& g) override;
        void mouseDown (const juce::MouseEvent& e) override;

        struct Item
        {
            int layer = -1;
            int type = 0;
            bool mix = false;
            bool add = false; // unused, kept so layout matches K1X item struct
            juce::Rectangle<int> bounds;
        };
        void layoutItems (juce::Array<Item>& items) const;
        SNR3AudioProcessorEditor& ed;
    };

    struct PatchBar : public juce::Component
    {
        void paint (juce::Graphics& g) override;
        void mouseUp (const juce::MouseEvent& e) override;
        void setText (const juce::String& t);

        juce::String text;
        std::function<void (int)> onStep;
        std::function<void()> onOpen;
    };

    struct VolumeStrip : public juce::Component
    {
        explicit VolumeStrip (SNR3AudioProcessorEditor& ed);
        ~VolumeStrip() override;
        void resized() override;
        void paint (juce::Graphics& g) override;
        void refresh();
        juce::Rectangle<float> faderArea() const;
        juce::Rectangle<float> scopeArea() const;

        struct LnF : public juce::LookAndFeel_V4
        {
            void drawLinearSlider (juce::Graphics&, int, int, int, int, float pos,
                                   float, float, juce::Slider::SliderStyle, juce::Slider&) override;
        };

        SNR3AudioProcessorEditor& editor;
        LnF lnf;
        juce::Slider slider;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAtt;
        bool clipped = false;
        juce::uint32 clipUntil = 0;
    };

    // Vertical Dry/FX strip: fader cap is the param, fill is post-fader peak.
    struct MixStrip : public juce::Component
    {
        MixStrip (juce::AudioProcessorValueTreeState& st, const char* id, const char* name,
                  snr3::OutputMeter& meter);
        ~MixStrip() override;
        void resized() override;
        void paint (juce::Graphics& g) override;
        void refresh();
        juce::Rectangle<float> trackArea() const;

        struct LnF : public juce::LookAndFeel_V4
        {
            void drawLinearSlider (juce::Graphics&, int, int, int, int, float pos,
                                   float, float, juce::Slider::SliderStyle, juce::Slider&) override;
        };

        snr3::OutputMeter& meter;
        LnF lnf;
        juce::Label label;
        juce::Slider slider;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> att;
        bool clipped = false;
        juce::uint32 clipUntil = 0;
    };

    struct Knob : public juce::Component
    {
        Knob (juce::AudioProcessorValueTreeState& st, const juce::String& id, const juce::String& name);
        void resized() override;
        void bind (juce::AudioProcessorValueTreeState& st, const char* id, const juce::String& name);

        bool used = true;
        juce::Label label;
        juce::Slider slider;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> att;
    };

    // Effects send + post EQ. Shares one editor page with Main.
    struct FxPage : public juce::Component,
                    private juce::AudioProcessorValueTreeState::Listener
    {
        explicit FxPage (SNR3AudioProcessor& p);
        ~FxPage() override;
        void resized() override;
        void parameterChanged (const juce::String&, float) override;
        void bindPreKnob();
        void refreshMix();

        SNR3AudioProcessor& proc;
        MixStrip dryStrip, fxStrip;
        juce::OwnedArray<Knob> knobs;
        juce::ToggleButton dstOn { "Dist" };
        juce::ToggleButton dlyOn { "Delay" };
        juce::ToggleButton dlySync { "Sync" };
        juce::ToggleButton revOn { "Rev" };
        juce::ToggleButton revSync { "Sync" };
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> dstOnAtt, dlyOnAtt, dlySyncAtt, revOnAtt, revSyncAtt;
        juce::ComboBox dstType, dlyDiv;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> dstTypeAtt, dlyDivAtt;
        juce::Label dlyDivLabel;
        EqPage eq;
    };

    PanelLnF panelLnF;
    Content content { *this };  // declared before its children so it is destroyed after them
    int baseW = 0, baseH = 0;   // unscaled size
    WaveView wave;
    LayerRail rail { *this };
    juce::OwnedArray<Knob> knobs;
    juce::Label lengthLabel;
    juce::ComboBox lengthBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lengthAtt;
    juce::Label holdLabel;
    juce::ComboBox holdBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> holdAtt;
    juce::ToggleButton sampleLoadBtn;
    PatchBar patchBar;
    PatchBar pageBar;
    VolumeStrip volumeStrip { *this };
    juce::ToggleButton playBtn { "Play" };
    juce::ToggleButton loopBtn { "Loop" };
    juce::TextEditor bpmBox;
    juce::ToggleButton eqBtn { "EQ" };
    juce::ToggleButton exportBtn { "Export WAV" };
    juce::ComboBox wavSrBox;
    juce::ToggleButton wavNormBtn { "Norm" };
    float loopBpm = 140.f;
    FxPage fxPage;
    EqPage layerEq;
    LayerMix layerMix;
    AboutBox aboutBox;
    int editorPage = 0;
    int selectedLayer = 0;
    bool layerEqOpen = false;
    bool mixView = false;
    juce::String knobSig;
    std::unique_ptr<juce::FileChooser> chooser;
    juce::Image logo;
    bool waveDirty = true;
    bool standalone = false;
    juce::TooltipWindow tip { this };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SNR3AudioProcessorEditor)
};
