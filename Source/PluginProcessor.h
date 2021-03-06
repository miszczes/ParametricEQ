/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


struct Nastawy 
{
    float Band1Freq{ 0 }, Band1GainToDB{ 0 }, Band1BW{ 1.f }, Band1BWGain{ 0 }, Band1GainRef{ 0 };
    float Band2Freq{ 0 }, Band2GainToDB{ 0 }, Band2BW{ 1.f }, Band2BWGain{ 0 }, Band2GainRef{ 0 };
    float Band3Freq{ 0 }, Band3GainToDB{ 0 }, Band3BW{ 1.f }, Band3BWGain{ 0 }, Band3GainRef{ 0 };
    float Band4Freq{ 0 }, Band4GainToDB{ 0 }, Band4BW{ 1.f }, Band4BWGain{ 0 }, Band4GainRef{ 0 };
    float LowShelfFreq{ 15000.f }, LowShelfQ{ 1.f }, LowShelfGain{ 0 };
    float HighShelfFreq{ 15000.f }, HighShelfQ{ 1.f }, HighShelfGain{ 0 };
};

Nastawy zbierzNastawy(juce::AudioProcessorValueTreeState& apvts);

using Band = juce::dsp::IIR::Filter<float>;

using LowShelf = Band;

using HighShelf = Band;

using Mono = juce::dsp::ProcessorChain<LowShelf, Band, Band, Band, Band, HighShelf>; //istancja lancucha filtrow dla jednego kanalu

using Wspolczynniki = Band::CoefficientsPtr;
void updateCoeffs(Wspolczynniki& old, const Wspolczynniki& nowe);
void Wzory(float f0, float G, float BW, float BG, float G0, float sampleRate);

Wspolczynniki makeLowShelf(const Nastawy& nastawy, double sampleRate);

Wspolczynniki makePeakFilter1(const Nastawy& nastawy, double sampleRate);
Wspolczynniki makePeakFilter2(const Nastawy& nastawy, double sampleRate);
Wspolczynniki makePeakFilter3(const Nastawy& nastawy, double sampleRate);
Wspolczynniki makePeakFilter4(const Nastawy& nastawy, double sampleRate);

Wspolczynniki makeHighShelf(const Nastawy& nastawy, double sampleRate);








//==============================================================================
/**
*/
class ParametricEQAudioProcessor  : public juce::AudioProcessor,
    juce::AudioProcessorValueTreeState::Listener
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

    void parameterChanged(const juce::String& parameterID, float newValue) override;

    //float beta = 0;
    //float b0{ 0 }, b1{ 0 }, b2{ 0 }, a0{ 1.f }, a1{ 0 }, a2{ 0 };


    Mono leftCh, rightCh; //rozdzial lancucha filtrow mono na stereo

    // void generujWspolczynniki(double fs, float f0, float G0, float Bf, float GB, float G, const size_t index);

    void updateLowShelf(const Nastawy& nastawy);

    void updatePeak1(const Nastawy& nastawy);
    void updatePeak2(const Nastawy& nastawy);
    void updatePeak3(const Nastawy& nastawy);
    void updatePeak4(const Nastawy& nastawy);

    void updateHighShelf(const Nastawy& nastawy);
    

   

    void updateFilters();


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParametricEQAudioProcessor)
};
