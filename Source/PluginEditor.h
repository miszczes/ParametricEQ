/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

struct WlasnyRotarySlider : juce::Slider
{
    WlasnyRotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                        juce::Slider::TextEntryBoxPosition::NoTextBox)
    {

    }
};

//==============================================================================
/**
*/
class ParametricEQAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ParametricEQAudioProcessorEditor (ParametricEQAudioProcessor&);
    ~ParametricEQAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ParametricEQAudioProcessor& audioProcessor;

    WlasnyRotarySlider Band1f0,
                        Band1BW,
                        Band1BG,
                        Band1G0,
                        Band1G, 
                        Band2f0,
                        Band2BW,
                        Band2BG,
                        Band2G0,
                        Band2G;

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
                Band2GAttachment;

    std::vector<juce::Component*> wstawElementy();

    Mono monoChain;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParametricEQAudioProcessorEditor)
};
