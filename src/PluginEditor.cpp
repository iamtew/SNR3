#include "PluginEditor.h"
#include "dsp/WavExport.h"
#include "presets/FactoryPresets.h"
#include "ui/VersionCompare.h"

#include <cmath>
#include <thread>
#include <vector>

namespace
{
constexpr int kEditorW = 720;
constexpr int kEditorH = 530;
constexpr const char* kPageNames[] = { "Main", "Effects" };
constexpr int kPageCount = 2;
constexpr int kPad = 8;
constexpr int kTitleH = 40;
constexpr int kTitleHitW = 150;
constexpr int kLabelH = 12;

const juce::Colour kChassis { 0xff1a1410 };
const juce::Colour kMetal   { 0xffc4a35a };
const juce::Colour kDark    { 0xff000000 };
const juce::Colour kOrange  { 0xff8a3a22 };
const juce::Colour kWhite   { 0xffffffff };

juce::Font boldFont (float h)
{
    return juce::Font (juce::FontOptions (h).withStyle ("Bold"));
}

constexpr int kMenuInit = 1;
constexpr int kMenuSave = 2;
constexpr int kMenuSaveAs = 3;
constexpr int kMenuExport = 4;
constexpr int kMenuImport = 5;
constexpr int kMenuFactory = 1000;
constexpr int kMenuUser = 10000;

juce::Colour layerColour (int layer)
{
    switch (layer)
    {
        case 0: return juce::Colour (0xffc4a35a);
        case 1: return juce::Colour (0xffe8dcc8);
        case 2: return juce::Colour (0xff8a3a22);
        default: return juce::Colour (0xffc4a35a);
    }
}

const char* layerLetter (int layer)
{
    switch (layer)
    {
        case 0: return "Sh";
        case 1: return "Wi";
        case 2: return "St";
        default: return "Sa";
    }
}

const char layerMark (int layer)
{
    switch (layer)
    {
        case 0: return 'H';
        case 1: return 'W';
        case 2: return 'T';
        default: return 'A';
    }
}

template <typename Fn>
void eachLayerVoiceId (Fn&& fn)
{
    for (int layer = 0; layer < ParamID::layerCount; ++layer)
    {
        fn (ParamID::lyGain[layer]);
        fn (ParamID::lyMute[layer]);
        fn (ParamID::lySolo[layer]);
        fn (ParamID::lyFlip[layer]);
        fn (ParamID::lySend[layer]);
        fn (ParamID::lyDelay[layer]);
    }
    fn (ParamID::tune);
    fn (ParamID::shellDec);
    fn (ParamID::shellTone);
    fn (ParamID::wiresDec);
    fn (ParamID::wiresTone);
    fn (ParamID::stickDec);
    fn (ParamID::stickTone);
    fn (ParamID::sampleHp);
    fn (ParamID::sampleLp);
    fn (ParamID::sampleLen);
    fn (ParamID::duckDepth);
    fn (ParamID::duckTime);
}

template <typename Fn>
void eachLayerEqId (Fn&& fn)
{
    for (int layer = 0; layer < ParamID::layerCount; ++layer)
    {
        fn (ParamID::lyEqOn[layer]);
        for (int b = 0; b < ParamID::eqBandCount; ++b)
        {
            fn (ParamID::lyEqType[layer][b]);
            fn (ParamID::lyEqFreq[layer][b]);
            fn (ParamID::lyEqGain[layer][b]);
            fn (ParamID::lyEqQ[layer][b]);
        }
    }
}
} // namespace

SNR3AudioProcessorEditor::Knob::Knob (juce::AudioProcessorValueTreeState& st,
                                     const juce::String& id, const juce::String& name)
{
    label.setText (name, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, kMetal);
    label.setColour (juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    addAndMakeVisible (label);

    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 56, 12);
    slider.setColour (juce::Slider::textBoxTextColourId, kMetal);
    slider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible (slider);
    att = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (st, id, slider);
}

void SNR3AudioProcessorEditor::Knob::bind (juce::AudioProcessorValueTreeState& st, const char* id,
                                          const juce::String& name)
{
    att.reset();
    used = id != nullptr && id[0] != '\0';
    label.setText (name, juce::dontSendNotification);
    if (! used)
        return;
    if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*> (st.getParameter (id)))
        slider.setDoubleClickReturnValue (true, ranged->convertFrom0to1 (ranged->getDefaultValue()));
    att = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (st, id, slider);
}

void SNR3AudioProcessorEditor::Knob::resized()
{
    auto r = getLocalBounds();
    label.setBounds (r.removeFromTop (kLabelH));
    slider.setBounds (r);
}

SNR3AudioProcessorEditor::MixStrip::MixStrip (juce::AudioProcessorValueTreeState& st, const char* id,
                                             const char* name, snr3::OutputMeter& m)
    : meter (m)
{
    label.setText (name, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, kMetal);
    label.setColour (juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    addAndMakeVisible (label);

    slider.setLookAndFeel (&lnf);
    slider.setSliderStyle (juce::Slider::LinearVertical);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 40, 12);
    slider.setOpaque (false);
    slider.setColour (juce::Slider::backgroundColourId, juce::Colours::transparentBlack);
    slider.setColour (juce::Slider::trackColourId, juce::Colours::transparentBlack);
    slider.setColour (juce::Slider::thumbColourId, kMetal);
    slider.setColour (juce::Slider::textBoxTextColourId, kMetal);
    slider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    if (auto* p = dynamic_cast<juce::RangedAudioParameter*> (st.getParameter (id)))
        slider.setDoubleClickReturnValue (true, p->convertFrom0to1 (p->getDefaultValue()));
    att = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (st, id, slider);
    addAndMakeVisible (slider);
}

SNR3AudioProcessorEditor::MixStrip::~MixStrip()
{
    slider.setLookAndFeel (nullptr);
}

juce::Rectangle<float> SNR3AudioProcessorEditor::MixStrip::trackArea() const
{
    auto r = slider.getBounds().toFloat();
    r.removeFromBottom (12.f); // percent box
    return r.reduced (10.f, 4.f);
}

void SNR3AudioProcessorEditor::MixStrip::resized()
{
    auto r = getLocalBounds();
    label.setBounds (r.removeFromTop (kLabelH));
    slider.setBounds (r);
}

void SNR3AudioProcessorEditor::MixStrip::LnF::drawLinearSlider (juce::Graphics& g, int x, int, int w, int,
                                                              float pos, float, float,
                                                              juce::Slider::SliderStyle, juce::Slider&)
{
    // Cap only — meter fill is MixStrip::paint.
    const float cx = (float) x + (float) w * 0.5f;
    juce::Rectangle<float> cap (cx - 12.f, pos - 4.f, 24.f, 8.f);
    g.setColour (kDark);
    g.fillRoundedRectangle (cap, 2.f);
    g.setColour (kMetal);
    g.drawRoundedRectangle (cap, 2.f, 1.f);
}

void SNR3AudioProcessorEditor::MixStrip::paint (juce::Graphics& g)
{
    auto track = trackArea();
    g.setColour (kDark);
    g.fillRoundedRectangle (track, 3.f);

    const float peak = meter.lastPeak();
    const float db = peak > 1.0e-6f ? 20.f * std::log10 (peak) : -90.f;
    const float y01 = juce::jlimit (0.f, 1.f, (db + 48.f) / 48.f);
    if (y01 > 0.f)
    {
        auto fill = track.removeFromBottom (track.getHeight() * y01);
        g.setColour (clipped ? kOrange : kChassis.brighter (0.15f));
        g.fillRoundedRectangle (fill, 3.f);
    }
}

void SNR3AudioProcessorEditor::MixStrip::refresh()
{
    if (meter.takeClip())
        clipUntil = juce::Time::getMillisecondCounter() + 800;
    clipped = juce::Time::getMillisecondCounter() < clipUntil;
    repaint();
}

