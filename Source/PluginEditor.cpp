/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleEQ_SCAudioProcessorEditor::SimpleEQ_SCAudioProcessorEditor (SimpleEQ_SCAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    std::vector<juce::Component*> components = getComponents();
    for (auto* component : components)
    {
        addAndMakeVisible(component);
    }

    setSize(600, 400);
}

SimpleEQ_SCAudioProcessorEditor::~SimpleEQ_SCAudioProcessorEditor()
{
}

//==============================================================================
void SimpleEQ_SCAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void SimpleEQ_SCAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.33);

    auto lowCutArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    auto highCutArea = bounds.removeFromRight(bounds.getWidth() * 0.5);

    auto peakFreqArea = bounds.removeFromTop(bounds.getHeight() * 0.33);
    auto peakGainArea = bounds.removeFromTop(bounds.getHeight() * 0.5);
    auto peakQArea = bounds;

    lowCutSlider.setBounds(lowCutArea.removeFromTop(lowCutArea.getHeight() * 0.5));
    lowCutSlopeSlider.setBounds(lowCutArea);

    highCutSlider.setBounds(highCutArea.removeFromTop(highCutArea.getHeight() * 0.5));
    highCutSlopeSlider.setBounds(highCutArea);

    peakFreqSlider.setBounds(peakFreqArea);
    peakGainSlider.setBounds(peakGainArea);
    peakQSlider.setBounds(peakQArea);
}

std::vector<juce::Component*> SimpleEQ_SCAudioProcessorEditor::getComponents()
{
    return { &peakFreqSlider, &peakGainSlider, &peakQSlider, &lowCutSlider, &highCutSlider, &lowCutSlopeSlider, &highCutSlopeSlider };
}
