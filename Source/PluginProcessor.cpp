/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

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

    auto chainSettings = getChainSettings(apvts);
    auto Band1Coeff = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,
                                                                        chainSettings.Band1Freq,
                                                                        chainSettings.Band1Q,
                                                                        juce::Decibels::decibelsToGain(chainSettings.Band1GainToDB));
    *leftCh.get<chainPos::Band1>().coefficients = *Band1Coeff;
    *rightCh.get<chainPos::Band1>().coefficients = *Band1Coeff;

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

    auto chainSettings = getChainSettings(apvts);
    auto Band1Coeff = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
                                                                        chainSettings.Band1Freq,
                                                                        chainSettings.Band1Q,
                                                                        juce::Decibels::decibelsToGain(chainSettings.Band1GainToDB));
    *leftCh.get<chainPos::Band1>().coefficients = *Band1Coeff;
    *rightCh.get<chainPos::Band1>().coefficients = *Band1Coeff;

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
    ustawienia.Band1Q = apvts.getRawParameterValue("Band1 Q")->load();

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
                                                            juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, .25f),
                                                            0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Band1 Q",
                                                            "Band1 Q",
                                                            juce::NormalisableRange<float>(0.1f, 10.f, 0.5f, .25f),
                                                            1.f));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ParametricEQAudioProcessor();
}
