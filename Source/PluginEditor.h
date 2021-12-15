/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//inicjalizacja wszystkich komponentow wykorzystanych podczas rysowania GUI

struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider(juce::Graphics&,
        int x, int y, int width, int height,
        float sliderPosProportional,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider&) override;
};

struct WlasnyRotarySlider : juce::Slider //inicjalizacja wszelkich metod istntych dla rysowania wlasnych rotary sliderow
{
    WlasnyRotarySlider(juce::RangedAudioParameter &rap, const juce::String& unit, const juce::String& sliderLabel) : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                                                                    juce::Slider::TextEntryBoxPosition::NoTextBox),
        param(&rap),
        jednostka(unit),
        label(sliderLabel)
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

    juce::String displayLabel() const;
private:
    LookAndFeel lnf;

    juce::RangedAudioParameter* param;
    juce::String jednostka;
    juce::String label;
};

struct CharakterystykaAmplitudowa : juce::Component, juce::AudioProcessorParameter::Listener, juce::Timer //inicjalizacja wszystkich metod istotnych z punktu widzenia rysowania charakterystki amplitudowej
{
    CharakterystykaAmplitudowa(ParametricEQAudioProcessor&);
    ~CharakterystykaAmplitudowa();

    void parameterValueChanged(int parameterIndex, float newValue) override;

   
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {}

    void timerCallback() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    ParametricEQAudioProcessor& audioProcessor;
    juce::Atomic<bool> parametryZmienione{ false };
    Mono monoChain;                                             // wlasna instancja lancucha filtrow potrzebna by na podstawie jej transmitancji wyznaczyc charatkerystyke amplitudowa 
    juce::Image tlo;

    juce::Rectangle<int> strefaRenderu();
    juce::Rectangle<int> strefaAnalizy();
    
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
                        LowShelfG,
                        HighShelfFreq,
                        HighShelfQ,
                        HighShelfG;

    CharakterystykaAmplitudowa charakterystykaAmplitudowa;

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
                LowShelfGAttachment,
                HighShelfFreqAttachment,
                HighShelfQAttachment,
                HighShelfGAttachment;

    std::vector<juce::Component*> wstawElementy();

    
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParametricEQAudioProcessorEditor)
};