SNR3AudioProcessorEditor::FxPage::FxPage (SNR3AudioProcessor& p)
    : proc (p),
      dryStrip (p.apvts, ParamID::fxDry, "Dry", p.dryMeter()),
      fxStrip (p.apvts, ParamID::fxWet, "FX", p.fxMeter()),
      eq (p)
{
    auto setupOn = [this, &p] (juce::ToggleButton& b, const char* id,
                               std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>& att)
    {
        addAndMakeVisible (b);
        att = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (p.apvts, id, b);
    };
    setupOn (dstOn, ParamID::fxDstOn, dstOnAtt);
    setupOn (dlyOn, ParamID::fxDlyOn, dlyOnAtt);
    setupOn (dlySync, ParamID::fxDlySync, dlySyncAtt);
    setupOn (revOn, ParamID::fxRevOn, revOnAtt);
    setupOn (revSync, ParamID::fxRevSync, revSyncAtt);

    struct Spec { const char* id; const char* name; };
    static constexpr Spec kFxKnobs[] = {
        { ParamID::fxDstDrive, "Drive" }, { ParamID::fxDstTone, "Tone" }, { ParamID::fxDstShape, "Shape" },
        { ParamID::fxDlyTime, "Time" }, { ParamID::fxDlyFb, "Fb" }, { ParamID::fxDlyCut, "Cut" },
        { ParamID::fxRevPredelay, "Pre" }, { ParamID::fxRevSize, "Size" },
        { ParamID::fxRevDamp, "Damp" }, { ParamID::fxRevWidth, "Width" }
    };
    for (auto& s : kFxKnobs)
    {
        auto* k = knobs.add (new Knob (p.apvts, s.id, s.name));
        addAndMakeVisible (k);
    }
    addAndMakeVisible (dryStrip);
    addAndMakeVisible (fxStrip);

    dstType.addItem ("Soft", 1);
    dstType.addItem ("Hard", 2);
    dstType.addItem ("Fold", 3);
    dstType.addItem ("Diode", 4);
    dstType.setColour (juce::ComboBox::textColourId, kMetal);
    dstType.setColour (juce::ComboBox::arrowColourId, kMetal);
    addAndMakeVisible (dstType);
    dstTypeAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        p.apvts, ParamID::fxDstType, dstType);

    dlyDivLabel.setText ("Div", juce::dontSendNotification);
    dlyDivLabel.setJustificationType (juce::Justification::centred);
    dlyDivLabel.setColour (juce::Label::textColourId, kMetal);
    addAndMakeVisible (dlyDivLabel);
    dlyDiv.addItem ("1/16", 1);
    dlyDiv.addItem ("1/8", 2);
    dlyDiv.addItem ("1/4", 3);
    dlyDiv.addItem ("1/2", 4);
    dlyDiv.setColour (juce::ComboBox::textColourId, kMetal);
    dlyDiv.setColour (juce::ComboBox::arrowColourId, kMetal);
    addAndMakeVisible (dlyDiv);
    dlyDivAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        p.apvts, ParamID::fxDlyDiv, dlyDiv);

    p.apvts.addParameterListener (ParamID::fxRevSync, this);
    bindPreKnob();
    addAndMakeVisible (eq);
}

SNR3AudioProcessorEditor::FxPage::~FxPage()
{
    proc.apvts.removeParameterListener (ParamID::fxRevSync, this);
}

void SNR3AudioProcessorEditor::FxPage::parameterChanged (const juce::String& id, float)
{
    if (id != ParamID::fxRevSync)
        return;
    juce::MessageManager::callAsync ([safe = juce::Component::SafePointer<FxPage> (this)]
    {
        if (safe != nullptr)
            safe->bindPreKnob();
    });
}

void SNR3AudioProcessorEditor::FxPage::bindPreKnob()
{
    if (knobs.size() < 7)
        return;
    auto* k = knobs[6];
    k->att.reset();
    const bool sync = proc.apvts.getRawParameterValue (ParamID::fxRevSync)->load() >= 0.5f;
    k->att = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        proc.apvts, sync ? ParamID::fxRevDiv : ParamID::fxRevPredelay, k->slider);
}

void SNR3AudioProcessorEditor::FxPage::refreshMix()
{
    dryStrip.refresh();
    fxStrip.refresh();
}

void SNR3AudioProcessorEditor::FxPage::resized()
{
    auto r = getLocalBounds();
    auto fx = r.removeFromTop (110);
    r.removeFromTop (12); // gap between FX knobs and EQ
    eq.setBounds (r);

    auto mix = fx.removeFromLeft (92);
    dryStrip.setBounds (mix.removeFromLeft (44));
    mix.removeFromLeft (4);
    fxStrip.setBounds (mix);

    auto dist = fx.removeFromLeft (186);
    auto distTop = dist.removeFromTop (22);
    dstOn.setBounds (distTop.removeFromLeft (48).reduced (2, 1));
    dstType.setBounds (distTop.reduced (0, 1));
    const int dstW = dist.getWidth() / 3;
    if (knobs.size() >= 3)
    {
        knobs[0]->setBounds (dist.removeFromLeft (dstW));
        knobs[1]->setBounds (dist.removeFromLeft (dstW));
        knobs[2]->setBounds (dist);
    }

    fx.removeFromLeft (4);
    auto delay = fx.removeFromLeft (186);
    auto delayTop = delay.removeFromTop (22);
    dlyOn.setBounds (delayTop.removeFromLeft (52).reduced (2, 1));
    dlySync.setBounds (delayTop.removeFromLeft (48).reduced (2, 1));
    dlyDivLabel.setBounds (delayTop.removeFromLeft (28));
    dlyDiv.setBounds (delayTop.removeFromLeft (64).reduced (0, 1));
    const int dlyW = delay.getWidth() / 3;
    if (knobs.size() >= 6)
    {
        knobs[3]->setBounds (delay.removeFromLeft (dlyW));
        knobs[4]->setBounds (delay.removeFromLeft (dlyW));
        knobs[5]->setBounds (delay);
    }

    fx.removeFromLeft (4);
    auto revTop = fx.removeFromTop (22);
    revOn.setBounds (revTop.removeFromLeft (48).reduced (2, 1));
    revSync.setBounds (revTop.removeFromLeft (48).reduced (2, 1));
    const int revW = juce::jmax (1, fx.getWidth() / 4);
    if (knobs.size() >= 10)
    {
        knobs[6]->setBounds (fx.removeFromLeft (revW));
        knobs[7]->setBounds (fx.removeFromLeft (revW));
        knobs[8]->setBounds (fx.removeFromLeft (revW));
        knobs[9]->setBounds (fx);
    }
}

void SNR3AudioProcessorEditor::PanelLnF::drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h,
                                                          float pos, float startAngle, float endAngle,
                                                          juce::Slider&)
{
    // Inset so the ticks stay inside the slider, including the 36 px EQ knobs.
    auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h).reduced (1.f);
    const float d = juce::jmin (bounds.getWidth(), bounds.getHeight());
    if (d < 8.f)
        return;

    auto area = bounds.withSizeKeepingCentre (d, d);
    const auto c = area.getCentre();
    const float tickW = juce::jmax (1.f, d * 0.018f);

    g.setColour (kMetal.withAlpha (0.75f));
    constexpr int nTicks = 11;
    for (int i = 0; i < nTicks; ++i)
    {
        const float t = (float) i / (float) (nTicks - 1);
        const float a = startAngle + t * (endAngle - startAngle);
        const auto p0 = c.getPointOnCircumference (d * 0.48f, a);
        const auto p1 = c.getPointOnCircumference (d * 0.40f, a);
        g.drawLine (p0.x, p0.y, p1.x, p1.y, tickW);
    }

    const float bodyD = d * 0.72f;
    auto body = area.withSizeKeepingCentre (bodyD, bodyD);
    g.setColour (juce::Colours::black.withAlpha (0.22f));
    g.fillEllipse (body.translated (0.f, d * 0.04f).expanded (d * 0.01f));
    g.setColour (juce::Colours::black.withAlpha (0.4f));
    g.fillEllipse (body.translated (0.f, d * 0.018f));

    g.setGradientFill (juce::ColourGradient (kChassis.brighter (0.18f), c.x, body.getY(),
                                              kDark, c.x, body.getBottom(), false));
    g.fillEllipse (body);
    g.setColour (kMetal.withAlpha (0.3f));
    g.drawEllipse (body.reduced (0.6f), juce::jmax (1.f, d * 0.012f));

    const float capD = bodyD * 0.5f;
    auto cap = area.withSizeKeepingCentre (capD, capD);
    g.setColour (kOrange);
    g.fillEllipse (cap);

    const float angle = startAngle + pos * (endAngle - startAngle);
    juce::Path pointer;
    pointer.startNewSubPath (c.getPointOnCircumference (bodyD * 0.32f, angle));
    pointer.lineTo (c.getPointOnCircumference (bodyD * 0.46f, angle));
    g.setColour (kWhite);
    g.strokePath (pointer, juce::PathStrokeType (juce::jmax (1.15f, d * 0.028f),
                                                  juce::PathStrokeType::curved,
                                                  juce::PathStrokeType::rounded));

    const float pip = juce::jmax (1.6f, d * 0.045f);
    const auto pipC = c.getPointOnCircumference (capD * 0.28f, angle);
    g.fillEllipse (pipC.x - pip * 0.5f, pipC.y - pip * 0.5f, pip, pip);
}

