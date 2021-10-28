/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ParametricEQAudioProcessorEditor::ParametricEQAudioProcessorEditor (ParametricEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    for (auto* elem : wstawElementy())
    {
        addAndMakeVisible(elem);
    }
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 400);
}

ParametricEQAudioProcessorEditor::~ParametricEQAudioProcessorEditor()
{
}

//==============================================================================
void ParametricEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
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
