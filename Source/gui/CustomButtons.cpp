/*
  ==============================================================================

    Buttons.cpp
    Created: 22 Oct 2024 5:15:25pm
    Author:  George Brewster

  ==============================================================================
*/

#include "CustomButtons.h"

void AnalyzerButton::resized()
    {
        auto bounds = getLocalBounds();
        auto insetRect = bounds.reduced(4);

        randomPath.clear();

        juce::Random r;

        randomPath.startNewSubPath(insetRect.getX(),
                                   insetRect.getY() + insetRect.getHeight() * r.nextFloat());

        for( auto x = insetRect.getX() + 1; x < insetRect.getRight(); x += 4 )
        {
            randomPath.lineTo(x,
                              insetRect.getY() + insetRect.getHeight() * r.nextFloat());
        }
    }