void SNR3AudioProcessorEditor::PanelLnF::drawToggleButton (juce::Graphics& g, juce::ToggleButton& b, bool, bool)
{
    auto bounds = b.getLocalBounds().toFloat().reduced (2.f);
    g.setColour (b.getToggleState() ? kOrange.withAlpha (0.85f) : kDark);
    g.fillRoundedRectangle (bounds, 3.f);
    g.setFont (boldFont (11.f));
    g.setColour (b.getToggleState() ? kWhite : kMetal);
    g.drawText (b.getButtonText(), bounds.toNearestInt(), juce::Justification::centred, false);
}

void SNR3AudioProcessorEditor::PanelLnF::drawComboBox (juce::Graphics& g, int width, int height, bool,
                                                      int, int, int, int, juce::ComboBox&)
{
    auto r = juce::Rectangle<float> (0.f, 0.f, (float) width, (float) height);
    g.setColour (kDark);
    g.fillRoundedRectangle (r, 3.f);
    g.setColour (kMetal.withAlpha (0.35f));
    g.drawRoundedRectangle (r.reduced (0.5f), 3.f, 1.f);
}

juce::Font SNR3AudioProcessorEditor::PanelLnF::getLabelFont (juce::Label&)
{
    return boldFont (11.f);
}

juce::Font SNR3AudioProcessorEditor::PanelLnF::getComboBoxFont (juce::ComboBox&)
{
    return boldFont (11.f);
}

void SNR3AudioProcessorEditor::WaveView::paint (juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat();
    g.setColour (kDark);
    g.fillRoundedRectangle (r, 4.f);
    g.setColour (kMetal.withAlpha (0.25f));
    g.drawRoundedRectangle (r.reduced (0.5f), 4.f, 1.f);

    auto inner = r.reduced (6.f, 4.f);
    auto ruler = inner.removeFromTop (14.f);
    constexpr float kPreMs = 80.f;
    const int nOn = (int) onsets.size();
    auto strip = inner.removeFromBottom (12.f + (float) juce::jmax (1, nOn) * 7.f);

    if (durationSec > 0.f)
    {
        static constexpr float kSteps[] = { 0.01f, 0.02f, 0.05f, 0.1f, 0.2f, 0.5f, 1.f };
        float step = kSteps[(int) (sizeof kSteps / sizeof kSteps[0]) - 1];
        for (float s : kSteps)
            if (durationSec / s <= 8.f)
            {
                step = s;
                break;
            }

        const bool useMs = durationSec < 1.f;
        auto xAt = [&] (float t)
        {
            return ruler.getX() + (t / durationSec) * ruler.getWidth();
        };
        auto tickLabel = [&] (float t) -> juce::String
        {
            if (t <= 0.f)
                return "0";
            if (useMs)
                return juce::String (juce::roundToInt (t * 1000.f)) + " ms";
            return juce::String (t, 2) + " s";
        };

        g.setFont (boldFont (9.f));
        g.setColour (kMetal.withAlpha (0.4f));
        g.drawLine (ruler.getX(), ruler.getBottom(), ruler.getRight(), ruler.getBottom(), 1.f);

        for (float t = 0.f; t < durationSec - 0.4f * step; t += step)
        {
            const float x = xAt (t);
            g.setColour (kMetal.withAlpha (0.55f));
            g.drawLine (x, ruler.getBottom() - 4.f, x, ruler.getBottom(), 1.f);
            g.setColour (kMetal.withAlpha (0.75f));
            g.drawText (tickLabel (t),
                        juce::Rectangle<float> (x + 2.f, ruler.getY(), 52.f, ruler.getHeight() - 2.f),
                        juce::Justification::centredLeft, false);
        }
        g.setColour (kMetal.withAlpha (0.55f));
        g.drawLine (ruler.getRight(), ruler.getBottom() - 4.f, ruler.getRight(), ruler.getBottom(), 1.f);
        g.setColour (kMetal.withAlpha (0.75f));
        g.drawText (tickLabel (durationSec), ruler, juce::Justification::centredRight, false);
    }

    const float mid = inner.getCentreY();
    const float amp = inner.getHeight() * 0.42f;
    for (const auto& tr : traces)
    {
        if (tr.samples.size() < 2)
            continue;
        juce::Path p;
        const int n = (int) tr.samples.size();
        for (int i = 0; i < n; ++i)
        {
            const float x = inner.getX() + (float) i / (float) (n - 1) * inner.getWidth();
            const float y = mid - tr.samples[(size_t) i] * amp;
            if (i == 0)
                p.startNewSubPath (x, y);
            else
                p.lineTo (x, y);
        }
        g.setColour (tr.colour.withAlpha (tr.alpha));
        g.strokePath (p, juce::PathStrokeType (tr.alpha > 0.8f ? 1.6f : 1.15f));

        if (tr.attackSec > 0.0002f && durationSec > 0.f)
        {
            const float ax = inner.getX()
                + juce::jlimit (0.f, 1.f, tr.attackSec / durationSec) * inner.getWidth();
            g.setColour (tr.colour.withAlpha (0.18f));
            g.drawLine (ax, inner.getY(), ax, inner.getBottom(), 1.f);
        }
    }

    g.setColour (kMetal.withAlpha (0.35f));
    g.drawLine (strip.getX(), strip.getY(), strip.getRight(), strip.getY(), 1.f);
    g.setFont (boldFont (8.f));
    g.setColour (kMetal.withAlpha (0.7f));
    g.drawText ("0", strip.removeFromLeft (10.f), juce::Justification::centredLeft, false);
    g.drawText ("80 ms", strip.removeFromRight (36.f), juce::Justification::centredRight, false);

    bool anySolo = false;
    for (const auto& o : onsets)
        if (o.solo)
            anySolo = true;

    const float laneH = nOn > 0 ? (strip.getHeight() - 2.f) / (float) nOn : strip.getHeight();
    for (int i = 0; i < nOn; ++i)
    {
        const auto& o = onsets[(size_t) i];
        auto lane = juce::Rectangle<float> (strip.getX(), strip.getY() + 1.f + (float) i * laneH,
                                           strip.getWidth(), laneH - 1.f);
        const float x0 = lane.getX() + juce::jlimit (0.f, 1.f, o.delayMs / kPreMs) * lane.getWidth();
        const float span = juce::jmax (3.f, juce::jlimit (0.f, 1.f, o.attackMs / kPreMs) * lane.getWidth());
        auto bar = juce::Rectangle<float> (x0, lane.getY() + 1.f, span, juce::jmax (3.f, lane.getHeight() - 2.f));
        const bool hear = ! o.mute && (! anySolo || o.solo);
        float a = hear ? 0.9f : 0.22f;
        g.setColour (o.colour.withAlpha (a));
        g.fillRoundedRectangle (bar, 1.5f);
        if (o.solo)
        {
            g.setColour (kWhite.withAlpha (0.9f));
            g.drawRoundedRectangle (bar.expanded (0.5f), 1.5f, 1.f);
        }
        g.setColour (hear ? o.colour : o.colour.withAlpha (0.4f));
        g.drawText (juce::String::charToString ((juce::juce_wchar) o.letter),
                    juce::Rectangle<float> (bar.getRight() + 2.f, lane.getY(), 12.f, lane.getHeight()),
                    juce::Justification::centredLeft, false);
        if (o.mute)
            g.drawText ("M", juce::Rectangle<float> (lane.getRight() - 12.f, lane.getY(), 12.f, lane.getHeight()),
                        juce::Justification::centred, false);
        else if (o.solo)
            g.drawText ("S", juce::Rectangle<float> (lane.getRight() - 12.f, lane.getY(), 12.f, lane.getHeight()),
                        juce::Justification::centred, false);
    }
}

void SNR3AudioProcessorEditor::WaveView::mouseDrag (const juce::MouseEvent& e)
{
    if (dragStarted || onDragOut == nullptr || e.getDistanceFromDragStart() < 12)
        return;
    dragStarted = true;
    onDragOut();
}

