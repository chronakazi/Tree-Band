/*
  ==============================================================================

    CompressorBandControls.h
    Created: 22 Oct 2024 5:34:52pm
    Author:  George Brewster

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "RotarySliderWithLabels.h"

struct CompressorBandControls : juce::Component, juce::Button::Listener
{
    CompressorBandControls(juce::AudioProcessorValueTreeState& apvts);
    ~CompressorBandControls() override;
    
    void resized() override;
    void paint(juce::Graphics& g) override;
    
    void buttonClicked (juce::Button* button) override;
private:
    juce::AudioProcessorValueTreeState& apvts;
    
    using RSWL = RotarySliderWithLabels;
    
    RSWL thresholdSlider, attackSlider, releaseSlider;
    RatioSlider ratioSlider;
    
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> thresholdSliderAttachment,
                                ratioSliderAttachment,
                                attackSliderAttachment,
                                releaseSliderAttachment;
    
    juce::ToggleButton bypassButton, soloButton, muteButton, lowBand, midBand, highBand;
    
    using BtnAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::unique_ptr<BtnAttachment> bypassButtonAttachment,
                                    soloButtonAttachment,
                                    muteButtonAttachment;
    
    juce::Component::SafePointer<CompressorBandControls> safePtr {this};
    
    void updateAttachments();
    void updateSliderEnablements();
    void updateSoloMuteBypassToggleStates(juce::Button& clickedButton);
};
