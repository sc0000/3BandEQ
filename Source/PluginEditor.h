/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

struct CustomRotarySlider : public juce::Slider
{
    CustomRotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                        juce::Slider::TextEntryBoxPosition::NoTextBox)
    {
    }
};

//==============================================================================
/**
*/
class SimpleEQ_SCAudioProcessorEditor  : public juce::AudioProcessorEditor, 
                                                juce::AudioProcessorParameter::Listener,
                                                juce::Timer
{
public:
    SimpleEQ_SCAudioProcessorEditor (SimpleEQ_SCAudioProcessor&);
    ~SimpleEQ_SCAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {};
    
    void timerCallback() override;


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleEQ_SCAudioProcessor& audioProcessor;

    CustomRotarySlider peakFreqSlider, peakGainSlider, peakQSlider, 
        lowCutSlider, highCutSlider, lowCutSlopeSlider, highCutSlopeSlider;

    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;

    Attachment peakFreqSliderAttachment, peakGainSliderAttachment, peakQSliderAttachment, 
        lowCutSliderAttachment, highCutSliderAttachment, lowCutSlopeSliderAttachment, highCutSlopeSliderAttachment;

    std::vector<juce::Component*> getComponents();

    MonoChain monoChain;

    juce::Atomic<bool> parameterChanged = false;

    void getMagForFreqCutFilters(CutFilter& cutFilter, double& magnitude, double& freq, const double& sampleRate);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQ_SCAudioProcessorEditor)
};
