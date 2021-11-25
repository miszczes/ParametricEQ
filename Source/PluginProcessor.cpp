/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define PI 3.14159265
float beta = 0;
float b0{ 0 }, b1{ 0 }, b2{ 0 }, a0{ 1.f }, a1{ 0 }, a2{ 0 };
std::atomic<bool> updateLS;
std::atomic<float>* LS_F;
std::atomic<float>* LS_G;
std::atomic<float>* LS_Q;

std::atomic<bool> updateBand1;
std::atomic<float>* B1_F0;
std::atomic<float>* B1_G;
std::atomic<float>* B1_BW;
std::atomic<float>* B1_GB;
std::atomic<float>* B1_G0;

std::atomic<bool> updateBand2;
std::atomic<float>* B2_F0;
std::atomic<float>* B2_G;
std::atomic<float>* B2_BW;
std::atomic<float>* B2_GB;
std::atomic<float>* B2_G0;

std::atomic<bool> updateBand3;
std::atomic<float>* B3_F0;
std::atomic<float>* B3_G;
std::atomic<float>* B3_BW;
std::atomic<float>* B3_GB;
std::atomic<float>* B3_G0;

std::atomic<bool> updateBand4;
std::atomic<float>* B4_F0;
std::atomic<float>* B4_G;
std::atomic<float>* B4_BW;
std::atomic<float>* B4_GB;
std::atomic<float>* B4_G0;

std::atomic<bool> updateHS;
std::atomic<float>* HS_F;
std::atomic<float>* HS_G;
std::atomic<float>* HS_Q;

//==============================================================================
ParametricEQAudioProcessor::ParametricEQAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    updateLS = true;

    LS_F = apvts.getRawParameterValue("LS Freq");
    LS_G = apvts.getRawParameterValue("LS Wzmocnienie");
    LS_Q = apvts.getRawParameterValue("LS Q");

    apvts.addParameterListener("LS Freq", this);
    apvts.addParameterListener("LS Wzmocnienie", this);
    apvts.addParameterListener("LS Q", this);

    updateBand1 = true;

    B1_F0 = apvts.getRawParameterValue("Band1 Freq");
    B1_G = apvts.getRawParameterValue("Band1 Wzmocnienie");
    B1_BW = apvts.getRawParameterValue("Band1 BW");
    B1_GB = apvts.getRawParameterValue("Band1 BW Gain");
    B1_G0 = apvts.getRawParameterValue("Band1 Reference");

    apvts.addParameterListener("Band1 Freq", this);
    apvts.addParameterListener("Band1 Wzmocnienie", this);
    apvts.addParameterListener("Band1 BW", this);
    apvts.addParameterListener("Band1 BW Gain", this);
    apvts.addParameterListener("Band1 Reference", this);

    updateBand2 = true;

    B2_F0 = apvts.getRawParameterValue("Band2 Freq");
    B2_G = apvts.getRawParameterValue("Band2 Wzmocnienie");
    B2_BW = apvts.getRawParameterValue("Band2 BW");
    B2_GB = apvts.getRawParameterValue("Band2 BW Gain");
    B2_G0 = apvts.getRawParameterValue("Band2 Reference");

    apvts.addParameterListener("Band2 Freq", this);
    apvts.addParameterListener("Band2 Wzmocnienie", this);
    apvts.addParameterListener("Band2 BW", this);
    apvts.addParameterListener("Band2 BW Gain", this);
    apvts.addParameterListener("Band2 Reference", this);

    updateBand3 = true;

    B3_F0 = apvts.getRawParameterValue("Band3 Freq");
    B3_G = apvts.getRawParameterValue("Band3 Wzmocnienie");
    B3_BW = apvts.getRawParameterValue("Band3 BW");
    B3_GB = apvts.getRawParameterValue("Band3 BW Gain");
    B3_G0 = apvts.getRawParameterValue("Band3 Reference");

    apvts.addParameterListener("Band3 Freq", this);
    apvts.addParameterListener("Band3 Wzmocnienie", this);
    apvts.addParameterListener("Band3 BW", this);
    apvts.addParameterListener("Band3 BW Gain", this);
    apvts.addParameterListener("Band3 Reference", this);

    updateBand4 = true;

    B4_F0 = apvts.getRawParameterValue("Band4 Freq");
    B4_G = apvts.getRawParameterValue("Band4 Wzmocnienie");
    B4_BW = apvts.getRawParameterValue("Band4 BW");
    B4_GB = apvts.getRawParameterValue("Band4 BW Gain");
    B4_G0 = apvts.getRawParameterValue("Band4 Reference");

    apvts.addParameterListener("Band4 Freq", this);
    apvts.addParameterListener("Band4 Wzmocnienie", this);
    apvts.addParameterListener("Band4 BW", this);
    apvts.addParameterListener("Band4 BW Gain", this);
    apvts.addParameterListener("Band4 Reference", this);

    updateHS = true;

    HS_F = apvts.getRawParameterValue("HS Freq");
    HS_G = apvts.getRawParameterValue("HS Wzmocnienie");
    HS_Q = apvts.getRawParameterValue("HS Q");

    apvts.addParameterListener("HS Freq", this);
    apvts.addParameterListener("HS Wzmocnienie", this);
    apvts.addParameterListener("HS Q", this);


}

