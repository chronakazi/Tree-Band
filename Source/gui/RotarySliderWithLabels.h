/*
  ==============================================================================

    RotarySliderWithLabels.h
    Created: 22 Oct 2024 5:07:45pm
    Author:  George Brewster

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

struct RotarySliderWithLabels : juce::Slider
{
    RotarySliderWithLabels(juce::RangedAudioParameter* rap,
                           const juce::String& unitSuffix,
                           const juce::String& title) :
    juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                 juce::Slider::TextEntryBoxPosition::NoTextBox),
    param(rap),
    suffix(unitSuffix)
    {
        setName(title);
    }
    
    struct LabelPos
    {
        float pos;
        juce::String label;
    };
    
    juce::Array<LabelPos> labels;
    
    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const {return 14;}
    virtual juce::String getDisplayString() const;
    
    void changeParam(juce::RangedAudioParameter* p);
protected:
    juce::RangedAudioParameter* param;
    juce::String suffix;
};

struct RatioSlider : RotarySliderWithLabels
{
    RatioSlider(juce::RangedAudioParameter* rap,
                const juce::String& unitSuffix) : RotarySliderWithLabels(rap, unitSuffix, "RATIO") {}
    
    juce::String getDisplayString() const override;
};