SNR3AudioProcessorEditor::AboutBox::AboutBox()
{
    status.setJustificationType (juce::Justification::centred);
    status.setColour (juce::Label::textColourId, kMetal);
    status.setColour (juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    addAndMakeVisible (status);

    auto setupPlain = [] (juce::ToggleButton& b)
    {
        b.setClickingTogglesState (false);
    };
    setupPlain (checkBtn);
    setupPlain (closeBtn);
    checkBtn.onClick = [this] { checkForUpdate(); };
    closeBtn.onClick = [this] { setVisible (false); };
    addAndMakeVisible (checkBtn);
    addAndMakeVisible (closeBtn);

    releasesBtn.setColour (juce::HyperlinkButton::textColourId, kOrange);
    releasesBtn.setVisible (false);
    addAndMakeVisible (releasesBtn);
}

void SNR3AudioProcessorEditor::AboutBox::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black.withAlpha (0.55f));
    auto card = cardBounds().toFloat();
    g.setColour (kDark);
    g.fillRoundedRectangle (card, 6.f);
    g.setColour (kMetal.withAlpha (0.35f));
    g.drawRoundedRectangle (card.reduced (0.5f), 6.f, 1.f);

    auto r = cardBounds().reduced (16, 14);
    g.setColour (kMetal);
    g.setFont (boldFont (20.f));
    g.drawText ("SNR3", r.removeFromTop (26), juce::Justification::centred, false);
    g.setFont (boldFont (12.f));
    g.drawText ("v" JucePlugin_VersionString, r.removeFromTop (18), juce::Justification::centred, false);
    g.setFont (boldFont (11.f));
    g.setColour (kMetal.withAlpha (0.8f));
    g.drawText ("Stupid Audio", r.removeFromTop (16), juce::Justification::centred, false);
    g.drawText ("MIT", r.removeFromTop (16), juce::Justification::centred, false);
}

void SNR3AudioProcessorEditor::AboutBox::resized()
{
    auto r = cardBounds().reduced (16, 14);
    r.removeFromTop (26 + 18 + 16 + 16 + 8);
    checkBtn.setBounds (r.removeFromTop (22).withSizeKeepingCentre (160, 22));
    r.removeFromTop (6);
    status.setBounds (r.removeFromTop (22));
    r.removeFromTop (4);
    releasesBtn.setBounds (r.removeFromTop (20).withSizeKeepingCentre (140, 20));
    closeBtn.setBounds (cardBounds().removeFromBottom (36).reduced (90, 8));
}

void SNR3AudioProcessorEditor::AboutBox::mouseUp (const juce::MouseEvent& e)
{
    if (e.mouseWasClicked() && ! cardBounds().contains (e.getPosition()))
        setVisible (false);
}

juce::Rectangle<int> SNR3AudioProcessorEditor::AboutBox::cardBounds() const
{
    return getLocalBounds().withSizeKeepingCentre (320, 240);
}

void SNR3AudioProcessorEditor::AboutBox::applyResult (const juce::String& text, bool hasUpdate)
{
    fetching.store (false);
    status.setText (text, juce::dontSendNotification);
    releasesBtn.setVisible (hasUpdate);
}

void SNR3AudioProcessorEditor::AboutBox::checkForUpdate()
{
    if (fetching.exchange (true))
        return;
    status.setText ("Checking...", juce::dontSendNotification);
    releasesBtn.setVisible (false);
    juce::Component::SafePointer<AboutBox> safe (this);
    std::thread ([safe]
    {
        juce::String text;
        bool hasUpdate = false;
        juce::URL url ("https://api.github.com/repos/iamtew/SNR3/tags");
        const auto opts = juce::URL::InputStreamOptions (juce::URL::ParameterHandling::inAddress)
                              .withExtraHeaders ("User-Agent: SNR3\nAccept: application/vnd.github+json")
                              .withConnectionTimeoutMs (8000);
        if (auto stream = url.createInputStream (opts))
        {
            const auto parsed = juce::JSON::parse (stream->readEntireStreamAsString());
            if (auto* arr = parsed.getArray())
            {
                std::vector<std::string> names;
                names.reserve ((size_t) arr->size());
                for (const auto& v : *arr)
                    if (auto* obj = v.getDynamicObject())
                        names.push_back (obj->getProperty ("name").toString().toStdString());
                const auto newest = snr3::newestNewerTag (names, JucePlugin_VersionString);
                if (newest.empty())
                    text = "You are up to date.";
                else
                {
                    text = "Newer version available: " + juce::String (newest);
                    hasUpdate = true;
                }
            }
            else
                text = "Could not reach GitHub.";
        }
        else
            text = "Could not reach GitHub.";

        juce::MessageManager::callAsync ([safe, text, hasUpdate]
        {
            if (safe != nullptr)
                safe->applyResult (text, hasUpdate);
        });
    }).detach();
}

void SNR3AudioProcessorEditor::PatchBar::setText (const juce::String& t)
{
    if (text == t)
        return;
    text = t;
    repaint();
}

void SNR3AudioProcessorEditor::PatchBar::paint (juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat();
    g.setColour (kDark);
    g.fillRoundedRectangle (r, 4.f);
    g.setColour (kMetal.withAlpha (0.35f));
    g.drawRoundedRectangle (r.reduced (0.5f), 4.f, 1.f);

    auto left = r.removeFromLeft (18.f);
    auto right = r.removeFromRight (18.f);
    auto chev = [] (juce::Rectangle<float> box, bool back)
    {
        juce::Path p;
        const float cx = box.getCentreX();
        const float cy = box.getCentreY();
        if (back)
            p.addTriangle (cx + 3.f, cy - 5.f, cx - 4.f, cy, cx + 3.f, cy + 5.f);
        else
            p.addTriangle (cx - 3.f, cy - 5.f, cx + 4.f, cy, cx - 3.f, cy + 5.f);
        return p;
    };
    g.setColour (kMetal);
    g.fillPath (chev (left, true));
    g.fillPath (chev (right, false));
    g.setFont (boldFont (12.f));
    g.drawText (text, r.toNearestInt(), juce::Justification::centred, true);
}

void SNR3AudioProcessorEditor::PatchBar::mouseUp (const juce::MouseEvent& e)
{
    if (! e.mouseWasClicked())
        return;
    const int w = getWidth();
    if (e.x < 20)
    {
        if (onStep)
            onStep (-1);
    }
    else if (e.x > w - 20)
    {
        if (onStep)
            onStep (1);
    }
    else if (onOpen)
        onOpen();
}

SNR3AudioProcessorEditor::VolumeStrip::VolumeStrip (SNR3AudioProcessorEditor& ed)
    : editor (ed)
{
    slider.setLookAndFeel (&lnf);
    slider.setSliderStyle (juce::Slider::LinearHorizontal);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, true, 0, 0);
    slider.setOpaque (false);
    slider.setColour (juce::Slider::backgroundColourId, juce::Colours::transparentBlack);
    slider.setColour (juce::Slider::trackColourId, juce::Colours::transparentBlack);
    slider.setColour (juce::Slider::thumbColourId, kMetal);
    auto* p = dynamic_cast<juce::RangedAudioParameter*> (editor.proc.apvts.getParameter (ParamID::level));
    if (p != nullptr)
        slider.setDoubleClickReturnValue (true, p->convertFrom0to1 (p->getDefaultValue()));
    sliderAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        editor.proc.apvts, ParamID::level, slider);
    addAndMakeVisible (slider);
}

SNR3AudioProcessorEditor::VolumeStrip::~VolumeStrip()
{
    slider.setLookAndFeel (nullptr);
}

juce::Rectangle<float> SNR3AudioProcessorEditor::VolumeStrip::faderArea() const
{
    auto r = getLocalBounds().toFloat();
    return r.removeFromLeft (r.getWidth() * 5.f / 9.f);
}

juce::Rectangle<float> SNR3AudioProcessorEditor::VolumeStrip::scopeArea() const
{
    auto r = getLocalBounds().toFloat();
    r.removeFromLeft (r.getWidth() * 5.f / 9.f + 4.f);
    return r;
}

void SNR3AudioProcessorEditor::VolumeStrip::resized()
{
    slider.setBounds (faderArea().toNearestInt());
}

void SNR3AudioProcessorEditor::VolumeStrip::LnF::drawLinearSlider (juce::Graphics& g, int, int y, int, int h,
                                                                 float pos, float, float,
                                                                 juce::Slider::SliderStyle, juce::Slider&)
{
    const float by = (float) (y + h);
    juce::Path p;
    p.addTriangle (pos - 5.f, by, pos + 5.f, by, pos, by - 7.f);
    g.setColour (kDark);
    g.fillPath (p);
    g.setColour (kMetal);
    g.strokePath (p, juce::PathStrokeType (1.f));
}

