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

    using Filter = juce::dsp::IIR::Filter<float>;
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
    using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;
    MonoChain leftChain, rightChain;

    enum ChainPositions
    {
        LowCut,
        Peak,
        HighCut
    };

    void updatePeakFilter(const ChainSettings& chainSettings);

    using Coefficients = Filter::CoefficientsPtr;
    static void updateCoefficients(Coefficients& oldCo, Coefficients& newCo);

    template<typename ChainType, typename CoefficientType>
    void updateCutFilter(ChainType& LowCut, const CoefficientType& cutCoefficients, Slope& lowCutSlope)
    {
        LowCut.template setBypassed<0>(true);
        LowCut.template setBypassed<1>(true);
        LowCut.template setBypassed<2>(true);
        LowCut.template setBypassed<3>(true);

        switch (lowCutSlope)
        {
        case Slope_12:
        {
            *LowCut.template get<0>().coefficients = *cutCoefficients[0];
            LowCut.template setBypassed<0>(false);
            break;
        }
        case Slope_24:
        {
            *LowCut.template get<0>().coefficients = *cutCoefficients[0];
            LowCut.template setBypassed<0>(false);
            *LowCut.template get<1>().coefficients = *cutCoefficients[1];
            LowCut.template setBypassed<1>(false);
            break;
        }
        case Slope_36:
        {
            *LowCut.template get<0>().coefficients = *cutCoefficients[0];
            LowCut.template setBypassed<0>(false);
            *LowCut.template get<1>().coefficients = *cutCoefficients[1];
            LowCut.template setBypassed<1>(false);
            *LowCut.template get<2>().coefficients = *cutCoefficients[2];
            LowCut.template setBypassed<2>(false);
            break;
        }
        case Slope_48:
        {
            *LowCut.template get<0>().coefficients = *cutCoefficients[0];
            LowCut.template setBypassed<0>(false);
            *LowCut.template get<1>().coefficients = *cutCoefficients[1];
            LowCut.template setBypassed<1>(false);
            *LowCut.template get<2>().coefficients = *cutCoefficients[2];
            LowCut.template setBypassed<2>(false);
            *LowCut.template get<3>().coefficients = *cutCoefficients[3];
            LowCut.template setBypassed<3>(false);
            break;
        }
        }
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQ_SCAudioProcessor)
};
