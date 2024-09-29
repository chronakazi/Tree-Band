/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "melatonin_inspector/melatonin_inspector.h"
#include "melatonin_blur/melatonin_blur.h"

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
    
    std::unique_ptr<melatonin::Inspector> inspector;
    juce::TextButton inspectButton { "Inspect the UI" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TreeBandAudioProcessorEditor)
};