void SNR3AudioProcessorEditor::VolumeStrip::paint (juce::Graphics& g)
{
    auto fader = faderArea();
    auto scope = scopeArea();

    g.setColour (kDark);
    g.fillRoundedRectangle (fader, 3.f);
    g.fillRoundedRectangle (scope, 3.f);

    const float pos = (float) slider.valueToProportionOfLength (slider.getValue());
    auto fill = fader.withWidth (juce::jmax (2.f, fader.getWidth() * pos));
    g.setColour (clipped ? kOrange : kChassis.brighter (0.15f));
    g.fillRoundedRectangle (fill, 3.f);

    g.setColour (kMetal.withAlpha (0.35f));
    for (int i = 1; i < 8; ++i)
    {
        const float x = fader.getX() + fader.getWidth() * (float) i / 8.f;
        const float tickH = (i == 4) ? 6.f : 3.f;
        g.drawLine (x, fader.getBottom() - 2.f, x, fader.getBottom() - 2.f - tickH, 1.f);
    }

    const int n = juce::jlimit (2, 256, (int) scope.getWidth());
    float specDb[256];
    editor.proc.eqAnalyser().copyLogDb (specDb, n);
    juce::Path spec;
    spec.startNewSubPath (scope.getX(), scope.getBottom());
    for (int i = 0; i < n; ++i)
    {
        const float x01 = (float) i / (float) (n - 1);
        const float y01 = 1.f - juce::jlimit (0.f, 1.f, (specDb[i] + 72.f) / 72.f);
        spec.lineTo (scope.getX() + x01 * scope.getWidth(),
                     scope.getY() + y01 * scope.getHeight());
    }
    spec.lineTo (scope.getRight(), scope.getBottom());
    spec.closeSubPath();
    g.setColour ((clipped ? kOrange : kMetal).withAlpha (0.7f));
    g.fillPath (spec);
}

void SNR3AudioProcessorEditor::VolumeStrip::refresh()
{
    if (editor.proc.outputMeter().takeClip())
        clipUntil = juce::Time::getMillisecondCounter() + 800;
    clipped = juce::Time::getMillisecondCounter() < clipUntil;
    repaint();
}

SNR3AudioProcessorEditor::SNR3AudioProcessorEditor (SNR3AudioProcessor& p)
    : juce::AudioProcessorEditor (p), proc (p), fxPage (p), layerEq (p), layerMix (p)
{
    setLookAndFeel (&panelLnF);
    setOpaque (true);
    panelLnF.setColour (juce::PopupMenu::backgroundColourId, kDark);
    panelLnF.setColour (juce::PopupMenu::textColourId, kMetal);
    panelLnF.setColour (juce::PopupMenu::highlightedBackgroundColourId, kOrange);
    panelLnF.setColour (juce::PopupMenu::highlightedTextColourId, kWhite);

    logo = juce::ImageCache::getFromMemory (BinaryData::logo_png, BinaryData::logo_pngSize);

    content.addAndMakeVisible (wave);
    wave.onDragOut = [this] { dragSnareFiles(); };

    content.addAndMakeVisible (rail);
    for (int i = 0; i < 18; ++i)
    {
        auto* k = knobs.add (new Knob (proc.apvts, ParamID::tune, "Tune"));
        content.addAndMakeVisible (k);
    }

    auto setupCombo = [this] (juce::Label& label, juce::ComboBox& box, const juce::String& name)
    {
        label.setText (name, juce::dontSendNotification);
        label.setJustificationType (juce::Justification::centred);
        label.setColour (juce::Label::textColourId, kMetal);
        content.addAndMakeVisible (label);
        box.setColour (juce::ComboBox::textColourId, kMetal);
        box.setColour (juce::ComboBox::arrowColourId, kMetal);
        content.addAndMakeVisible (box);
    };
    setupCombo (styleLabel, styleBox, "Style");
    styleBox.addItem ("Classic", 1);
    styleBox.addItem ("Fat", 2);
    styleBox.addItem ("Snap", 3);
    styleBox.addItem ("Tight", 4);
    styleBox.addItem ("Hard", 5);
    styleAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        proc.apvts, ParamID::style, styleBox);

    setupCombo (shapeLabel, shapeBox, "Shape");
    shapeBox.addItem ("Sine", 1);
    shapeBox.addItem ("Tri", 2);
    shapeAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        proc.apvts, ParamID::bodyShape, shapeBox);

    setupCombo (lengthLabel, lengthBox, "Length");
    lengthBox.addItem ("Free", 1);
    lengthBox.addItem ("1/4", 2);
    lengthBox.addItem ("1/8", 3);
    lengthBox.addItem ("1/16", 4);
    lengthAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        proc.apvts, ParamID::length, lengthBox);

    setupCombo (holdLabel, holdBox, "Hold");
    holdBox.addItem ("One-shot", 1);
    holdBox.addItem ("Hold", 2);
    holdAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        proc.apvts, ParamID::holdMode, holdBox);

    sampleLoadBtn.setButtonText ("Load");
    sampleLoadBtn.setClickingTogglesState (false);
    sampleLoadBtn.onClick = [this] { chooseSample(); };
    content.addAndMakeVisible (sampleLoadBtn);

    patchBar.onStep = [this] (int d) { cyclePatch (d); };
    patchBar.onOpen = [this] { openPatchMenu(); };
    content.addAndMakeVisible (patchBar);
    refreshPatchName();
    pageBar.setText (kPageNames[0]);
    pageBar.onStep = [this] (int d) { cyclePage (d); };
    pageBar.onOpen = [this] { openPageMenu(); };
    content.addAndMakeVisible (pageBar);
    content.addAndMakeVisible (volumeStrip);

    standalone = juce::PluginHostType::getPluginLoadedAs() == juce::AudioProcessor::wrapperType_Standalone;
    playBtn.setClickingTogglesState (false);
    playBtn.onClick = [this]
    {
        if (loopBtn.getToggleState())
        {
            const bool on = ! playBtn.getToggleState();
            commitLoopBpm();
            proc.setLooping (on, loopBpm);
            playBtn.setToggleState (on, juce::dontSendNotification);
        }
        else
        {
            proc.requestTrigger();
        }
    };
    exportBtn.setClickingTogglesState (false);
    exportBtn.onClick = [this] { exportWav(); };
    loopBtn.setClickingTogglesState (true);
    loopBtn.onClick = [this]
    {
        if (! loopBtn.getToggleState() && playBtn.getToggleState())
        {
            proc.setLooping (false, loopBpm);
            playBtn.setToggleState (false, juce::dontSendNotification);
        }
    };
    bpmBox.setText ("140", juce::dontSendNotification);
    bpmBox.setInputRestrictions (3, "0123456789");
    bpmBox.setJustification (juce::Justification::centred);
    bpmBox.setColour (juce::TextEditor::textColourId, kMetal);
    bpmBox.setColour (juce::TextEditor::backgroundColourId, kDark);
    bpmBox.setColour (juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
    bpmBox.onReturnKey = [this] { commitLoopBpm(); };
    bpmBox.onFocusLost = [this] { commitLoopBpm(); };
    content.addAndMakeVisible (playBtn);
    content.addAndMakeVisible (loopBtn);
    content.addAndMakeVisible (bpmBox);
    content.addAndMakeVisible (exportBtn);
    wavSrBox.addItem ("44.1", 1);
    wavSrBox.addItem ("48", 2);
    wavSrBox.setSelectedId (proc.wavSr >= 47000.0 ? 2 : 1, juce::dontSendNotification);
    wavSrBox.onChange = [this]
    {
        proc.wavSr = wavSrBox.getSelectedId() == 2 ? 48000.0 : 44100.0;
    };
    wavSrBox.setColour (juce::ComboBox::textColourId, kMetal);
    wavSrBox.setColour (juce::ComboBox::arrowColourId, kMetal);
    content.addAndMakeVisible (wavSrBox);
    wavNormBtn.setClickingTogglesState (true);
    wavNormBtn.setToggleState (proc.wavNorm, juce::dontSendNotification);
    wavNormBtn.onClick = [this] { proc.wavNorm = wavNormBtn.getToggleState(); };
    content.addAndMakeVisible (wavNormBtn);
    playBtn.setVisible (standalone);
    loopBtn.setVisible (standalone);
    bpmBox.setVisible (standalone);

    eqBtn.setClickingTogglesState (true);
    eqBtn.onClick = [this]
    {
        layerEqOpen = eqBtn.getToggleState();
        applyPageVisibility();
    };
    content.addAndMakeVisible (eqBtn);
    selectLayer (0);

    content.addAndMakeVisible (fxPage);
    fxPage.setVisible (false);
    content.addChildComponent (layerEq);
    content.addChildComponent (layerMix);

    content.addChildComponent (aboutBox);
    aboutBox.setVisible (false);

    for (auto* id : { ParamID::tune, ParamID::shellDec, ParamID::shellTone, ParamID::wiresDec,
                      ParamID::wiresTone, ParamID::stickDec, ParamID::stickTone, ParamID::sampleHp,
                      ParamID::sampleLp, ParamID::sampleLen, ParamID::level, ParamID::length,
                      ParamID::holdMode, ParamID::style, ParamID::bodyShape, ParamID::velAmt,
                      ParamID::width, ParamID::duckDepth, ParamID::duckTime })
        proc.apvts.addParameterListener (id, this);
    eachLayerEqId ([this] (const char* id) { proc.apvts.addParameterListener (id, this); });
    eachLayerVoiceId ([this] (const char* id) { proc.apvts.addParameterListener (id, this); });

    baseW = kEditorW;
    baseH = kEditorH;
    content.setSize (baseW, baseH);
    addAndMakeVisible (content);

    // Drag the corner to scale (40%..150%), aspect ratio locked. The scale is saved with the session.
    // Read it first: setResizeLimits clamps the still-empty editor and resized() overwrites uiScale.
    const float s = juce::jlimit (0.4f, 1.5f, proc.uiScale);
    setResizable (true, true);
    setResizeLimits (baseW * 2 / 5, baseH * 2 / 5, baseW * 3 / 2, baseH * 3 / 2);
    getConstrainer()->setFixedAspectRatio ((double) baseW / (double) baseH);
    setSize (juce::roundToInt ((float) baseW * s), juce::roundToInt ((float) baseH * s));
    startTimerHz (15);
}