ParametricEQAudioProcessor::~ParametricEQAudioProcessor()
{
    updateLS = false;

    apvts.removeParameterListener("LS Freq", this);
    apvts.removeParameterListener("LS Wzmocnienie", this);
    apvts.removeParameterListener("LS Q", this);

    updateBand1 = false;

    apvts.removeParameterListener("Band1 Freq", this);
    apvts.removeParameterListener("Band1 Wzmocnienie", this);
    apvts.removeParameterListener("Band1 BW", this);
    apvts.removeParameterListener("Band1 BW Gain", this);
    apvts.removeParameterListener("Band1 Reference", this);

    updateBand2 = false;

    apvts.removeParameterListener("Band2 Freq", this);
    apvts.removeParameterListener("Band2 Wzmocnienie", this);
    apvts.removeParameterListener("Band2 BW", this);
    apvts.removeParameterListener("Band2 BW Gain", this);
    apvts.removeParameterListener("Band2 Reference", this);

    updateBand3 = false;


    apvts.removeParameterListener("Band3 Freq", this);
    apvts.removeParameterListener("Band3 Wzmocnienie", this);
    apvts.removeParameterListener("Band3 BW", this);
    apvts.removeParameterListener("Band3 BW Gain", this);
    apvts.removeParameterListener("Band3 Reference", this);

    updateBand4 = false;


    apvts.removeParameterListener("Band4 Freq", this);
    apvts.removeParameterListener("Band4 Wzmocnienie", this);
    apvts.removeParameterListener("Band4 BW", this);
    apvts.removeParameterListener("Band4 BW Gain", this);
    apvts.removeParameterListener("Band4 Reference", this);

    updateHS = false;

    apvts.removeParameterListener("HS Freq", this);
    apvts.removeParameterListener("HS Wzmocnienie", this);
    apvts.removeParameterListener("HS Q", this);
}

//==============================================================================
const juce::String ParametricEQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ParametricEQAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ParametricEQAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ParametricEQAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ParametricEQAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ParametricEQAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ParametricEQAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ParametricEQAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ParametricEQAudioProcessor::getProgramName (int index)
{
    return {};
}

void ParametricEQAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================


void ParametricEQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    leftCh.reset();
    rightCh.reset();

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;

    leftCh.prepare(spec);
    rightCh.prepare(spec);


    updateFilters();


}

void ParametricEQAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ParametricEQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif


void ParametricEQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    //updateFilters();

    if (updateLS)
    {
        auto nastawy = zbierzNastawy(apvts);
        updateLowShelf(nastawy);
        updateLS = false;
    }
    if (updateBand1)
    {
        auto nastawy = zbierzNastawy(apvts);
        updatePeak1(nastawy);
        updateBand1 = false;
    }
    if (updateBand2)
    {
        auto nastawy = zbierzNastawy(apvts);
        updatePeak2(nastawy);
        updateBand2 = false;
    }
    if (updateBand3)
    {
        auto nastawy = zbierzNastawy(apvts);
        updatePeak3(nastawy);
        updateBand3 = false;
    }
    if (updateBand4)
    {
        auto nastawy = zbierzNastawy(apvts);
        updatePeak4(nastawy);
        updateBand4 = false;
    }
    if (updateHS)
    {
        auto nastawy = zbierzNastawy(apvts);
        updateHighShelf(nastawy);
        updateHS = false;
    }

    juce::dsp::AudioBlock<float> block(buffer);

    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftCh.process(leftContext);
    rightCh.process(rightContext);
}
void ParametricEQAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "LS Freq" ||
        parameterID == "LS Wzmocnienie" ||
        parameterID == "LS Q")
        updateLS = true;

    if (parameterID == "Band1 Freq" ||
        parameterID == "Band1 Wzmocnienie" ||
        parameterID == "Band1 BW" ||
        parameterID == "Band1 BW Gain" ||
        parameterID == "Band1 Reference")
        updateBand1 = true;

    if (parameterID == "Band2 Freq" ||
        parameterID == "Band2 Wzmocnienie" ||
        parameterID == "Band2 BW" ||
        parameterID == "Band2 BW Gain" ||
        parameterID == "Band2 Reference")
        updateBand2 = true;

    if (parameterID == "Band3 Freq" ||
        parameterID == "Band3 Wzmocnienie" ||
        parameterID == "Band3 BW" ||
        parameterID == "Band3 BW Gain" ||
        parameterID == "Band3 Reference")
        updateBand3 = true;

    if (parameterID == "Band4 Freq" ||
        parameterID == "Band4 Wzmocnienie" ||
        parameterID == "Band4 BW" ||
        parameterID == "Band4 BW Gain" ||
        parameterID == "Band4 Reference")
        updateBand4 = true;

    if (parameterID == "HS Freq" ||
        parameterID == "HS Wzmocnienie" ||
        parameterID == "HS Q")
        updateHS = true;


}

