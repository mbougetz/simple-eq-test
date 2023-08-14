/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


//Extract params from AudioProcessorValueTreeState

//Struct to keep track of param values;
struct ChainSettings {
    float peakFreq{ 0 }, peakGainInDecibels{ 0 }, peakQuality{ 1.f };
    float lowCutFreq{ 0 }, highCutFreq{ 0 };
    int lowCutSlope{ 0 }, highCutSlope{ 0 };
};

//Helper to wrap param values in a struct
ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);

//==============================================================================
/**
*/
class SimpleEQAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SimpleEQAudioProcessor();
    ~SimpleEQAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override; //called by host when about to start playback
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override; //what happens whenever you hit play button in transport

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

    //Milo
    //Define fcn to get list of all paramaters
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    //Add created layout
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout()}; 
    

private:

    //filter alias; we're only using floats
    //Filters have 12db/oct response by default
    //This is the peak filter
    using Filter = juce::dsp::IIR::Filter<float>;

    //ProcessorChain creates 1 processor out of any numbers chained together; calls process() on them in sequence
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;

    //This is the whole mono signal path w/ all three filter types
    using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;

    //MonoChain only processes 1 channel so there's 2 to process stereo audio
    MonoChain leftChain, rightChain;

    //Define enum
    enum ChainPositions {
        LowCut,
        Peak,
        HighCut
    };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQAudioProcessor)
};