SNR3AudioProcessorEditor::~SNR3AudioProcessorEditor()
{
    stopTimer();
    for (auto* id : { ParamID::tune, ParamID::shellDec, ParamID::shellTone, ParamID::wiresDec,
                      ParamID::wiresTone, ParamID::stickDec, ParamID::stickTone, ParamID::sampleHp,
                      ParamID::sampleLp, ParamID::sampleLen, ParamID::level, ParamID::length,
                      ParamID::holdMode, ParamID::style, ParamID::bodyShape, ParamID::velAmt,
                      ParamID::width, ParamID::duckDepth, ParamID::duckTime })
        proc.apvts.removeParameterListener (id, this);
    eachLayerEqId ([this] (const char* id) { proc.apvts.removeParameterListener (id, this); });
    eachLayerVoiceId ([this] (const char* id) { proc.apvts.removeParameterListener (id, this); });
    setLookAndFeel (nullptr);
}

void SNR3AudioProcessorEditor::parameterChanged (const juce::String&, float)
{
    waveDirty = true;
}

void SNR3AudioProcessorEditor::timerCallback()
{
    if (waveDirty)
        rebuildWave();
    bindKnobs();
    if (layerMix.isVisible())
        layerMix.refresh();
    rail.repaint();
    refreshPatchName();
    volumeStrip.refresh();
    if (fxPage.isVisible())
        fxPage.refreshMix();
}

void SNR3AudioProcessorEditor::rebuildWave()
{
    waveDirty = false;
    constexpr int fullN = 44100;
    constexpr int bins = 1024;
    std::vector<float> mix ((size_t) fullN, 0.f);
    proc.renderShot (mix.data(), fullN, 44100.0);

    int last = 64;
    {
        int end = fullN - 1;
        while (end > 64 && std::abs (mix[(size_t) end]) < 0.002f)
            --end;
        last = end;
    }

    auto downsample = [&] (const float* src, int nSrc, juce::Colour colour, float alpha, float attack)
    {
        SNR3AudioProcessorEditor::WaveView::Trace tr;
        tr.colour = colour;
        tr.alpha = alpha;
        tr.attackSec = attack;
        tr.samples.resize ((size_t) bins);
        for (int i = 0; i < bins; ++i)
        {
            const int idx = (int) ((double) i / (double) (bins - 1) * (double) nSrc);
            tr.samples[(size_t) i] = juce::jlimit (-1.f, 1.f, src[idx]);
        }
        wave.traces.push_back (std::move (tr));
    };

    wave.durationSec = (float) last / 44100.f;
    wave.traces.clear();
    downsample (mix.data(), last, kMetal, 1.f, 0.f);

    if (! mixView && selectedLayer >= 0)
    {
        std::vector<float> storage[snr3::kMixLayers];
        float* chan[snr3::kMixLayers] = {};
        storage[selectedLayer].assign ((size_t) fullN, 0.f);
        chan[selectedLayer] = storage[selectedLayer].data();
        proc.renderLayerChans (chan, fullN, 44100.0);
        downsample (storage[selectedLayer].data(), last, layerColour (selectedLayer), 0.7f, 0.f);
    }

    auto rawf = [this] (const char* id) -> float
    {
        if (auto* v = proc.apvts.getRawParameterValue (id))
            return v->load();
        return 0.f;
    };
    wave.onsets.clear();
    for (int layer = 0; layer < ParamID::layerCount; ++layer)
    {
        SNR3AudioProcessorEditor::WaveView::Onset o;
        o.colour = layerColour (layer);
        o.letter = layerMark (layer);
        o.delayMs = rawf (ParamID::lyDelay[layer]);
        o.attackMs = 0.f;
        o.mute = rawf (ParamID::lyMute[layer]) >= 0.5f;
        o.solo = rawf (ParamID::lySolo[layer]) >= 0.5f;
        wave.onsets.push_back (o);
    }
    wave.repaint();
}

void SNR3AudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (kChassis);  // covers the sub-pixel edge the scaled content may leave
}

void SNR3AudioProcessorEditor::resized()
{
    if (baseW == 0)
        return;  // called before the constructor finished
    const float s = (float) getWidth() / (float) baseW;
    content.setTransform (juce::AffineTransform::scale (s));
    proc.uiScale = s;
}

void SNR3AudioProcessorEditor::paintContent (juce::Graphics& g)
{
    g.fillAll (kChassis);

    auto r = content.getLocalBounds().reduced (kPad);
    auto title = r.removeFromTop (kTitleH);
    if (logo.isValid())
    {
        auto logoR = title.removeFromLeft (kTitleH).reduced (2);
        g.setImageResamplingQuality (juce::Graphics::highResamplingQuality);
        g.drawImage (logo, logoR.toFloat(), juce::RectanglePlacement::centred);
        title.removeFromLeft (6);
    }
    auto nameCol = title.removeFromLeft (90);
    g.setColour (kMetal);
    g.setFont (boldFont (16.f));
    g.drawText ("SNR3", nameCol.removeFromTop (22), juce::Justification::centredLeft, false);
    g.setFont (boldFont (10.f));
    g.drawText ("v" JucePlugin_VersionString, nameCol, juce::Justification::centredLeft, false);

    g.setColour (kChassis.brighter (0.08f));
    g.drawRoundedRectangle (content.getLocalBounds().toFloat().reduced (1.5f), 2.f, 2.f);
}

void SNR3AudioProcessorEditor::layoutContent()
{
    auto r = content.getLocalBounds().reduced (kPad);
    auto title = r.removeFromTop (kTitleH);
    title.removeFromLeft (kTitleHitW);
    volumeStrip.setBounds (title.removeFromRight (240).reduced (2, 4));
    pageBar.setBounds (title.removeFromLeft (88).reduced (4, 4));
    patchBar.setBounds (title.reduced (4, 4));

    auto tools = r.removeFromTop (28);
    eqBtn.setBounds (tools.removeFromRight (52).reduced (0, 2));
    styleLabel.setBounds (tools.removeFromLeft (44));
    styleBox.setBounds (tools.removeFromLeft (80).reduced (0, 2));
    tools.removeFromLeft (4);
    shapeLabel.setBounds (tools.removeFromLeft (40));
    shapeBox.setBounds (tools.removeFromLeft (64).reduced (0, 2));
    tools.removeFromLeft (8);
    lengthLabel.setBounds (tools.removeFromLeft (52));
    lengthBox.setBounds (tools.removeFromLeft (80).reduced (0, 2));
    tools.removeFromLeft (4);
    holdLabel.setBounds (tools.removeFromLeft (36));
    holdBox.setBounds (tools.removeFromLeft (80).reduced (0, 2));
    if (standalone)
    {
        tools.removeFromLeft (8);
        playBtn.setBounds (tools.removeFromLeft (56).reduced (0, 2));
        loopBtn.setBounds (tools.removeFromLeft (52).reduced (0, 2));
        bpmBox.setBounds (tools.removeFromLeft (44).reduced (0, 4));
    }
    tools.removeFromLeft (8);
    exportBtn.setBounds (tools.removeFromLeft (88).reduced (0, 2));
    wavSrBox.setBounds (tools.removeFromLeft (56).reduced (0, 2));
    wavNormBtn.setBounds (tools.removeFromLeft (48).reduced (0, 2));

    fxPage.setBounds (r);
    auto waveRow = r.removeFromTop (110).reduced (0, 4);
    rail.setBounds (waveRow.removeFromLeft (36));
    wave.setBounds (waveRow);
    r.removeFromTop (6);
    layerEq.setBounds (r);
    const int cols = 6;
    const int rows = 3;
    if (mixView)
    {
        const int duckH = juce::jmin (110, r.getHeight() / 4);
        auto duckRow = r.removeFromTop (duckH);
        const int dw = duckRow.getWidth() / cols;
        knobs[0]->setBounds (duckRow.removeFromLeft (dw));
        knobs[1]->setBounds (duckRow.removeFromLeft (dw));
        for (int i = 2; i < knobs.size(); ++i)
            knobs[i]->setBounds ({});
    }
    layerMix.setBounds (r);
    const int cellW = r.getWidth() / cols;
    const int cellH = r.getHeight() / rows;
    auto cellAt = [&] (int i)
    {
        return juce::Rectangle<int> (r.getX() + (i % cols) * cellW,
                                     r.getY() + (i / cols) * cellH,
                                     cellW, cellH);
    };
    const bool showSampleLoad = editorPage == 0 && ! layerEqOpen && ! mixView && selectedLayer == 3;
    if (showSampleLoad)
        sampleLoadBtn.setBounds (cellAt (4).reduced (8, juce::jmax (0, (cellH - 28) / 2)));
    if (! mixView)
        for (int i = 0; i < knobs.size(); ++i)
            knobs[i]->setBounds (cellAt (i));

    aboutBox.setBounds (content.getLocalBounds());
    applyPageVisibility();
}