//==============================================================================
bool ParametricEQAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ParametricEQAudioProcessor::createEditor()
{
    return new ParametricEQAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void ParametricEQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void ParametricEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
    {
        apvts.replaceState(tree);
        updateFilters();
    }
}
Nastawy zbierzNastawy(juce::AudioProcessorValueTreeState& apvts)
{
    Nastawy ustawienia;

    ustawienia.Band1Freq = apvts.getRawParameterValue("Band1 Freq")->load();
    ustawienia.Band1GainToDB = apvts.getRawParameterValue("Band1 Wzmocnienie")->load();
    ustawienia.Band1BW = apvts.getRawParameterValue("Band1 BW")->load();
    ustawienia.Band1BWGain = apvts.getRawParameterValue("Band1 BW Gain")->load();
    ustawienia.Band1GainRef = apvts.getRawParameterValue("Band1 Reference")->load();

    ustawienia.Band2Freq = apvts.getRawParameterValue("Band2 Freq")->load();
    ustawienia.Band2GainToDB = apvts.getRawParameterValue("Band2 Wzmocnienie")->load();
    ustawienia.Band2BW = apvts.getRawParameterValue("Band2 BW")->load();
    ustawienia.Band2BWGain = apvts.getRawParameterValue("Band2 BW Gain")->load();
    ustawienia.Band2GainRef = apvts.getRawParameterValue("Band2 Reference")->load();

    ustawienia.Band3Freq = apvts.getRawParameterValue("Band3 Freq")->load();
    ustawienia.Band3GainToDB = apvts.getRawParameterValue("Band3 Wzmocnienie")->load();
    ustawienia.Band3BW = apvts.getRawParameterValue("Band3 BW")->load();
    ustawienia.Band3BWGain = apvts.getRawParameterValue("Band3 BW Gain")->load();
    ustawienia.Band3GainRef = apvts.getRawParameterValue("Band3 Reference")->load();

    ustawienia.Band4Freq = apvts.getRawParameterValue("Band4 Freq")->load();
    ustawienia.Band4GainToDB = apvts.getRawParameterValue("Band4 Wzmocnienie")->load();
    ustawienia.Band4BW = apvts.getRawParameterValue("Band4 BW")->load();
    ustawienia.Band4BWGain = apvts.getRawParameterValue("Band4 BW Gain")->load();
    ustawienia.Band4GainRef = apvts.getRawParameterValue("Band4 Reference")->load();

    ustawienia.LowShelfFreq = apvts.getRawParameterValue("LS Freq")->load();
    ustawienia.LowShelfGain = apvts.getRawParameterValue("LS Wzmocnienie")->load();
    ustawienia.LowShelfQ = apvts.getRawParameterValue("LS Q")->load();

    ustawienia.HighShelfFreq = apvts.getRawParameterValue("HS Freq")->load();
    ustawienia.HighShelfGain = apvts.getRawParameterValue("HS Wzmocnienie")->load();
    ustawienia.HighShelfQ = apvts.getRawParameterValue("HS Q")->load();


    return ustawienia;
}
void Wzory(float f0, float G, float BW, float BG, float G0, float sampleRate)
{

    beta = tan(BW / 2 * PI / (sampleRate / 2)) * sqrt(abs(pow(pow(10, BG / 20), 2) - pow(pow(10, G0 / 20), 2))) / sqrt(abs(pow(pow(10, G / 20), 2) - pow(pow(10, BG / 20), 2)));
    b0 = (pow(10, G0 / 20) + pow(10, G / 20) * beta) / (1 + beta);
    b1 = -2 * pow(10, G0 / 20) * cos(f0 * PI / (sampleRate / 2)) / (1 + beta);
    b2 = (pow(10, G0 / 20) - pow(10, G / 20) * beta) / (1 + beta);

    a0 = 1;
    a1 = -2 * cos(f0 * PI / (sampleRate / 2)) / (1 + beta);
    a2 = (1 - beta) / (1 + beta);
}

