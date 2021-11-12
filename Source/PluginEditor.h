/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider(juce::Graphics&,
        int x, int y, int width, int height,
        float sliderPosProportional,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider&) override;
};

struct WlasnyRotarySlider : juce::Slider
{
    WlasnyRotarySlider(juce::RangedAudioParameter &rap, const juce::String& unit) : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                                                                    juce::Slider::TextEntryBoxPosition::NoTextBox),
        param(&rap),
        jednostka(unit)
    {
        setLookAndFeel(&lnf);
    }

    ~WlasnyRotarySlider()
    {
        setLookAndFeel(nullptr);
    }
    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 14; }
    juce::String getDisplayString() const;
private:
    LookAndFeel lnf;

    juce::RangedAudioParameter* param;
    juce::String jednostka;
};

//==============================================================================
/**
*/
class ParametricEQAudioProcessorEditor  : public juce::AudioProcessorEditor,
    juce::AudioProcessorParameter::Listener,
    juce::Timer
{
public:
    ParametricEQAudioProcessorEditor (ParametricEQAudioProcessor&);
    ~ParametricEQAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void parameterValueChanged(int parameterIndex, float newValue) override;

    /** Indicates that a parameter change gesture has started.

        E.g. if the user is dragging a slider, this would be called with gestureIsStarting
        being true when they first press the mouse button, and it will be called again with
        gestureIsStarting being false when they release it.

        IMPORTANT NOTE: This will be called synchronously, and many audio processors will
        call it during their audio callback. This means that not only has your handler code
        got to be completely thread-safe, but it's also got to be VERY fast, and avoid
        blocking. If you need to handle this event on your message thread, use this callback
        to trigger an AsyncUpdater or ChangeBroadcaster which you can respond to later on the
        message thread.
    */
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {}

    void timerCallback() override;
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ParametricEQAudioProcessor& audioProcessor;

    juce::Atomic<bool> parametryZmienione{ false };

    WlasnyRotarySlider Band1f0,
                        Band1BW,
                        Band1BG,
                        Band1G0,
                        Band1G,
                        Band2f0,
                        Band2BW,
                        Band2BG,
                        Band2G0,
                        Band2G,
                        Band3f0,
                        Band3BW,
                        Band3BG,
                        Band3G0,
                        Band3G,
                        Band4f0,
                        Band4BW,
                        Band4BG,
                        Band4G0,
                        Band4G,
                        LowShelfFreq,
                        LowShelfQ,
                        LowShelfG;

    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;

    Attachment Band1f0Attachment,
                Band1BWAttachment,
                Band1BGAttachment,
                Band1G0Attachment,
                Band1GAttachment,
                Band2f0Attachment,
                Band2BWAttachment,
                Band2BGAttachment,
                Band2G0Attachment,
                Band2GAttachment,
                Band3f0Attachment,
                Band3BWAttachment,
                Band3BGAttachment,
                Band3G0Attachment,
                Band3GAttachment,
                Band4f0Attachment,
                Band4BWAttachment,
                Band4BGAttachment,
                Band4G0Attachment,
                Band4GAttachment,
                LowShelfFreqAttachment,
                LowShelfQAttachment,
                LowShelfGAttachment;

    std::vector<juce::Component*> wstawElementy();

    Mono monoChain;
    
    juce::Image background;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParametricEQAudioProcessorEditor)
};