juce::Rectangle<int> SNR3AudioProcessorEditor::titleHitBounds() const
{
    return { kPad, kPad, kTitleHitW, kTitleH };
}

void SNR3AudioProcessorEditor::setEditorPage (int page)
{
    editorPage = juce::jlimit (0, kPageCount - 1, page);
    pageBar.setText (kPageNames[editorPage]);
    applyPageVisibility();
}

void SNR3AudioProcessorEditor::cyclePage (int delta)
{
    setEditorPage ((editorPage + delta + kPageCount) % kPageCount);
}

void SNR3AudioProcessorEditor::openPageMenu()
{
    juce::PopupMenu m;
    for (int i = 0; i < kPageCount; ++i)
        m.addItem (i + 1, kPageNames[i], true, i == editorPage);

    juce::Component::SafePointer<SNR3AudioProcessorEditor> safe (this);
    m.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (pageBar),
                     [safe] (int result)
                     {
                         if (safe == nullptr || result <= 0)
                             return;
                         safe->setEditorPage (result - 1);
                     });
}

void SNR3AudioProcessorEditor::bindKnobs()
{
    juce::String sig = mixView ? "mix" : ("L" + juce::String (selectedLayer));
    if (sig == knobSig)
        return;
    knobSig = sig;

    struct Spec { const char* id; const char* name; };
    Spec spec[18] = {};
    int n = 0;
    auto add = [&] (const char* id, const char* name)
    {
        if (n < 18)
            spec[n++] = { id, name };
    };

    if (mixView)
    {
        add (ParamID::duckDepth, "Duck");
        add (ParamID::duckTime, "Time");
    }
    else if (selectedLayer == 0)
    {
        add (ParamID::tune, "Tune");
        add (ParamID::shellDec, "Decay");
        add (ParamID::shellTone, "Tone");
        add (ParamID::velAmt, "Vel");
        add (ParamID::width, "Width");
        add (ParamID::lyDelay[0], "Delay");
    }
    else if (selectedLayer == 1)
    {
        add (ParamID::wiresDec, "Decay");
        add (ParamID::wiresTone, "Tone");
        add (ParamID::lyDelay[1], "Pre");
    }
    else if (selectedLayer == 2)
    {
        add (ParamID::stickDec, "Decay");
        add (ParamID::stickTone, "Tone");
        add (ParamID::lyDelay[2], "Pre");
    }
    else
    {
        add (ParamID::sampleHp, "HPF");
        add (ParamID::sampleLp, "LPF");
        add (ParamID::sampleLen, "Length");
        add (ParamID::lyDelay[3], "Pre");
    }

    for (int i = 0; i < knobs.size(); ++i)
    {
        if (i < n)
            knobs[i]->bind (proc.apvts, spec[i].id, spec[i].name);
        else
            knobs[i]->bind (proc.apvts, nullptr, {});
    }

    layoutContent();
}

void SNR3AudioProcessorEditor::selectLayer (int layer)
{
    mixView = false;
    selectedLayer = juce::jlimit (0, ParamID::layerCount - 1, layer);
    layerEq.bindTo (snr3::layerEqIds (selectedLayer), proc.layerAnalyser (selectedLayer));
    knobSig.clear();
    bindKnobs();
    waveDirty = true;
    rail.repaint();
    applyPageVisibility();
}

void SNR3AudioProcessorEditor::selectMix()
{
    mixView = true;
    knobSig.clear();
    bindKnobs();
    rail.repaint();
    applyPageVisibility();
}

void SNR3AudioProcessorEditor::applyPageVisibility()
{
    const bool main = editorPage == 0;
    const bool showEq = main && layerEqOpen && ! mixView;
    const bool showKnobs = main && ! layerEqOpen && ! mixView;
    const bool showMixKnobs = main && mixView;
    wave.setVisible (main);
    rail.setVisible (main);
    for (auto* k : knobs)
        k->setVisible (((showKnobs || showMixKnobs) && k->used));
    sampleLoadBtn.setVisible (showKnobs && selectedLayer == 3);
    layerEq.setVisible (showEq);
    layerMix.setVisible (main && mixView);
    eqBtn.setVisible (main && ! mixView);
    fxPage.setVisible (! main);
}

void SNR3AudioProcessorEditor::LayerRail::layoutItems (juce::Array<Item>& items) const
{
    items.clear();
    const int h = 18;
    int y = 0;
    auto push = [&] (Item it)
    {
        it.bounds = { 0, y, getWidth(), h };
        y += h + 2;
        items.add (it);
    };
    for (int i = 0; i < ParamID::layerCount; ++i)
        push ({ i, i, false, false, {} });
    push ({ -1, 0, true, false, {} });
}

void SNR3AudioProcessorEditor::LayerRail::paint (juce::Graphics& g)
{
    juce::Array<Item> items;
    layoutItems (items);
    for (const auto& it : items)
    {
        const bool on = it.mix ? ed.mixView : (! ed.mixView && ed.selectedLayer == it.layer);
        const juce::Colour col = it.mix ? kMetal : layerColour (it.layer);
        auto r = it.bounds.toFloat().reduced (1.f, 0.f);
        g.setColour (on ? col.withAlpha (0.9f) : col.withAlpha (0.18f));
        g.fillRoundedRectangle (r, 3.f);
        g.setColour (col.withAlpha (on ? 1.f : 0.7f));
        g.drawRoundedRectangle (r, 3.f, 1.f);
        g.setColour (on ? kDark : col);
        g.setFont (boldFont (11.f));
        const char* text = it.mix ? "Mx" : layerLetter (it.layer);
        g.drawText (text, it.bounds, juce::Justification::centred, false);
    }
}

void SNR3AudioProcessorEditor::LayerRail::mouseDown (const juce::MouseEvent& e)
{
    juce::Array<Item> items;
    layoutItems (items);
    const Item* hit = nullptr;
    for (const auto& it : items)
        if (it.bounds.contains (e.getPosition()))
            hit = &it;
    if (hit == nullptr)
        return;
    if (hit->mix)
        ed.selectMix();
    else if (hit->layer >= 0)
        ed.selectLayer (hit->layer);
}

void SNR3AudioProcessorEditor::contentMouseUp (const juce::MouseEvent& e)
{
    if (e.mouseWasClicked() && titleHitBounds().contains (e.getPosition()))
        openAbout();
}

void SNR3AudioProcessorEditor::refreshPatchName()
{
    const auto name = proc.patchName();
    if (const auto* preset = snr3::findFactoryPreset (name.toRawUTF8()))
        patchBar.setText (juce::String (preset->category) + " / " + name);
    else
        patchBar.setText (name);
}

void SNR3AudioProcessorEditor::cyclePatch (int delta)
{
    const auto names = proc.patchNames();
    if (names.isEmpty())
        return;
    const auto cur = proc.patchName();
    int idx = names.indexOf (cur);
    if (idx < 0)
        idx = delta > 0 ? 0 : names.size() - 1;
    else
        idx = (idx + delta + names.size()) % names.size();
    proc.loadPatchByName (names[idx]);
    refreshPatchName();
}

