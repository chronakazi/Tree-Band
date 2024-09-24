/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "juce_gui_basics/juce_gui_basics.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class TreeBandAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    TreeBandAudioProcessorEditor (TreeBandAudioProcessor&);
    ~TreeBandAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TreeBandAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TreeBandAudioProcessorEditor)
};
