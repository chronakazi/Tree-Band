/*
  ==============================================================================

    Utilities.cpp
    Created: 22 Oct 2024 5:28:00pm
    Author:  George Brewster

  ==============================================================================
*/

#include "Utilities.h"
#include "../PluginEditor.h"

juce::String getValString(const juce::RangedAudioParameter& param, bool getLow, juce::String suffix)
{
    juce::String str;
    
    auto val = getLow ? param.getNormalisableRange().start : param.getNormalisableRange().end;
    
    bool useK = truncateKiloValue(val);
    str << val;
    
    if (useK)
        str << "k";
    
    str << suffix;
    
    return str;
}

//==============================================================================
void drawModuleBackground(juce::Graphics &g,
                          juce::Rectangle<int> bounds)
{
    using namespace juce;
    
    g.setColour(Colour(ColorPalette::Secondary));
    g.fillAll();
    
    auto localBounds = bounds;
    
    bounds.reduce(3, 3);
    g.setColour(Colours::black);
    g.fillRoundedRectangle(bounds.toFloat(), 3);
    
    g.drawRect(localBounds);
}
