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
    Band2G0Attachment(audioProcessor.apvts, "Band2 Reference", Band2G0),

    Band3f0Attachment(audioProcessor.apvts, "Band3 Freq", Band3f0),
    Band3GAttachment(audioProcessor.apvts, "Band3 Wzmocnienie", Band3G),
    Band3BWAttachment(audioProcessor.apvts, "Band3 BW", Band3BW),
    Band3BGAttachment(audioProcessor.apvts, "Band3 BW Gain", Band3BG),
    Band3G0Attachment(audioProcessor.apvts, "Band3 Reference", Band3G0),

    Band4f0Attachment(audioProcessor.apvts, "Band4 Freq", Band4f0),
    Band4GAttachment(audioProcessor.apvts, "Band4 Wzmocnienie", Band4G),
    Band4BWAttachment(audioProcessor.apvts, "Band4 BW", Band4BW),
    Band4BGAttachment(audioProcessor.apvts, "Band4 BW Gain", Band4BG),
    Band4G0Attachment(audioProcessor.apvts, "Band4 Reference", Band4G0),

    LowShelfFreqAttachment(audioProcessor.apvts, "LS Freq", LowShelfFreq),
    LowShelfQAttachment(audioProcessor.apvts, "LS Q", LowShelfQ),
    LowShelfGAttachment(audioProcessor.apvts, "LS Wzmocnienie", LowShelfG)
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
    startTimer(60);
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1000, 800);
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

    auto& LowShelf = monoChain.get<0>();
    auto& band1 = monoChain.get<1>();
    auto& band2 = monoChain.get<2>();
    auto& band3 = monoChain.get<3>();
    auto& band4 = monoChain.get<4>();

    auto sampleRate = audioProcessor.getSampleRate();

    std::vector<double> magnitudes;

    magnitudes.resize(w);

    for (int i = 0; i < w; i++)
    {
        double mag = 1.f;
        auto freq = mapToLog10(double(i) / double(w), 20.0, 20000.0);

        if (!monoChain.isBypassed<0>())
            mag *= LowShelf.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!monoChain.isBypassed<1>())
            mag *= band1.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!monoChain.isBypassed<2>())
            mag *= band2.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!monoChain.isBypassed<3>())
            mag *= band3.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!monoChain.isBypassed<4>())
            mag *= band4.coefficients->getMagnitudeForFrequency(freq, sampleRate);
            

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
    g.setColour(Colours::aqua);
    g.drawRoundedRectangle(strefaCharakteryski.toFloat(), 4.f, 1.f);

    g.setColour(Colours::white);
    g.strokePath(charakterystyka, PathStrokeType(2.f));
    repaint();
}

void ParametricEQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();
    auto strefaCharakteryski = bounds.removeFromTop(bounds.getHeight() * 0.33);

    auto reference = bounds.removeFromLeft(bounds.getWidth() * 0.17);
    Band1G0.setBounds(reference.removeFromBottom(bounds.getHeight() * 0.2));

    auto ls = bounds.removeFromLeft(bounds.getWidth() * 0.20);
    LowShelfFreq.setBounds(ls.removeFromTop(bounds.getHeight() * 0.33));
    LowShelfG.setBounds(ls.removeFromTop(bounds.getHeight() * 0.33));
    LowShelfQ.setBounds(ls.removeFromTop(bounds.getHeight() * 0.33));
    
    auto band1 = bounds.removeFromLeft(bounds.getWidth() * 0.25);
    Band1f0.setBounds(band1.removeFromTop(bounds.getHeight() * 0.25));
    Band1G.setBounds(band1.removeFromTop(bounds.getHeight() * 0.25));
    Band1BW.setBounds(band1.removeFromTop(bounds.getHeight() * 0.25));
    Band1BG.setBounds(band1.removeFromTop(bounds.getHeight() * 0.25));
    //Band1G0.setBounds(band1.removeFromTop(bounds.getHeight() * 0.2));

    auto band2 = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    Band2f0.setBounds(band2.removeFromTop(bounds.getHeight() * 0.25));
    Band2G.setBounds(band2.removeFromTop(bounds.getHeight() * 0.25));
    Band2BW.setBounds(band2.removeFromTop(bounds.getHeight() * 0.25));
    Band2BG.setBounds(band2.removeFromTop(bounds.getHeight() * 0.25));
   // Band2G0.setBounds(band2.removeFromTop(bounds.getHeight() * 0.2));

    auto band3 = bounds.removeFromLeft(bounds.getWidth() * 0.50);
    Band3f0.setBounds(band3.removeFromTop(bounds.getHeight() * 0.25));
    Band3G.setBounds(band3.removeFromTop(bounds.getHeight() * 0.25));
    Band3BW.setBounds(band3.removeFromTop(bounds.getHeight() * 0.25));
    Band3BG.setBounds(band3.removeFromTop(bounds.getHeight() * 0.25));
    //Band3G0.setBounds(band3.removeFromTop(bounds.getHeight() * 0.2));

    //auto band2 = bounds.removeFromLeft(bounds.getWidth() * 0.50);
    //Band2f0.setBounds(band2.removeFromTop(bounds.getHeight() * 0.25));
    //Band2G.setBounds(band2.removeFromTop(bounds.getHeight() * 0.25));
    //Band2BW.setBounds(band2.removeFromTop(bounds.getHeight() * 0.25));
    //Band2BG.setBounds(band2.removeFromTop(bounds.getHeight() * 0.25));

    Band4f0.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.25));
    Band4G.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.33));
    Band4BW.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.5));
    Band4BG.setBounds(bounds.removeFromTop(bounds.getHeight()));
   // Band4G0.setBounds(bounds.removeFromTop(bounds.getHeight()));
}
    
void ParametricEQAudioProcessorEditor::parameterValueChanged(int parameterIndex, float newValue)
{
    parametryZmienione.set(true);
}
void ParametricEQAudioProcessorEditor::timerCallback()
{
    if (parametryZmienione.compareAndSetBool(false, true))
    {
        auto nastaw = zbierzNastawy(audioProcessor.apvts);
        for (size_t i = 0; i < 4; i++)
        {
            if (i == 0) {
                auto peakCoeffs = makePeakFilter(nastaw, audioProcessor.getSampleRate(), i);
                updateCoeffs(monoChain.get<1 >().coefficients, peakCoeffs);
            }   
            else if (i == 1) {
                auto peakCoeffs = makePeakFilter(nastaw, audioProcessor.getSampleRate(), i);
                updateCoeffs(monoChain.get<2 >().coefficients, peakCoeffs);
            }
            else if (i == 2) {
                auto peakCoeffs = makePeakFilter(nastaw, audioProcessor.getSampleRate(), i);
                updateCoeffs(monoChain.get<3 >().coefficients, peakCoeffs);
            }
            else if (i == 3) {
                auto peakCoeffs = makePeakFilter(nastaw, audioProcessor.getSampleRate(), i);
                updateCoeffs(monoChain.get<4 >().coefficients, peakCoeffs);
            }

            auto peakCoeffs = makeLowShelf(nastaw, audioProcessor.getSampleRate());
            updateCoeffs(monoChain.get<0>().coefficients, peakCoeffs);

        }
        repaint();
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
        &Band2G,
        &Band3f0,
        &Band3BW,
        &Band3BG,
        &Band3G0,
        &Band3G,
        &Band4f0,
        &Band4BW,
        &Band4BG,
        &Band4G0,
        &Band4G,

        &LowShelfFreq,
        &LowShelfQ,
        &LowShelfG
    };
}
