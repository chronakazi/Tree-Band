/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


TreeBandAudioProcessorEditor::TreeBandAudioProcessorEditor (TreeBandAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    
    addAndMakeVisible (inspectButton);
    
    inspectButton.onClick = [&] {
            if (!inspector)
            {
                inspector = std::make_unique<melatonin::Inspector> (*this);
                inspector->onClose = [this]() { inspector.reset(); };
            }

            inspector->setVisible (true);
        };
    
    setLookAndFeel(&lnf);
    
//    addAndMakeVisible(controlBar);
//    addAndMakeVisible(analyzer);
    addAndMakeVisible(globalControls);
    addAndMakeVisible(bandControls);
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 650);
}

TreeBandAudioProcessorEditor::~TreeBandAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void TreeBandAudioProcessorEditor::paint (juce::Graphics& g)
{
    TRACE_COMPONENT();
    // (Our component is opaque, so we must completely fill the background with a solid colour)
//    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
//
//    g.setColour (juce::Colours::white);
//    g.setFont (juce::FontOptions (15.0f));
//    g.drawText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
    
    g.fillAll(juce::Colour(ColorPalette::Primary));
    
}

void TreeBandAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto area = getLocalBounds();
    auto inspectButtonArea = area.removeFromBottom(50);
    
    inspectButton.setBounds (inspectButtonArea);

    controlBar.setBounds(area.removeFromTop(50));
    
    analyzer.setBounds(area.removeFromTop(275));
    
    globalControls.setBounds(area.removeFromTop(125));
    
    bandControls.setBounds(area.removeFromBottom(150));
    
}
