/*
  ==============================================================================

    CompressorBand.cpp
    Created: 22 Oct 2024 5:45:01pm
    Author:  George Brewster

  ==============================================================================
*/

#include "CompressorBand.h"

void CompressorBand::prepare(const juce::dsp::ProcessSpec& spec)
{
    compressor.prepare(spec);
}

void CompressorBand::updateCompressorSettings()
{
    compressor.setThreshold(threshold->get());
    compressor.setAttack(attack->get());
    compressor.setRelease(release->get());
    compressor.setRatio(ratio->getCurrentChoiceName().getFloatValue());
}

void CompressorBand::process(juce::AudioBuffer<float>& buffer)
{
    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);
    
    context.isBypassed = bypassed->get();
    
    compressor.process(context);
}
