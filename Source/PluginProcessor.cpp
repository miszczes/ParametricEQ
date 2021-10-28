/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define PI 3.14159265

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
}

ParametricEQAudioProcessor::~ParametricEQAudioProcessor()
{
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
void ParametricEQAudioProcessor::generujWspolczynniki(double fs, float f0, float G0, float Bf, float GB, float G, const size_t index)
{
    if (fs > 0) {

        beta = tan(Bf / 2 * PI / (fs / 2)) * sqrt(abs(pow(pow(10, GB / 20), 2) - pow(pow(10, G0 / 20), 2))) / sqrt(abs(pow(pow(10, G / 20), 2) - pow(pow(10, GB / 20), 2)));
        b0 = (pow(10, G0 / 20) + pow(10, G / 20) * beta) / (1 + beta);
        b1 = -2 * pow(10, G0 / 20) * cos(f0 * PI / (fs / 2)) / (1 + beta);
        b2 = (pow(10, G0 / 20) - pow(10, G / 20) * beta) / (1 + beta);

        a0 = 1;
        a1 = -2 * cos(f0 * PI / (fs / 2)) / (1 + beta);
        a2 = (1 - beta) / (1 + beta);


    }

    juce::dsp::IIR::Coefficients<float>::Ptr coeffs(new juce::dsp::IIR::Coefficients<float>(b0, b1, b2, a0, a1, a2));
    if (coeffs)
    {
        if (index == 0)
        {
            *leftCh.get<0>().coefficients = *coeffs;
            *rightCh.get<0>().coefficients = *coeffs;
        }
        else if (index == 1)
        {
            *leftCh.get<1>().coefficients = *coeffs;
            *rightCh.get<1>().coefficients = *coeffs;
        }
    }
}
void ParametricEQAudioProcessor::updateFilters()
{

    auto chainSettings = getChainSettings(apvts);

    for (size_t i = 0; i < 2; i++) {
        if (i == 0)
            generujWspolczynniki(getSampleRate(), chainSettings.Band1Freq, chainSettings.Band1GainRef, chainSettings.Band1BW, chainSettings.Band1BWGain, chainSettings.Band1GainToDB, i);
        else if (i == 1)
            generujWspolczynniki(getSampleRate(), chainSettings.Band2Freq, chainSettings.Band2GainRef, chainSettings.Band2BW, chainSettings.Band2BWGain, chainSettings.Band2GainToDB, i);
    }

}

void ParametricEQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
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

    updateFilters();

    juce::dsp::AudioBlock<float> block(buffer);

    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftCh.process(leftContext);
    rightCh.process(rightContext);
}

//==============================================================================
bool ParametricEQAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ParametricEQAudioProcessor::createEditor()
{
    //return new ParametricEQAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void ParametricEQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ParametricEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}
ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
    ChainSettings ustawienia;

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


    return ustawienia;
}

juce::AudioProcessorValueTreeState::ParameterLayout
    ParametricEQAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>("Band1 Freq",
        "Band1 Freq",
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, .25f),
        100.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Band1 Wzmocnienie",
        "Band1 Wzmocnienie",
        juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
        0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Band1 BW",
        "Band1 BW",
        juce::NormalisableRange<float>(100.f, 1000.f, 0.5f, 1.f),
        100.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Band1 BW Gain",
        "Band1 BW Gain",
        juce::NormalisableRange<float>(0.5f, 12.f, 0.5f, 1.f),
        0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Band1 Reference",
        "Band1 Reference",
        juce::NormalisableRange<float>(-2.f, 2.f, 0.01f, 1.f),
        0.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Band2 Freq",
        "Band2 Freq",
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, .25f),
        1000.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Band2 Wzmocnienie",
        "Band2 Wzmocnienie",
        juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
        0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Band2 BW",
        "Band2 BW",
        juce::NormalisableRange<float>(100.f, 1000.f, 0.5f, 1.f),
        100.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Band2 BW Gain",
        "Band2 BW Gain",
        juce::NormalisableRange<float>(0.f, 12.f, 0.5f, 1.f),
        0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Band2 Reference",
        "Band2 Reference",
        juce::NormalisableRange<float>(-2.f, 2.f, 0.5f, 1.f),
        0.f));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ParametricEQAudioProcessor();
}
