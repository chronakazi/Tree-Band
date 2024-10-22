/*
  ==============================================================================

    Buttons.h
    Created: 22 Oct 2024 5:15:25pm
    Author:  George Brewster

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

struct PowerButton : juce::ToggleButton {};
struct AnalyzerButton : juce::ToggleButton
{
    void resized() override;

        juce::Path randomPath;
};
