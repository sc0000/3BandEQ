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
class SimpleEQ_SCAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SimpleEQ_SCAudioProcessorEditor (SimpleEQ_SCAudioProcessor&);
    ~SimpleEQ_SCAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQ_SCAudioProcessorEditor)
};
