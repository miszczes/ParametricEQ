/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
auto red = juce::Colour(179u, 11u, 0u);

CharakterystykaAmplitudowa::CharakterystykaAmplitudowa(ParametricEQAudioProcessor& p) : audioProcessor(p)
{
    const auto& params = audioProcessor.getParameters();
    for (auto param : params)
    {
        param->addListener(this);
    }
    startTimer(60);

}
CharakterystykaAmplitudowa::~CharakterystykaAmplitudowa()
{
    const auto& params = audioProcessor.getParameters();
    for (auto param : params)
    {
        param->removeListener(this);
    }
}
void CharakterystykaAmplitudowa::parameterValueChanged(int parameterIndex, float newValue)
{
    parametryZmienione.set(true);
}
void CharakterystykaAmplitudowa::timerCallback()
{
    if (parametryZmienione.compareAndSetBool(false, true))
    {
        auto nastaw = zbierzNastawy(audioProcessor.apvts);

        auto peakCoeffs1 = makePeakFilter1(nastaw, audioProcessor.getSampleRate());
        auto peakCoeffs2 = makePeakFilter2(nastaw, audioProcessor.getSampleRate());
        auto peakCoeffs3 = makePeakFilter3(nastaw, audioProcessor.getSampleRate());
        auto peakCoeffs4 = makePeakFilter4(nastaw, audioProcessor.getSampleRate());
        updateCoeffs(monoChain.get<1 >().coefficients, peakCoeffs1);
        updateCoeffs(monoChain.get<2 >().coefficients, peakCoeffs2);
        updateCoeffs(monoChain.get<3 >().coefficients, peakCoeffs3);
        updateCoeffs(monoChain.get<4 >().coefficients, peakCoeffs4);

        auto LSCoeffs = makeLowShelf(nastaw, audioProcessor.getSampleRate());
        auto HSCoeffs = makeHighShelf(nastaw, audioProcessor.getSampleRate());

        updateCoeffs(monoChain.get<0>().coefficients, LSCoeffs);
        updateCoeffs(monoChain.get<5>().coefficients, HSCoeffs);


    }
    repaint();
}
void CharakterystykaAmplitudowa::paint(juce::Graphics& g)
{
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(Colours::black);

    g.drawImage(tlo, getLocalBounds().toFloat());

    auto strefaCharakteryski = strefaAnalizy();
    //strefaCharakteryski.removeFromTop(20);
    

    auto w = strefaCharakteryski.getWidth();

    auto& LowShelf = monoChain.get<0>();
    auto& band1 = monoChain.get<1>();
    auto& band2 = monoChain.get<2>();
    auto& band3 = monoChain.get<3>();
    auto& band4 = monoChain.get<4>();
    auto& HighShelf = monoChain.get<5>();

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
        if (!monoChain.isBypassed<5>())
            mag *= HighShelf.coefficients->getMagnitudeForFrequency(freq, sampleRate);


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
    g.drawRoundedRectangle(strefaRenderu().toFloat(), 4.f, 1.f);

    g.setColour(Colours::white);
    g.strokePath(charakterystyka, PathStrokeType(2.f));
   
}
void CharakterystykaAmplitudowa::resized()
{
   // startTimer(60);
    using namespace juce;
    tlo = Image(Image::PixelFormat::RGB, getWidth(), getHeight(), true);
    Graphics g(tlo);

    Array<float> czest
    {
        20, 30, 40, 50, 100,
        200, 300, 400, 500, 1000,
        2000, 3000, 4000, 5000, 10000,
        20000
    };

    auto Rend = strefaAnalizy();
    auto lewo = Rend.getX();
    auto prawo = Rend.getRight();
    auto gora = Rend.getY();
    auto dol = Rend.getBottom();
    auto width = Rend.getWidth();

    Array<float> cache;
    for (auto f : czest)
    {
        auto normaX = mapFromLog10(f, 20.f, 20000.f);
        cache.add(lewo + width * normaX);
    }

    g.setColour(Colours::darkgrey);
    for (auto x : cache)
    {
        g.drawVerticalLine(x, gora, dol);
    }

    Array<float> gain
    {
       -24.f, -18.f, -12.f, -6.f, 0.f, 6.f, 12.f, 18.f, 24.f
    };

    for (auto gl : gain)
    {
        auto y = jmap(gl, -24.f, 24.f, float(dol), float(gora));
  
        g.setColour(gl == 0.f ? red : Colours::darkgrey);
        g.drawHorizontalLine(y, lewo, prawo);

    }
    g.setColour(Colours::white);
    const int rozCzcion = 10;
    g.setFont(rozCzcion);

    for (int i = 0; i < czest.size(); ++i)
    {
        auto f = czest[i];
        auto x = cache[i];

        bool addK = false;
        String str;
        if (f > 999.f)
        {
            addK = true;
            f /= 1000.f;
        }
        str << f;
        if (addK)
            str << "k";
        str << "Hz";

        auto szerCzcion = g.getCurrentFont().getStringWidth(str);

        Rectangle<int> rect;
        rect.setSize(szerCzcion, rozCzcion);
        rect.setCentre(x, 0);
        rect.setY(1);

        g.drawFittedText(str, rect, juce::Justification::centred, 1);
    }
    //const int rozCzcion2 = 15;
    //g.setFont(rozCzcion2);
    for (auto gl : gain)
    {
        auto y = jmap(gl, -24.f, 24.f, float(dol), float(gora));

        String str;
        if (gl > 0.f)
            str << "+";
        str << gl;

        auto szerCzcion = g.getCurrentFont().getStringWidth(str);
        Rectangle<int> rect;
        rect.setSize(szerCzcion, rozCzcion);
        rect.setX(getWidth() - szerCzcion);
        rect.setCentre(rect.getCentreX(), y);
        g.setColour(gl == 0.f ? red : Colours::white);

        g.drawFittedText(str, rect, juce::Justification::centred, 1.f);

        str.clear();
        if (gl > 0.f)
            str << "+";
        str << gl;
        rect.setSize(szerCzcion, rozCzcion);
        rect.setX(1);
        
        g.setColour(gl == 0.f ? red : Colours::white);

        g.drawFittedText(str, rect, juce::Justification::centred, 1.f);


    }
 
}
juce::Rectangle<int> CharakterystykaAmplitudowa::strefaRenderu()
{
    auto bounds = getLocalBounds();

    bounds.removeFromTop(15);
    bounds.removeFromLeft(20);
    bounds.removeFromRight(20);
    bounds.removeFromBottom(5);

    return bounds;
}
juce::Rectangle<int> CharakterystykaAmplitudowa::strefaAnalizy()
{
    auto bounds = strefaRenderu();
    bounds.removeFromTop(4);
    bounds.removeFromBottom(4);

    return bounds;
}
void LookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
    using namespace juce;

    auto bounds = Rectangle<float>(x, y, width, height);
        
    g.setColour(Colour(0u,0u,0u));
    g.fillEllipse(bounds);

    g.setColour(red);
    g.drawEllipse(bounds, 5.f);

    if (auto* wrs = dynamic_cast<WlasnyRotarySlider*>(&slider))
    {
        auto center = bounds.getCentre();
        Path p1;
        Path p2;
        jassert(rotaryStartAngle < rotaryEndAngle);

        auto sliderKatRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);


        Rectangle<float> rect2;
        rect2.setLeft(center.getX() - 2);
        rect2.setRight(center.getX() + 4);
        rect2.setTop(bounds.getY());
        rect2.setBottom(center.getY() - 14);

        p2.addRoundedRectangle(rect2, 3.f);
        p2.applyTransform(AffineTransform().rotated(sliderKatRad, center.getX(), center.getY()));
        g.fillPath(p2);

        Rectangle<float> rect;
        rect.setLeft(center.getX() + 4);
        rect.setRight(center.getX() + 10);
        rect.setTop(bounds.getY() - 5);
        rect.setBottom(center.getY() - 14);
        p1.addRoundedRectangle(rect, 2.f);
        g.setColour(Colours::black);

        p1.applyTransform(AffineTransform().rotated(sliderKatRad, center.getX(), center.getY()));
        g.fillPath(p1);
        
        
        g.setFont(wrs->getTextHeight());
        auto text = wrs->getDisplayString();
        auto textWidth = g.getCurrentFont().getStringWidth(text);
        
        rect.setSize(textWidth + 4, wrs->getTextHeight() + 2);
        rect.setCentre(bounds.getCentre());


        //miejsce na zmiane koloru prostokatu w ktorym znajduje sie wartośc liczbowa

        g.setColour(Colours::white);
        g.drawFittedText(text, rect.toNearestInt(), juce::Justification::verticallyCentred, 1);

        Rectangle<float> rectLabel;
        auto textLabel = wrs->displayLabel();
        auto textWidthLabel = g.getCurrentFont().getStringWidth(textLabel);

        rectLabel.setSize(textWidthLabel + 4, wrs->getTextHeight() + 2);
        rectLabel.setCentre(bounds.getCentreX(), bounds.getCentreY() * 1.5);

        g.setColour(red);
        g.setFont(Font("Arial", 20, Font::bold));
        g.drawFittedText(textLabel, rectLabel.toNearestInt(), juce::Justification::verticallyCentred, 1);
    }



    


}
//==============================================================================
void WlasnyRotarySlider::paint(juce::Graphics& g)
{
    using namespace juce;
    
    auto startKat = degreesToRadians(180.f + 45.f);
    auto endKat = degreesToRadians(180.f - 45.f) + MathConstants<float>::twoPi;

    auto range = getRange();
    auto sliderBounds = getSliderBounds();

    //g.setColour(Colours::blue);
    //g.drawRect(getLocalBounds());
    //g.setColour(Colours::yellow);
    //g.drawRect(sliderBounds);

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
    auto bounds = getLocalBounds();

    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());

    size -= getTextHeight() * 2;
    juce::Rectangle<int> rect;
    rect.setSize(size, size);
    rect.setCentre(bounds.getCentreX(), 0);
    rect.setY(2);

    return rect;

}

