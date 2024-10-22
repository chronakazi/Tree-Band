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

#include "gui/LookAndFeel.h"
#include "gui/UtilityComponents.h"
#include "gui/CompressorBandControls.h"
#include "gui/GlobalControls.h"

//==============================================================================

enum ColorPalette
{
    Primary = 0xff001f3f,
    Secondary = 0xff3a6d8c,
    Accent = 0xff6a9ab0,
    Tertiary = 0xffead8b1,
    Pop = 0xffad7976
};

class TreeBandAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    TreeBandAudioProcessorEditor (TreeBandAudioProcessor&);
    ~TreeBandAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    

private:
    LookAndFeel lnf;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TreeBandAudioProcessor& audioProcessor;
    
    std::unique_ptr<melatonin::Inspector> inspector;
    juce::TextButton inspectButton { "Inspect the UI" };
    
    Placeholder controlBar, analyzer /*globalControls,*/ /*bandControls*/;
    GlobalControls globalControls {audioProcessor.apvts};
    CompressorBandControls bandControls {audioProcessor.apvts};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TreeBandAudioProcessorEditor)
};