Wspolczynniki makeLowShelf(const Nastawy& nastawy, double sampleRate)
{
     return juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate,
                                                                nastawy.LowShelfFreq,
                                                                nastawy.LowShelfQ,
                                                                juce::Decibels::decibelsToGain(nastawy.LowShelfGain));
}

Wspolczynniki makePeakFilter1(const Nastawy& nastawy, double sampleRate)
{
    Wzory(nastawy.Band1Freq, nastawy.Band1GainToDB, nastawy.Band1BW, nastawy.Band1BWGain, nastawy.Band1GainRef, sampleRate);
    return new juce::dsp::IIR::Coefficients<float>(b0, b1, b2, a0, a1, a2);
}
Wspolczynniki makePeakFilter2(const Nastawy& nastawy, double sampleRate)
{
    Wzory(nastawy.Band2Freq, nastawy.Band2GainToDB, nastawy.Band2BW, nastawy.Band2BWGain, nastawy.Band2GainRef, sampleRate);
    return new juce::dsp::IIR::Coefficients<float>(b0, b1, b2, a0, a1, a2);
}
Wspolczynniki makePeakFilter3(const Nastawy& nastawy, double sampleRate)
{
    Wzory(nastawy.Band3Freq, nastawy.Band3GainToDB, nastawy.Band3BW, nastawy.Band3BWGain, nastawy.Band3GainRef, sampleRate);
    return new juce::dsp::IIR::Coefficients<float>(b0, b1, b2, a0, a1, a2);
}
Wspolczynniki makePeakFilter4(const Nastawy& nastawy, double sampleRate)
{
    Wzory(nastawy.Band4Freq, nastawy.Band4GainToDB, nastawy.Band4BW, nastawy.Band4BWGain, nastawy.Band4GainRef, sampleRate);
    return new juce::dsp::IIR::Coefficients<float>(b0, b1, b2, a0, a1, a2);
}

Wspolczynniki makeHighShelf(const Nastawy& nastawy, double sampleRate)
{
    return juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate,
                                                                nastawy.HighShelfFreq,
                                                                nastawy.HighShelfQ,
                                                                juce::Decibels::decibelsToGain(nastawy.HighShelfGain));
}
void ParametricEQAudioProcessor::updateLowShelf(const Nastawy& nastawy)
{
    auto LScoeffs = makeLowShelf(nastawy, getSampleRate());
    updateCoeffs(leftCh.get<0>().coefficients, LScoeffs);
    updateCoeffs(rightCh.get<0>().coefficients, LScoeffs);
}

void ParametricEQAudioProcessor::updatePeak1(const Nastawy& nastawy)
{
    auto& coeffs = makePeakFilter1(nastawy, getSampleRate());
    updateCoeffs(leftCh.get<1>().coefficients, coeffs);
    updateCoeffs(rightCh.get<1>().coefficients, coeffs);
}
void ParametricEQAudioProcessor::updatePeak2(const Nastawy& nastawy)
{
    auto& coeffs = makePeakFilter2(nastawy, getSampleRate());
    updateCoeffs(leftCh.get<2>().coefficients, coeffs);
    updateCoeffs(rightCh.get<2>().coefficients, coeffs);
}
void ParametricEQAudioProcessor::updatePeak3(const Nastawy& nastawy)
{
    auto& coeffs = makePeakFilter3(nastawy, getSampleRate());
    updateCoeffs(leftCh.get<3>().coefficients, coeffs);
    updateCoeffs(rightCh.get<3>().coefficients, coeffs);
}
void ParametricEQAudioProcessor::updatePeak4(const Nastawy& nastawy)
{
    auto& coeffs = makePeakFilter4(nastawy, getSampleRate());
    updateCoeffs(leftCh.get<4>().coefficients, coeffs);
    updateCoeffs(rightCh.get<4>().coefficients, coeffs);
}

