/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ParametricEQAudioProcessorEditor::ParametricEQAudioProcessorEditor (ParametricEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    Band1f0Attachment(audioProcessor.apvts, "Band1 Freq",Band1f0),
    Band1GAttachment(audioProcessor.apvts, "Band1 Wzmocnienie", Band1G),
    Band1BWAttachment(audioProcessor.apvts, "Band1 BW", Band1BW),
    Band1BGAttachment(audioProcessor.apvts, "Band1 BW Gain", Band1BG),
    Band1G0Attachment(audioProcessor.apvts, "Band1 Reference", Band1G0),

    Band2f0Attachment(audioProcessor.apvts, "Band2 Freq", Band2f0),
    Band2GAttachment(audioProcessor.apvts, "Band2 Wzmocnienie", Band2G),
    Band2BWAttachment(audioProcessor.apvts, "Band2 BW", Band2BW),
    Band2BGAttachment(audioProcessor.apvts, "Band2 BW Gain", Band2BG),
    Band2G0Attachment(audioProcessor.apvts, "Band2 Reference", Band2G0)
{
    for (auto* elem : wstawElementy())
    {
        addAndMakeVisible(elem);
    }

    const auto& params = audioProcessor.getParameters();
    for (auto param : params) 
    {
        param->addListener(this);
    }
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 400);
}

ParametricEQAudioProcessorEditor::~ParametricEQAudioProcessorEditor()
{
    const auto& params = audioProcessor.getParameters();
    for (auto param : params)
    {
        param->removeListener(this);
    }
}

//==============================================================================
void ParametricEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (Colours::black);

    auto bounds = getLocalBounds();
    auto strefaCharakteryski = bounds.removeFromTop(bounds.getHeight() * 0.33);

    auto w = strefaCharakteryski.getWidth();

    auto& band1 = monoChain.get<0>();
    auto& band2 = monoChain.get<1>();

    auto sampleRate = audioProcessor.getSampleRate();

    std::vector<double> magnitudes;

    magnitudes.resize(w);

    for (int i = 0; i < w; i++)
    {
        double mag = 1.f;
        auto freq = mapToLog10(double(i) / double(w), 20.0, 20000.0);

        if (!monoChain.isBypassed<0>())
            mag *= band1.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!monoChain.isBypassed<1>())
            mag *= band2.coefficients->getMagnitudeForFrequency(freq, sampleRate);

        magnitudes[i] = Decibels::gainToDecibels(mag);

    }

    Path charakterystyka;

    const double outMin = strefaCharakteryski.getBottom();
    const double outMax = strefaCharakteryski.getY();

    auto map = [outMin, outMax](double input)
    {
        return jmap(input, -24.0, 24.0, outMin, outMax);
    };

    charakterystyka.startNewSubPath(strefaCharakteryski.getX(), map(magnitudes.front()));

    for (size_t i = 1; i < magnitudes.size(); i++)
    {
        charakterystyka.lineTo(strefaCharakteryski.getX() + i, map(magnitudes[i]));
    }
    g.setColour(Colours::orange);
    g.drawRoundedRectangle(strefaCharakteryski.toFloat(), 4.f, 1.f);

    g.setColour(Colours::white);
    g.strokePath(charakterystyka, PathStrokeType(2.f));
}

void ParametricEQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();
    auto strefaCharakteryski = bounds.removeFromTop(bounds.getHeight() * 0.33);

    auto band1 = bounds.removeFromLeft(bounds.getWidth() * 0.5);
    Band1f0.setBounds(band1.removeFromTop(bounds.getHeight() * 0.2));
    Band1G.setBounds(band1.removeFromTop(bounds.getHeight() * 0.2));
    Band1BW.setBounds(band1.removeFromTop(bounds.getHeight() * 0.2));
    Band1BG.setBounds(band1.removeFromTop(bounds.getHeight() * 0.2));
    Band1G0.setBounds(band1.removeFromTop(bounds.getHeight() * 0.2));

    Band2f0.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.2));
    Band2G.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.25));
    Band2BW.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.33));
    Band2BG.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.5));
    Band2G0.setBounds(bounds.removeFromTop(bounds.getHeight()));
}
    
void ParametricEQAudioProcessorEditor::parameterValueChanged(int parameterIndex, float newValue)
{
    parametryZmienione.set(true);
}
void ParametricEQAudioProcessorEditor::timerCallback()
{
    if (parametryZmienione.compareAndSetBool(false, true))
    {
        auto chainSettings = getChainSettings(audioProcessor.apvts);
        
    }
}

std::vector<juce::Component*> ParametricEQAudioProcessorEditor::wstawElementy()
{
    return
    {
        &Band1f0,
        &Band1BW,
        &Band1BG,
        &Band1G0,
        &Band1G,
        &Band2f0,
        &Band2BW,
        &Band2BG,
        &Band2G0,
        &Band2G
    };
}
