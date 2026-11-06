#pragma once

#include "../PluginProcessor.h"

#include <JuceHeader.h>

// Vertical strips for Shell / Wires / Stick / Sample. Fader, meter, mute, solo, phase, send.
class LayerMix : public juce::Component
{
public:
    explicit LayerMix (SNR3AudioProcessor& p) : proc (p)
    {
        static const char* names[] = { "Shell", "Wires", "Stick", "Sample" };
        static const juce::Colour cols[] = {
            juce::Colour (0xffc4a35a), juce::Colour (0xffe8dcc8),
            juce::Colour (0xff8a3a22), juce::Colour (0xff66d4ff)
        };
        for (int i = 0; i < ParamID::layerCount; ++i)
            addStrip (i, names[i], cols[i]);
    }

    void refresh()
    {
        for (auto* s : strips)
            s->tick();
    }

    SNR3AudioProcessor& proc;

private:
    struct Strip : public juce::Component
    {
        Strip (SNR3AudioProcessor& p, int layer, const char* title, juce::Colour col)
            : proc (p), layer (layer), accent (col)
        {
            struct LnF : public juce::LookAndFeel_V4
            {
                void drawLinearSlider (juce::Graphics&, int, int, int, int, float, float, float,
                                       juce::Slider::SliderStyle, juce::Slider&) override {}
            };
            lnf = std::make_unique<LnF>();
            fader.setSliderStyle (juce::Slider::LinearVertical);
            fader.setTextBoxStyle (juce::Slider::NoTextBox, true, 0, 0);
            fader.setLookAndFeel (lnf.get());
            if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*> (proc.apvts.getParameter (ParamID::lyGain[layer])))
                fader.setDoubleClickReturnValue (true, ranged->convertFrom0to1 (ranged->getDefaultValue()));
            fader.onValueChange = [this] { repaint(); };
            addAndMakeVisible (fader);
            gainAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                proc.apvts, ParamID::lyGain[layer], fader);

            auto setup = [this] (juce::ToggleButton& b, const char* text, const char* id,
                                 std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>& att)
            {
                b.setButtonText (text);
                addAndMakeVisible (b);
                att = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (proc.apvts, id, b);
            };
            setup (mute, "M", ParamID::lyMute[layer], muteAtt);
            setup (solo, "S", ParamID::lySolo[layer], soloAtt);
            setup (phase, "P", ParamID::lyFlip[layer], phaseAtt);

            send.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
            send.setTextBoxStyle (juce::Slider::NoTextBox, true, 0, 0);
            send.setPopupDisplayEnabled (true, false, this, 400);
            if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*> (proc.apvts.getParameter (ParamID::lySend[layer])))
                send.setDoubleClickReturnValue (true, ranged->convertFrom0to1 (ranged->getDefaultValue()));
            addAndMakeVisible (send);
            sendAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                proc.apvts, ParamID::lySend[layer], send);

            name.setText (title, juce::dontSendNotification);
            name.setJustificationType (juce::Justification::centred);
            name.setColour (juce::Label::textColourId, juce::Colours::white);
            name.setInterceptsMouseClicks (false, false);
            addAndMakeVisible (name);
        }

        ~Strip() override { fader.setLookAndFeel (nullptr); }

        void tick()
        {
            const float pk = proc.layerMeter (layer).lastPeak();
            if (std::abs (pk - shown) < 0.002f)
                return;
            shown = pk;
            repaint();
        }

        void resized() override
        {
            auto r = getLocalBounds().reduced (4, 4);
            name.setBounds (r.removeFromBottom (16));
            auto row = r.removeFromBottom (22);
            const int w = row.getWidth() / 3;
            mute.setBounds (row.removeFromLeft (w).reduced (1, 1));
            solo.setBounds (row.removeFromLeft (w).reduced (1, 1));
            phase.setBounds (row.reduced (1, 1));
            send.setBounds (r.removeFromBottom (36).reduced (6, 0));
            fader.setBounds (r.reduced (8, 2));
        }

        void paint (juce::Graphics& g) override
        {
            auto body = getLocalBounds().toFloat().reduced (2.f);
            g.setColour (juce::Colour (0xff12100c));
            g.fillRoundedRectangle (body, 4.f);
            g.setColour (accent.withAlpha (0.45f));
            g.drawRoundedRectangle (body.reduced (0.5f), 4.f, 1.f);

            auto area = fader.getBounds().toFloat().reduced (0.f, 4.f);
            auto meter = area.withSizeKeepingCentre (8.f, area.getHeight());
            g.setColour (juce::Colours::black);
            g.fillRoundedRectangle (meter, 2.f);
            const float pk = juce::jlimit (0.f, 1.f, shown);
            if (pk > 0.001f)
            {
                auto fill = meter.removeFromBottom (juce::jmax (2.f, meter.getHeight() * pk));
                g.setColour (shown >= 1.f ? juce::Colour (0xffff00ff) : accent);
                g.fillRoundedRectangle (fill, 2.f);
            }

            const float pos = (float) fader.valueToProportionOfLength (fader.getValue());
            const float hy = area.getBottom() - pos * area.getHeight();
            auto handle = juce::Rectangle<float> (area.getCentreX() - 16.f, hy - 6.f, 32.f, 12.f);
            g.setColour (juce::Colour (0xff2a241c));
            g.fillRoundedRectangle (handle, 2.f);
            g.setColour (accent);
            g.drawRoundedRectangle (handle.reduced (0.5f), 2.f, 1.4f);
        }

        SNR3AudioProcessor& proc;
        int layer = 0;
        float shown = 0.f;
        juce::Colour accent;
        std::unique_ptr<juce::LookAndFeel_V4> lnf;
        juce::Slider fader, send;
        juce::ToggleButton mute, solo, phase;
        juce::Label name;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAtt, sendAtt;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> muteAtt, soloAtt, phaseAtt;
    };

    void addStrip (int layer, const char* title, juce::Colour col)
    {
        auto* s = strips.add (new Strip (proc, layer, title, col));
        addAndMakeVisible (s);
    }

    void resized() override
    {
        auto r = getLocalBounds();
        if (strips.isEmpty())
            return;
        const int w = juce::jmax (1, r.getWidth() / strips.size());
        for (int i = 0; i < strips.size(); ++i)
            strips[i]->setBounds (r.removeFromLeft (i == strips.size() - 1 ? r.getWidth() : w).reduced (4, 2));
    }

    juce::OwnedArray<Strip> strips;
};