void SNR3AudioProcessorEditor::openPatchMenu()
{
    juce::PopupMenu m;
    const auto cur = proc.patchName();
    m.addItem (kMenuInit, "Init Patch", true, cur == "Init Patch");
    m.addSeparator();
    m.addItem (kMenuSave, "Save");
    m.addItem (kMenuSaveAs, "Save As...");
    m.addItem (kMenuExport, "Export...");
    m.addItem (kMenuImport, "Import...");
    m.addSeparator();

    {
        juce::String cat;
        juce::PopupMenu sub;
        for (int i = 0; i < snr3::factoryPresetCount; ++i)
        {
            const auto& preset = snr3::factoryPresets[i];
            const juce::String next (preset.category);
            if (next != cat)
            {
                if (sub.getNumItems() > 0)
                    m.addSubMenu (cat, sub);
                sub = juce::PopupMenu();
                cat = next;
            }
            sub.addItem (kMenuFactory + i, preset.name, true, cur == preset.name);
        }
        if (sub.getNumItems() > 0)
            m.addSubMenu (cat, sub);
    }

    juce::StringArray userOnly;
    for (const auto& name : proc.patchNames())
        if (snr3::findFactoryPreset (name.toRawUTF8()) == nullptr)
            userOnly.add (name);
    if (userOnly.size() > 0)
    {
        m.addSeparator();
        for (int i = 0; i < userOnly.size(); ++i)
            m.addItem (kMenuUser + i, userOnly[i], true, userOnly[i] == cur);
    }

    juce::Component::SafePointer<SNR3AudioProcessorEditor> safe (this);
    m.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (patchBar),
                     [safe, userOnly] (int result)
                     {
                         if (safe == nullptr || result <= 0)
                             return;
                         if (result == kMenuInit)
                             safe->proc.initPatch();
                         else if (result == kMenuSave)
                         {
                             if (! safe->proc.savePatch())
                                 safe->chooseSaveAs();
                         }
                         else if (result == kMenuSaveAs)
                             safe->chooseSaveAs();
                         else if (result == kMenuExport)
                             safe->chooseExport();
                         else if (result == kMenuImport)
                             safe->chooseImport();
                         else if (result >= kMenuUser)
                         {
                             const int i = result - kMenuUser;
                             if (juce::isPositiveAndBelow (i, userOnly.size()))
                                 safe->proc.loadPatchByName (userOnly[i]);
                         }
                         else if (result >= kMenuFactory)
                         {
                             const int i = result - kMenuFactory;
                             if (juce::isPositiveAndBelow (i, snr3::factoryPresetCount))
                                 safe->proc.loadPatchByName (snr3::factoryPresets[i].name);
                         }
                         safe->refreshPatchName();
                     });
}

void SNR3AudioProcessorEditor::chooseSaveAs()
{
    const auto start = proc.patchesDir().getChildFile ("Patch.snr3p");
    chooser = std::make_unique<juce::FileChooser> ("Save Patch", start, "*.snr3p");
    juce::Component::SafePointer<SNR3AudioProcessorEditor> safe (this);
    chooser->launchAsync (juce::FileBrowserComponent::saveMode
                              | juce::FileBrowserComponent::canSelectFiles
                              | juce::FileBrowserComponent::warnAboutOverwriting,
                          [safe] (const juce::FileChooser& c)
                          {
                              if (safe == nullptr)
                                  return;
                              auto file = c.getResult();
                              if (file.getFullPathName().isEmpty())
                                  return;
                              safe->proc.savePatchAs (file.getFileNameWithoutExtension());
                              safe->refreshPatchName();
                          });
}

void SNR3AudioProcessorEditor::chooseExport()
{
    const auto start = juce::File::getSpecialLocation (juce::File::userDocumentsDirectory)
                           .getChildFile ("SNR3 Patch.snr3p");
    chooser = std::make_unique<juce::FileChooser> ("Export Patch", start, "*.snr3p");
    juce::Component::SafePointer<SNR3AudioProcessorEditor> safe (this);
    chooser->launchAsync (juce::FileBrowserComponent::saveMode
                              | juce::FileBrowserComponent::canSelectFiles
                              | juce::FileBrowserComponent::warnAboutOverwriting,
                          [safe] (const juce::FileChooser& c)
                          {
                              if (safe == nullptr)
                                  return;
                              auto file = c.getResult();
                              if (file.getFullPathName().isEmpty())
                                  return;
                              safe->proc.exportPatch (file);
                          });
}

void SNR3AudioProcessorEditor::chooseImport()
{
    chooser = std::make_unique<juce::FileChooser> (
        "Import Patch",
        juce::File::getSpecialLocation (juce::File::userDocumentsDirectory),
        "*.snr3p");
    juce::Component::SafePointer<SNR3AudioProcessorEditor> safe (this);
    chooser->launchAsync (juce::FileBrowserComponent::openMode
                              | juce::FileBrowserComponent::canSelectFiles,
                          [safe] (const juce::FileChooser& c)
                          {
                              if (safe == nullptr)
                                  return;
                              auto file = c.getResult();
                              if (! file.existsAsFile())
                                  return;
                              safe->proc.importPatch (file);
                              safe->refreshPatchName();
                          });
}

void SNR3AudioProcessorEditor::chooseSample()
{
    chooser = std::make_unique<juce::FileChooser> (
        "Load sample",
        juce::File::getSpecialLocation (juce::File::userDocumentsDirectory),
        "*.wav;*.aif;*.aiff;*.flac");
    juce::Component::SafePointer<SNR3AudioProcessorEditor> safe (this);
    chooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                          [safe] (const juce::FileChooser& c)
                          {
                              if (safe == nullptr)
                                  return;
                              auto file = c.getResult();
                              if (file.getFullPathName().isEmpty())
                                  return;
                              safe->proc.loadSample (file);
                              safe->waveDirty = true;
                          });
}

void SNR3AudioProcessorEditor::openAbout()
{
    aboutBox.setVisible (true);
    aboutBox.toFront (true);
}

void SNR3AudioProcessorEditor::commitLoopBpm()
{
    const float v = bpmBox.getText().getFloatValue();
    if (v >= 40.f && v <= 240.f)
        loopBpm = v;
    bpmBox.setText (juce::String (juce::roundToInt (loopBpm)), juce::dontSendNotification);
    if (playBtn.getToggleState())
        proc.setLooping (true, loopBpm);
}

void SNR3AudioProcessorEditor::exportWav()
{
    commitLoopBpm();
    chooser = std::make_unique<juce::FileChooser> ("Export snare",
                                                   juce::File::getSpecialLocation (juce::File::userDesktopDirectory)
                                                       .getChildFile ("SNR3.wav"),
                                                   "*.wav");
    juce::Component::SafePointer<SNR3AudioProcessorEditor> safe (this);
    chooser->launchAsync (juce::FileBrowserComponent::saveMode
                              | juce::FileBrowserComponent::canSelectFiles
                              | juce::FileBrowserComponent::warnAboutOverwriting,
                          [safe] (const juce::FileChooser& c)
                          {
                              if (safe == nullptr)
                                  return;
                              auto file = c.getResult();
                              if (file.getFullPathName().isEmpty())
                                  return;
                              if (! file.hasFileExtension (".wav"))
                                  file = file.withFileExtension (".wav");

                              const double sr = safe->proc.wavSr;
                              juce::AudioBuffer<float> buf (2, 1);
                              int start = 0;
                              int count = 0;
                              if (safe->loopBtn.getToggleState())
                              {
                                  safe->proc.renderBars (buf, sr, safe->loopBpm, 2);
                                  count = buf.getNumSamples() / 2;
                                  start = count;
                              }
                              else
                              {
                                  count = (int) (sr * 4.0); // hit + ~2 s FX tail
                                  buf.setSize (2, count);
                                  safe->proc.renderWithPost (buf, sr);
                              }
                              snr3::writeWav (file, buf, sr, start, count, safe->proc.wavNorm);
                          });
}

void SNR3AudioProcessorEditor::dragSnareFiles()
{
    const double sr = proc.wavSr;
    const int count = (int) (sr * 4.0);
    juce::AudioBuffer<float> buf (2, count);
    proc.renderWithPost (buf, sr);
    const auto dir = juce::File::getSpecialLocation (juce::File::tempDirectory);
    const auto wav = dir.getChildFile ("SNR3.wav");
    const auto patch = dir.getChildFile ("SNR3.snr3p");
    wav.deleteFile();
    patch.deleteFile();
    if (! snr3::writeWav (wav, buf, sr, 0, count, proc.wavNorm))
        return;
    proc.exportPatch (patch);
    juce::StringArray files;
    files.add (wav.getFullPathName());
    if (patch.existsAsFile())
        files.add (patch.getFullPathName());
    performExternalDragDropOfFiles (files, false);
}