juce::String WlasnyRotarySlider::getDisplayString() const
{
    //return juce::String(getValue());
    juce::String str;
    bool addK = false;

    if (auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param))
    {
        float value = getValue();
        if (value > 999.f)
        {
            value /= 1000.f;
            addK = true;
        }
        str = juce::String(value, (addK ? 2 : 0));
    }
    else
    {
        jassertfalse;
    }
    if (jednostka.isNotEmpty())
    {
        str << " ";
        if (addK)
            str << "k";
        str << jednostka;
    }
    return str;

}
juce::String WlasnyRotarySlider::displayLabel() const
{
    return label;
}
//==============================================================================
ParametricEQAudioProcessorEditor::ParametricEQAudioProcessorEditor (ParametricEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    Band1f0(*audioProcessor.apvts.getParameter("Band1 Freq"), "Hz", "f0"),
    Band1G(*audioProcessor.apvts.getParameter("Band1 Wzmocnienie"), "dB", "Gain"),
    Band1BW(*audioProcessor.apvts.getParameter("Band1 BW"), "Hz", "BW"),
    Band1BG(*audioProcessor.apvts.getParameter("Band1 BW Gain"), "dB", "GB"),
    Band1G0(*audioProcessor.apvts.getParameter("Band1 Reference"), "dB", "G0"),

    Band2f0(*audioProcessor.apvts.getParameter("Band2 Freq"), "Hz", "f0"),
    Band2G(*audioProcessor.apvts.getParameter("Band2 Wzmocnienie"), "dB", "Gain"),
    Band2BW(*audioProcessor.apvts.getParameter("Band2 BW"), "Hz", "BW"),
    Band2BG(*audioProcessor.apvts.getParameter("Band2 BW Gain"), "dB", "GB"),
    Band2G0(*audioProcessor.apvts.getParameter("Band2 Reference"), "dB", "G0"),

    Band3f0(*audioProcessor.apvts.getParameter("Band3 Freq"), "Hz", "f0"),
    Band3G(*audioProcessor.apvts.getParameter("Band3 Wzmocnienie"), "dB", "Gain"),
    Band3BW(*audioProcessor.apvts.getParameter("Band3 BW"), "Hz", "BW"),
    Band3BG(*audioProcessor.apvts.getParameter("Band3 BW Gain"), "dB", "GB"),
    Band3G0(*audioProcessor.apvts.getParameter("Band3 Reference"), "dB", "G0"),

    Band4f0(*audioProcessor.apvts.getParameter("Band4 Freq"), "Hz", "f0"),
    Band4G(*audioProcessor.apvts.getParameter("Band4 Wzmocnienie"), "dB", "Gain"),
    Band4BW(*audioProcessor.apvts.getParameter("Band4 BW"), "Hz", "BW"),
    Band4BG(*audioProcessor.apvts.getParameter("Band4 BW Gain"), "dB", "GB"),
    Band4G0(*audioProcessor.apvts.getParameter("Band4 Reference"), "dB", "G0"),

    LowShelfFreq(*audioProcessor.apvts.getParameter("LS Freq"), "Hz", "f"),
    LowShelfQ(*audioProcessor.apvts.getParameter("LS Q"), "", "Q"),
    LowShelfG(*audioProcessor.apvts.getParameter("LS Wzmocnienie"), "dB", "Gain"),

    HighShelfFreq(*audioProcessor.apvts.getParameter("HS Freq"), "Hz", "f"),
    HighShelfQ(*audioProcessor.apvts.getParameter("HS Q"), "", "Q"),
    HighShelfG(*audioProcessor.apvts.getParameter("HS Wzmocnienie"), "dB", "Gain"),

    charakterystykaAmplitudowa(audioProcessor),

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
    LowShelfGAttachment(audioProcessor.apvts, "LS Wzmocnienie", LowShelfG),

    HighShelfFreqAttachment(audioProcessor.apvts, "HS Freq", HighShelfFreq),
    HighShelfQAttachment(audioProcessor.apvts, "HS Q", HighShelfQ),
    HighShelfGAttachment(audioProcessor.apvts, "HS Wzmocnienie", HighShelfG)
{
    for (auto* elem : wstawElementy())
    {
        addAndMakeVisible(elem);
    }

    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1000, 800);
}

