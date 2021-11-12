/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


void LookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
    using namespace juce;

    auto bounds = Rectangle<float>(x, y, width, height);
        
    g.setColour(Colour(0u,0u,0u));
    g.fillEllipse(bounds);

    g.setColour(Colour(179u, 11u, 0u));
    g.drawEllipse(bounds, 5.f);

    auto center = bounds.getCentre();
    Path p;

    Rectangle<float> rect;
    rect.setLeft(center.getX() - 2);
    rect.setRight(center.getX() + 2);
    rect.setTop(bounds.getY());
    rect.setBottom(center.getY()-30);

    p.addRectangle(rect);

    jassert(rotaryStartAngle < rotaryEndAngle);

    auto sliderKatRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);

    p.applyTransform(AffineTransform().rotated(sliderKatRad, center.getX(), center.getY()));

    g.fillPath(p);
}
//==============================================================================
void WlasnyRotarySlider::paint(juce::Graphics& g)
{
    using namespace juce;
    
    auto startKat = degreesToRadians(180.f + 45.f);
    auto endKat = degreesToRadians(180.f - 45.f) + MathConstants<float>::twoPi;

    auto range = getRange();
    auto sliderBounds = getSliderBounds();

    getLookAndFeel().drawRotarySlider(g,    
                                        sliderBounds.getX(), 
                                        sliderBounds.getY(), 
                                        sliderBounds.getWidth(), 
                                        sliderBounds.getHeight(),
                                        jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0), 
                                        startKat, 
                                        endKat, 
                                        *this);
}

juce::Rectangle<int> WlasnyRotarySlider::getSliderBounds() const
{
    return getLocalBounds();
}

//==============================================================================
ParametricEQAudioProcessorEditor::ParametricEQAudioProcessorEditor (ParametricEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    Band1f0(*audioProcessor.apvts.getParameter("Band1 Freq"), "Hz"),
    Band1G(*audioProcessor.apvts.getParameter("Band1 Wzmocnienie"), "dB"),
    Band1BW(*audioProcessor.apvts.getParameter("Band1 BW"), "Hz"),
    Band1BG(*audioProcessor.apvts.getParameter("Band1 BW Gain"), "dB"),
    Band1G0(*audioProcessor.apvts.getParameter("Band1 Reference"), "dB"),

    Band2f0(*audioProcessor.apvts.getParameter("Band2 Freq"), "Hz"),
    Band2G(*audioProcessor.apvts.getParameter("Band2 Wzmocnienie"), "dB"),
    Band2BW(*audioProcessor.apvts.getParameter("Band2 BW"), "Hz"),
    Band2BG(*audioProcessor.apvts.getParameter("Band2 BW Gain"), "dB"),
    Band2G0(*audioProcessor.apvts.getParameter("Band2 Reference"), "dB"),

    Band3f0(*audioProcessor.apvts.getParameter("Band3 Freq"), "Hz"),
    Band3G(*audioProcessor.apvts.getParameter("Band3 Wzmocnienie"), "dB"),
    Band3BW(*audioProcessor.apvts.getParameter("Band3 BW"), "Hz"),
    Band3BG(*audioProcessor.apvts.getParameter("Band3 BW Gain"), "dB"),
    Band3G0(*audioProcessor.apvts.getParameter("Band3 Reference"), "dB"),

    Band4f0(*audioProcessor.apvts.getParameter("Band4 Freq"), "Hz"),
    Band4G(*audioProcessor.apvts.getParameter("Band4 Wzmocnienie"), "dB"),
    Band4BW(*audioProcessor.apvts.getParameter("Band4 BW"), "Hz"),
    Band4BG(*audioProcessor.apvts.getParameter("Band4 BW Gain"), "dB"),
    Band4G0(*audioProcessor.apvts.getParameter("Band4 Reference"), "dB"),

    LowShelfFreq(*audioProcessor.apvts.getParameter("LS Freq"), "Hz"),
    LowShelfQ(*audioProcessor.apvts.getParameter("LS Q"), ""),
    LowShelfG(*audioProcessor.apvts.getParameter("LS Wzmocnienie"), "dB"),


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

    g.drawImage(background, strefaCharakteryski.toFloat());

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
    using namespace juce;
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

    background = Image(Image::PixelFormat::RGB, getWidth(), getHeight(), true);
    Graphics g(background);

    Array<float> czest
    {
        20, 30, 40, 50, 100,
        200, 300, 400, 500, 1000,
        2000, 3000, 4000, 5000, 10000,
        20000
    };

    g.setColour(Colours::darkgrey);
    for (auto f : czest)
    {
        auto normX = mapFromLog10(f, 20.f, 20000.f);
        g.drawVerticalLine(getWidth() * normX, 0.f, getHeight());
    }

    Array<float> gain
    {
         -12, 0, 12
    };

    for (auto gl : gain)
    {
        auto y = jmap(gl, -24.f, 24.f, float(getHeight()), 0.f);
        g.drawHorizontalLine(y, 0.f, getWidth());
    }
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
