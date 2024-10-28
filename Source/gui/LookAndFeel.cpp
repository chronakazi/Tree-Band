/*
  ==============================================================================

    LookAndFeel.cpp
    Created: 22 Oct 2024 5:01:41pm
    Author:  George Brewster

  ==============================================================================
*/

#include "LookAndFeel.h"
#include "../PluginEditor.h"
#include "CustomButtons.h"

void LookAndFeel::drawRotarySlider(juce::Graphics & g,
                                   int x,
                                   int y,
                                   int width,
                                   int height,
                                   float sliderPosProportional,
                                   float rotaryStartAngle,
                                   float rotaryEndAngle,
                                   juce::Slider & slider)
{
    using namespace juce;
    
    auto bounds = Rectangle<float>(x, y, width, height);
    
    auto enabled = slider.isEnabled();
    
//    Rotary slider bg color
    g.setColour(enabled ? Colour(ColorPalette::Accent) : Colours::dimgrey);
    g.fillEllipse(bounds);
    
//    Rotary slider border color.
    g.setColour(Colour(ColorPalette::Tertiary));
    g.drawEllipse(bounds, 1.f);
    
    if (auto* rswl = dynamic_cast<RotarySliderWithLabels*>(&slider))
    {
        auto center = bounds.getCentre();
//        Indicator
        Path p;
        Rectangle<float> r;
        r.setLeft(center.getX() - 2);
        r.setRight(center.getX() + 2);
        r.setTop(bounds.getY());
        r.setBottom(center.getY() - rswl->getTextHeight() * 1.5);
        
        p.addRoundedRectangle(r, 2.f);
        jassert(rotaryStartAngle < rotaryEndAngle);
        
        auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f,
                                 rotaryStartAngle, rotaryEndAngle);
        
        p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));
        
        g.setColour(Colour(ColorPalette::Tertiary));
        g.fillPath(p);
        
//        Param text
        g.setFont(rswl->getTextHeight());
        auto text = rswl->getDisplayString();
        auto strWidth = GlyphArrangement::getStringWidth(g.getCurrentFont(), text);
        
        r.setSize(strWidth + 4, rswl->getTextHeight() + 2);
        r.setCentre(bounds.getCentre());
        
        g.setColour(enabled ? Colour(ColorPalette::Accent) : Colours::dimgrey);
        g.fillRect(r);
        
        g.setColour(enabled ? Colour(ColorPalette::Primary) : Colour(ColorPalette::Tertiary));
        g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1);
    }

}

void LookAndFeel::drawToggleButton(juce::Graphics &g,
                                   juce::ToggleButton &toggleButton,
                                   bool shouldDrawButtonAsHighlighted,
                                   bool shouldDrawButtonAsDown)
{
    using namespace juce;
    
    if (auto* pb = dynamic_cast<PowerButton*>(&toggleButton))
    {
        Path powerButton;
        
        auto bounds = toggleButton.getLocalBounds();
        
        auto size = jmin(bounds.getWidth(), bounds.getHeight()) - 7;
        auto r = bounds.withSizeKeepingCentre(size, size).toFloat();
        
        float ang = 30.f; //30.f;
        
        size -= 8;
        
        powerButton.addCentredArc(r.getCentreX(),
                                  r.getCentreY() + 2,
                                  size * 0.5,
                                  size * 0.5,
                                  0.f,
                                  degreesToRadians(ang),
                                  degreesToRadians(360.f - ang),
                                  true);
        
        powerButton.startNewSubPath(r.getCentreX(), r.getY() + 2);
        powerButton.lineTo(r.getCentre());
        
        PathStrokeType pst(2.f, PathStrokeType::JointStyle::curved);
        
        auto color = toggleButton.getToggleState() ? Colours::dimgrey : Colour(ColorPalette::Accent);
        
        r.setY(r.getCentreY() - 7);
        g.setColour(color);
        g.strokePath(powerButton, pst);
        g.drawEllipse(r, 1.f);
    }
    else if (auto* analyzerButton = dynamic_cast<AnalyzerButton*>(&toggleButton))
    {
        auto color = ! toggleButton.getToggleState() ? Colours::dimgrey : Colour(ColorPalette::Accent);
        
        g.setColour(color);
        
        auto bounds = toggleButton.getLocalBounds();
        g.drawRect(bounds);
        
        g.strokePath(analyzerButton->randomPath, PathStrokeType(1.f));
    }
    else
    {
        auto bounds = toggleButton.getLocalBounds().reduced(3);
        
        auto buttonIsOn = toggleButton.getToggleState();
        
        const int cornerSize = 4;
        
//        button color
        g.setColour(buttonIsOn ? 
                    toggleButton.findColour(TextButton::ColourIds::buttonOnColourId) :
                    toggleButton.findColour(TextButton::ColourIds::buttonColourId));
        
        g.fillRoundedRectangle(bounds.toFloat(), cornerSize);
        
//        button text color
        g.setColour(buttonIsOn ? juce::Colour(ColorPalette::Tertiary) : juce::Colour(ColorPalette::Accent));
        g.drawRoundedRectangle(bounds.toFloat(), cornerSize, 1);
        g.drawText(toggleButton.getName(), bounds, juce::Justification::centred, 1);
        
    }
}
