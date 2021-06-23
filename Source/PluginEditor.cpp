/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleEQ_SCAudioProcessorEditor::SimpleEQ_SCAudioProcessorEditor(SimpleEQ_SCAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),
    peakFreqSliderAttachment(audioProcessor.apvts, "Peak Freq", peakFreqSlider),
    peakGainSliderAttachment(audioProcessor.apvts, "Peak Gain", peakGainSlider),
    peakQSliderAttachment(audioProcessor.apvts, "Peak Q", peakQSlider),
    lowCutSliderAttachment(audioProcessor.apvts, "LowCut Freq", lowCutSlider),
    highCutSliderAttachment(audioProcessor.apvts, "HighCut Freq", highCutSlider),
    lowCutSlopeSliderAttachment(audioProcessor.apvts, "LowCut Slope", lowCutSlopeSlider),
    highCutSlopeSliderAttachment(audioProcessor.apvts, "HighCut Slope", highCutSlopeSlider)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    setSize(600, 400);

    startTimerHz(60);

    std::vector<juce::Component*> components = getComponents();
    for (auto* component : components)
    {
        addAndMakeVisible(component);
    }

    const auto& params = audioProcessor.getParameters();
    for (auto* param : params)
    {
        param->addListener(this);
    }

}

SimpleEQ_SCAudioProcessorEditor::~SimpleEQ_SCAudioProcessorEditor()
{
    const auto& params = audioProcessor.getParameters();
    for (auto* param : params)
    {
        param->removeListener(this);
    }
}

//==============================================================================
void SimpleEQ_SCAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.33);
    auto width = responseArea.getWidth();

    auto& lowCut = monoChain.get<ChainPositions::LowCut>();
    auto& peak = monoChain.get<ChainPositions::Peak>();
    auto& highCut = monoChain.get<ChainPositions::HighCut>();

    auto sampleRate = audioProcessor.getSampleRate();

    std::vector<double> magnitudes; //magnitude of all frequencies between 20 and 20000 Hz
    magnitudes.resize(width); // one for every pixel

    for (size_t i = 0; i < magnitudes.size(); ++i)
    {
        double magnitude = 1.f;
        auto freq = juce::mapToLog10<double>(double(i) / double(magnitudes.size()), 20, 20000);

        // get magnitude for frequency i as a product from all filters
        if (!monoChain.isBypassed<ChainPositions::Peak>())
        {
            magnitude *= peak.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }

        getMagForFreqCutFilters(lowCut, magnitude, freq, sampleRate);
        getMagForFreqCutFilters(highCut, magnitude, freq, sampleRate);

        magnitudes[i] = juce::Decibels::gainToDecibels(magnitude);
    }

    juce::Path responseCurve;
    const double outputMin = responseArea.getBottom() - 10.f; // don't draw into frame
    const double outputMax = responseArea.getY() + 10.f;

    auto map = [outputMin, outputMax](double input)
    {
        return juce::jmap<double>(input, -24.f, 24.f, outputMin, outputMax);
    };

    responseCurve.startNewSubPath(responseArea.getX(), map(magnitudes.front()));

    for (size_t i = 1; i < magnitudes.size(); ++i)
    {
        responseCurve.lineTo(responseArea.getX() + i, map(magnitudes[i]));
    }

    g.setColour(juce::Colours::white);
    g.drawRoundedRectangle(responseArea.toFloat(), 4.f, 1.f);
    g.strokePath(responseCurve, juce::PathStrokeType(2.));
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

void SimpleEQ_SCAudioProcessorEditor::parameterValueChanged(int parameterIndex, float newValue)
{
    parameterChanged.set(true);
}

void SimpleEQ_SCAudioProcessorEditor::timerCallback()
{
    if (parameterChanged.compareAndSetBool(false, true))
    {
        // update monoChain, repaint
        auto chainSettings = getChainSettings(audioProcessor.apvts);
        auto peakCoefficients = makePeakFilter(chainSettings, audioProcessor.getSampleRate());
        
        updateCoefficients(monoChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);

        repaint();

    }
}


std::vector<juce::Component*> SimpleEQ_SCAudioProcessorEditor::getComponents()
{
    return { &peakFreqSlider, &peakGainSlider, &peakQSlider, &lowCutSlider, &highCutSlider, &lowCutSlopeSlider, &highCutSlopeSlider };
}

void SimpleEQ_SCAudioProcessorEditor::getMagForFreqCutFilters(CutFilter& cutFilter, double& magnitude, double& freq, const double& sampleRate)
{
    if (!cutFilter.isBypassed<0>())
    {
        magnitude *= cutFilter.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
    }
    if (!cutFilter.isBypassed<1>())
    {
        magnitude *= cutFilter.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
    }
    if (!cutFilter.isBypassed<2>())
    {
        magnitude *= cutFilter.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
    }
    if (!cutFilter.isBypassed<3>())
    {
        magnitude *= cutFilter.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
    }
}