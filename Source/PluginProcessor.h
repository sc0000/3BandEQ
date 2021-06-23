/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

enum Slope
{
    Slope_12, Slope_24, Slope_36, Slope_48
};

struct ChainSettings
{
    float peakFreq = 0.f, peakGainInDecibels = 0.f, peakQ = 1.f;
    float lowCutFreq = 0.f, highCutFreq = 0.f;
    Slope lowCutSlope = Slope::Slope_12, highCutSlope = Slope::Slope_12;
};

ChainSettings getChainSettings(const juce::AudioProcessorValueTreeState& apvts);

using Filter = juce::dsp::IIR::Filter<float>;
using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;

enum ChainPositions
{
    LowCut,
    Peak,
    HighCut
};

using Coefficients = Filter::CoefficientsPtr;
void updateCoefficients(Coefficients& oldCo, Coefficients& newCo);

Coefficients makePeakFilter(const ChainSettings chainSettings, double sampleRate);



//==============================================================================
/**
*/
class SimpleEQ_SCAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SimpleEQ_SCAudioProcessor();
    ~SimpleEQ_SCAudioProcessor() override;

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
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout() };

private:
    MonoChain leftChain, rightChain;

    void updatePeakFilter(const ChainSettings& chainSettings);



    template<int Index, typename ChainType, typename CoefficientType>
    void update(ChainType& chain, const CoefficientType& coefficients)
    {
        updateCoefficients(chain.template get<Index>().coefficients, coefficients[Index]);
        chain.template setBypassed<Index>(false);
    }

    template<typename ChainType, typename CoefficientType>
    void updateCutFilter(ChainType& cutFilter, const CoefficientType& cutCoefficients, Slope& cutFilterSlope)
    {
        cutFilter.template setBypassed<0>(true);
        cutFilter.template setBypassed<1>(true);
        cutFilter.template setBypassed<2>(true);
        cutFilter.template setBypassed<3>(true);

        switch (cutFilterSlope)
        {
        case Slope_48:
        {
            update<Slope_48>(cutFilter, cutCoefficients);
            __fallthrough;
        }
        case Slope_36:
        {
            update<Slope_36>(cutFilter, cutCoefficients);
            __fallthrough;
        }
        case Slope_24:
        {
            update<Slope_24>(cutFilter, cutCoefficients);
            __fallthrough;
        }
        case Slope_12:
        {
            update<Slope_12>(cutFilter, cutCoefficients);
            break;
        }
        }
    }

    void updateLowCutFilters(ChainSettings& chainSettings);
    void updateHighCutFilters(ChainSettings& chainSettings);
    void updateFilters();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQ_SCAudioProcessor)
};