void ParametricEQAudioProcessor::updateHighShelf(const Nastawy& nastawy)
{
    auto HScoeffs = makeHighShelf(nastawy, getSampleRate());
    updateCoeffs(leftCh.get<5>().coefficients, HScoeffs);
    updateCoeffs(rightCh.get<5>().coefficients, HScoeffs);
}


void updateCoeffs(Wspolczynniki& old, const Wspolczynniki& nowe)
{
    *old = *nowe;
}
void ParametricEQAudioProcessor::updateFilters()
{

    auto nastawy = zbierzNastawy(apvts);
    updatePeak1(nastawy);
    updatePeak2(nastawy);
    updatePeak3(nastawy);
    updatePeak4(nastawy);

    updateLowShelf(nastawy);
    updateHighShelf(nastawy);


}



juce::AudioProcessorValueTreeState::ParameterLayout
    ParametricEQAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>("Band1 Freq",
        "Band1 Freq",
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.1f),
        500.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Band1 Wzmocnienie",
        "Band1 Wzmocnienie",
        juce::NormalisableRange<float>(-24.f, 24.f, 0.1f, 1.f),
        0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Band1 BW",
        "Band1 BW",
        juce::NormalisableRange<float>(100.f, 1000.f, 0.5f, 1.f),
        100.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Band1 BW Gain",
        "Band1 BW Gain",
        juce::NormalisableRange<float>(0.5f, 12.f, 0.1f, 1.f),
        0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Band1 Reference",
        "Band1 Reference",
        juce::NormalisableRange<float>(-2.f, 2.f, 0.01f, 1.f),
        0.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Band2 Freq",
        "Band2 Freq",
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, .10f),
        1000.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Band2 Wzmocnienie",
        "Band2 Wzmocnienie",
        juce::NormalisableRange<float>(-24.f, 24.f, 0.1f, 1.f),
        0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Band2 BW",
        "Band2 BW",
        juce::NormalisableRange<float>(100.f, 1000.f, 0.5f, 1.f),
        100.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Band2 BW Gain",
        "Band2 BW Gain",
        juce::NormalisableRange<float>(0.5f, 12.f, 0.1f, 1.f),
        0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Band2 Reference",
        "Band2 Reference",
        juce::NormalisableRange<float>(-2.f, 2.f, 0.5f, 1.f),
        0.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Band3 Freq",
        "Band3 Freq",
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, .10f),
        3000.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Band3 Wzmocnienie",
        "Band3 Wzmocnienie",
        juce::NormalisableRange<float>(-24.f, 24.f, 0.1f, 1.f),
        0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Band3 BW",
        "Band3 BW",
        juce::NormalisableRange<float>(100.f, 1000.f, 0.5f, 1.f),
        100.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Band3 BW Gain",
        "Band3 BW Gain",
        juce::NormalisableRange<float>(0.5f, 12.f, 0.1f, 1.f),
        0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Band3 Reference",
        "Band3 Reference",
        juce::NormalisableRange<float>(-2.f, 2.f, 0.5f, 1.f),
        0.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Band4 Freq",
        "Band4 Freq",
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, .10f),
        6000.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Band4 Wzmocnienie",
        "Band4 Wzmocnienie",
        juce::NormalisableRange<float>(-24.f, 24.f, 0.1f, 1.f),
        0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Band4 BW",
        "Band4 BW",
        juce::NormalisableRange<float>(100.f, 1000.f, 0.5f, 1.f),
        100.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Band4 BW Gain",
        "Band4 BW Gain",
        juce::NormalisableRange<float>(0.5f, 12.f, 0.1f, 1.f),
        0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Band4 Reference",
        "Band4 Reference",
        juce::NormalisableRange<float>(-2.f, 2.f, 0.5f, 1.f),
        0.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("LS Freq",
        "LS Freq",
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, .10f),
        100.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LS Wzmocnienie",
        "LS Wzmocnienie",
        juce::NormalisableRange<float>(-24.f, 24.f, 0.1f, 1.f),
        0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LS Q",
        "LS Q",
        juce::NormalisableRange<float>(1.f, 10.f, 0.5f, 1.f),
        1.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("HS Freq",
        "HS Freq",
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, .10f),
        10000.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("HS Wzmocnienie",
        "HS Wzmocnienie",
        juce::NormalisableRange<float>(-24.f, 24.f, 0.1f, 1.f),
        0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("HS Q",
        "HS Q",
        juce::NormalisableRange<float>(1.f, 10.f, 0.5f, 1.f),
        1.f));

    return layout;
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ParametricEQAudioProcessor();
}
