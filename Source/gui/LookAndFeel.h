/*
  ==============================================================================

    LookAndFeel.h
    Created: 22 Oct 2024 5:01:41pm
    Author:  George Brewster

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider(juce::Graphics&,
                          int x, int y, int width, int height,
                          float sliderPosProportional,
                          float rotaryStartAngle,
                          float rotaryEndAngle,
                          juce::Slider&) override;
    
    void drawToggleButton (juce::Graphics & g,
                           juce::ToggleButton & button,
                           bool shouldDrawButtonAsHighlighted,
                           bool shouldDrawButtonAsDown) override;
};
