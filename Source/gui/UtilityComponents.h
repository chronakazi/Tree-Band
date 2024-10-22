/*
  ==============================================================================

    UtilityComponents.h
    Created: 22 Oct 2024 5:18:55pm
    Author:  George Brewster

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

struct Placeholder : juce::Component
{
    Placeholder();
    
    void paint(juce::Graphics& g) override;
    
    juce::Colour customColor;
};

struct RotarySlider : juce::Slider
{
    RotarySlider();
};