ParametricEQAudioProcessorEditor::~ParametricEQAudioProcessorEditor()
{
    
}

//==============================================================================
void ParametricEQAudioProcessorEditor::paint(juce::Graphics& g)
{
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(Colours::black);

    g.setFont(15);
    g.setColour(Colours::white);
    g.drawFittedText("Niski polkowy", LowShelfQ.getBounds(), juce::Justification::centredBottom, 1);
    g.drawFittedText("Parametryczny 1", Band1G0.getBounds(), juce::Justification::centredBottom, 1);
    g.drawFittedText("Parametryczny 2", Band2G0.getBounds(), juce::Justification::centredBottom, 1);
    g.drawFittedText("Parametryczny 3", Band3G0.getBounds(), juce::Justification::centredBottom, 1);
    g.drawFittedText("Parametryczny 4", Band4G0.getBounds(), juce::Justification::centredBottom, 1);
    g.drawFittedText("Wysoki polkowy", HighShelfQ.getBounds(), juce::Justification::centredBottom, 1);

}

void ParametricEQAudioProcessorEditor::resized()
{
    using namespace juce;
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();

    auto strefaCharakteryski = bounds.removeFromTop((bounds.getHeight() * 0.33));
    //strefaCharakteryski.removeFromTop(20);

    charakterystykaAmplitudowa.setBounds(strefaCharakteryski);

    //auto reference = bounds.removeFromLeft(bounds.getWidth() * 0.14);
    //Band1G0.setBounds(reference.removeFromBottom(bounds.getHeight() * 0.2));

    auto ls = bounds.removeFromLeft(bounds.getWidth() * 0.17);
    LowShelfFreq.setBounds(ls.removeFromTop(bounds.getHeight() * 0.33));
    LowShelfG.setBounds(ls.removeFromTop(bounds.getHeight() * 0.33));
    LowShelfQ.setBounds(ls.removeFromTop(bounds.getHeight() * 0.345));
    
    auto band1 = bounds.removeFromLeft(bounds.getWidth() * 0.20);
    Band1f0.setBounds(band1.removeFromTop(bounds.getHeight() * 0.2));
    Band1G.setBounds(band1.removeFromTop(bounds.getHeight() * 0.2));
    Band1BW.setBounds(band1.removeFromTop(bounds.getHeight() * 0.2));
    Band1BG.setBounds(band1.removeFromTop(bounds.getHeight() * 0.2));
    Band1G0.setBounds(band1.removeFromTop(bounds.getHeight() * 0.2));

    auto band2 = bounds.removeFromLeft(bounds.getWidth() * 0.25);
    Band2f0.setBounds(band2.removeFromTop(bounds.getHeight() * 0.2));
    Band2G.setBounds(band2.removeFromTop(bounds.getHeight() * 0.2));
    Band2BW.setBounds(band2.removeFromTop(bounds.getHeight() * 0.2));
    Band2BG.setBounds(band2.removeFromTop(bounds.getHeight() * 0.2));
    Band2G0.setBounds(band2.removeFromTop(bounds.getHeight() * 0.2));

    auto band3 = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    Band3f0.setBounds(band3.removeFromTop(bounds.getHeight() * 0.2));
    Band3G.setBounds(band3.removeFromTop(bounds.getHeight() * 0.2));
    Band3BW.setBounds(band3.removeFromTop(bounds.getHeight() * 0.2));
    Band3BG.setBounds(band3.removeFromTop(bounds.getHeight() * 0.2));
    Band3G0.setBounds(band3.removeFromTop(bounds.getHeight() * 0.2));

    //auto band2 = bounds.removeFromLeft(bounds.getWidth() * 0.50);
    //Band2f0.setBounds(band2.removeFromTop(bounds.getHeight() * 0.25));
    //Band2G.setBounds(band2.removeFromTop(bounds.getHeight() * 0.25));
    //Band2BW.setBounds(band2.removeFromTop(bounds.getHeight() * 0.25));
    //Band2BG.setBounds(band2.removeFromTop(bounds.getHeight() * 0.25));
    
    auto band4 = bounds.removeFromLeft(bounds.getWidth() * 0.5);
    Band4f0.setBounds(band4.removeFromTop(bounds.getHeight() * 0.2));
    Band4G.setBounds(band4.removeFromTop(bounds.getHeight() * 0.2));
    Band4BW.setBounds(band4.removeFromTop(bounds.getHeight() * 0.2));
    Band4BG.setBounds(band4.removeFromTop(bounds.getHeight() * 0.2));
    Band4G0.setBounds(band4.removeFromTop(bounds.getHeight() * 0.2));

    HighShelfFreq.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.33));
    HighShelfG.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.5));
    HighShelfQ.setBounds(bounds.removeFromTop(bounds.getHeight()));
   


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
        &LowShelfG,

        &HighShelfFreq,
        &HighShelfQ,
        &HighShelfG,

        &charakterystykaAmplitudowa
    };
}
