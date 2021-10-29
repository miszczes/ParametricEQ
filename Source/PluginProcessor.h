/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


struct ChainSettings
{
    float Band1Freq{ 0 }, Band1GainToDB{ 0 }, Band1BW{ 1.f }, Band1BWGain{ 0 }, Band1GainRef{ 0 };
    float Band2Freq{ 0 }, Band2GainToDB{ 0 }, Band2BW{ 1.f }, Band2BWGain{ 0 }, Band2GainRef{ 0 };

};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);

using Band = juce::dsp::IIR::Filter<float>;

using Mono = juce::dsp::ProcessorChain<Band, Band>;

using Wspolczynniki = Band::CoefficientsPtr;
void updateCoeffs(Wspolczynniki& old, const Wspolczynniki& nowe);

Wspolczynniki makePeakFilter(const ChainSettings& chainSettings, double sampleRate, const size_t index);

//==============================================================================
/**
*/
class ParametricEQAudioProcessor  : public juce::AudioProcessor
{
public:

    //==============================================================================
    ParametricEQAudioProcessor();
    ~ParametricEQAudioProcessor() override;

    //==============================================================================

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parametry", createParameterLayout() };

private:

    float beta = 0;
    float b0{ 0 }, b1{ 0 }, b2{ 0 }, a0{ 1.f }, a1{ 0 }, a2{ 0 };


    Mono leftCh, rightCh;

    enum chainPos
    {
        Band1,
        Band2
    };
    // void generujWspolczynniki(double fs, float f0, float G0, float Bf, float GB, float G, const size_t index);

    void updatePeak(const ChainSettings& chainSettings, const size_t index);

   

    void updateFilters();


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParametricEQAudioProcessor)
};
