/*
  ==============================================================================

    CompressorBandControls.cpp
    Created: 22 Oct 2024 5:34:52pm
    Author:  George Brewster

  ==============================================================================
*/

#include "CompressorBandControls.h"
#include "../dsp/Params.h"
#include "Utilities.h"
#include "../PluginEditor.h"

CompressorBandControls::CompressorBandControls(juce::AudioProcessorValueTreeState& apv) :
apvts(apv),
thresholdSlider(nullptr, "dB", "THRESHOLD"),
attackSlider(nullptr, "ms", "ATTACK"),
releaseSlider(nullptr, "ms", "RELEASE"),
ratioSlider(nullptr, "")
{
    addAndMakeVisible(thresholdSlider);
    addAndMakeVisible(ratioSlider);
    addAndMakeVisible(attackSlider);
    addAndMakeVisible(releaseSlider);
    
    bypassButton.addListener(this);
    soloButton.addListener(this);
    muteButton.addListener(this);
    
    bypassButton.setName("X");
    soloButton.setName("S");
    muteButton.setName("M");
    
    bypassButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::yellow);
    bypassButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::black);
    
    soloButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::limegreen);
    soloButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::black);
    
    muteButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::red);
    muteButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::black);
    
    addAndMakeVisible(bypassButton);
    addAndMakeVisible(soloButton);
    addAndMakeVisible(muteButton);
    
    lowBand.setName("L");
    midBand.setName("M");
    highBand.setName("H");
    
    lowBand.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colour(ColorPalette::Accent));
    lowBand.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour(ColorPalette::Secondary));
    
    midBand.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colour(ColorPalette::Accent));
    midBand.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour(ColorPalette::Secondary));
    
    highBand.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colour(ColorPalette::Accent));
    highBand.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour(ColorPalette::Secondary));
    
    lowBand.setRadioGroupId(1);
    midBand.setRadioGroupId(1);
    highBand.setRadioGroupId(1);
    
    auto buttonSwitcher = [safePtr = this->safePtr]()
    {
        if (auto* c = safePtr.getComponent())
        {
            c->updateAttachments();
        }
    };
    
    lowBand.onClick = buttonSwitcher;
    midBand.onClick = buttonSwitcher;
    highBand.onClick = buttonSwitcher;
    
    lowBand.setToggleState(true, juce::NotificationType::dontSendNotification);
    
    updateAttachments();
    updateSliderEnablements();
    updateBandSelectButtonStates();
    
    
    addAndMakeVisible(lowBand);
    addAndMakeVisible(midBand);
    addAndMakeVisible(highBand);
    
}

CompressorBandControls::~CompressorBandControls()
{
    bypassButton.removeListener(this);
    soloButton.removeListener(this);
    muteButton.removeListener(this);
}

void CompressorBandControls::resized()
{
    using namespace juce;
    
    auto bounds = getLocalBounds();
    
    auto createBandButtonControlBox = [](std::vector<Component*> comps)
    {
        FlexBox flexBox;
        flexBox.flexDirection = FlexBox::Direction::column;
        flexBox.flexWrap = FlexBox::Wrap::noWrap;
        
        auto spacer = FlexItem().withHeight(4);
        
        for (auto* comp : comps)
        {
            flexBox.items.add(spacer);
            flexBox.items.add(FlexItem(*comp).withFlex(1.0f));
        }
        
        flexBox.items.add(spacer);
        
        return flexBox;
    };
    
    auto bandButtonControlBox = createBandButtonControlBox({&bypassButton, &soloButton, &muteButton});
    
    auto bandSelectControlBox = createBandButtonControlBox({&lowBand, &midBand, &highBand});
    
    FlexBox flexBox;
    flexBox.flexDirection = FlexBox::Direction::row;
    flexBox.flexWrap = FlexBox::Wrap::noWrap;
    
    auto endCap = FlexItem().withWidth(7);
    auto spacer = FlexItem().withWidth(4);
    
    flexBox.items.add(endCap);
    flexBox.items.add(FlexItem(bandSelectControlBox).withWidth(30));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(thresholdSlider).withFlex(1.0f));
    flexBox.items.add(FlexItem(ratioSlider).withFlex(1.0f));
    flexBox.items.add(FlexItem(attackSlider).withFlex(1.0f));
    flexBox.items.add(FlexItem(releaseSlider).withFlex(1.0f));
//    flexBox.items.add(endCap);
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(bandButtonControlBox).withWidth(30));
    flexBox.items.add(endCap);
    
    flexBox.performLayout(bounds);
}

void CompressorBandControls::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds();
    drawModuleBackground(g, bounds);
}

void CompressorBandControls::buttonClicked(juce::Button *button)
{
    updateSliderEnablements();
    updateSoloMuteBypassToggleStates(*button);
    updateActiveBandFillColors(*button);
}

void CompressorBandControls::updateActiveBandFillColors(juce::Button& clickedButton)
{
    jassert(activeBand != nullptr);
    
    if (clickedButton.getToggleState() == false)
    {
        resetActiveBandColors();
    }
    else
    {
        refreshBandButtonColors(*activeBand, clickedButton);
    }
}

void CompressorBandControls::refreshBandButtonColors(juce::Button &band, juce::Button &colorSource)
{
    band.setColour(juce::TextButton::ColourIds::buttonOnColourId,
                   colorSource.findColour(juce::TextButton::ColourIds::buttonOnColourId));
    band.setColour(juce::TextButton::ColourIds::buttonColourId,
                   colorSource.findColour(juce::TextButton::ColourIds::buttonOnColourId));
    band.repaint();
}

void CompressorBandControls::resetActiveBandColors()
{
    activeBand->setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colour(ColorPalette::Accent));
    activeBand->setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour(ColorPalette::Secondary));
    activeBand->repaint();
}

void CompressorBandControls::updateBandSelectButtonStates()
{
    using namespace Params;
    
    std::vector<std::array<Names, 3>> paramsToCheck
    {
        {Names::Solo_Low_Band, Names::Mute_Low_Band, Names::Bypassed_Low_Band},
        {Names::Solo_Mid_Band, Names::Mute_Mid_Band, Names::Bypassed_Mid_Band},
        {Names::Solo_High_Band, Names::Mute_High_Band, Names::Bypassed_High_Band}
    };
    
    const auto& params = GetParams();
    auto paramHelper = [&params, this](const auto& name)
    {
        return dynamic_cast<juce::AudioParameterBool*>(&getParam(apvts, params, name));
    };
    
    for (size_t i = 0; i < paramsToCheck.size(); ++i)
    {
        auto& list = paramsToCheck[i];
        
        auto* bandButton =  (i == 0) ?  &lowBand :
                            (i == 1) ?  &midBand :
                                        &highBand;
        
        if (auto* solo = paramHelper(list[0]);
            solo->get())
        {
            refreshBandButtonColors(*bandButton, soloButton);
        }
        else if (auto* mute = paramHelper(list[1]);
                 mute->get())
        {
            refreshBandButtonColors(*bandButton, muteButton);
        }
        else if (auto* bypass = paramHelper(list[2]);
                 bypass->get())
        {
            refreshBandButtonColors(*bandButton, bypassButton);
        }
    }
    
}

void CompressorBandControls::updateSliderEnablements()
{
    auto disabled = muteButton.getToggleState() || bypassButton.getToggleState();
    thresholdSlider.setEnabled(!disabled);
    ratioSlider.setEnabled(!disabled);
    attackSlider.setEnabled(!disabled);
    releaseSlider.setEnabled(!disabled);
}

void CompressorBandControls::updateSoloMuteBypassToggleStates(juce::Button &clickedButton)
{
    if (&clickedButton == &bypassButton && bypassButton.getToggleState())
    {
        soloButton.setToggleState(false, juce::NotificationType::sendNotification);
        muteButton.setToggleState(false, juce::NotificationType::sendNotification);
    }
    else if (&clickedButton == &soloButton && soloButton.getToggleState())
    {
        bypassButton.setToggleState(false, juce::NotificationType::sendNotification);
        muteButton.setToggleState(false, juce::NotificationType::sendNotification);
    }
    else if (&clickedButton == &muteButton && muteButton.getToggleState())
    {
        bypassButton.setToggleState(false, juce::NotificationType::sendNotification);
        soloButton.setToggleState(false, juce::NotificationType::sendNotification);
    }
}

void CompressorBandControls::updateAttachments()
{
    enum BandType
    {
        Low,
        Mid,
        High
    };
    
    BandType bandType = [this]()
    {
        if (lowBand.getToggleState())
            return BandType::Low;
        if (midBand.getToggleState())
            return BandType::Mid;
        return BandType::High;
    }();
    
    using namespace Params;
    std::vector<Names> names;
    
    switch (bandType)
    {
        case Low:
        {
            names = std::vector<Names>
            {
                Names::Threshold_Low_Band,
                Names::Ratio_Low_Band,
                Names::Attack_Low_Band,
                Names::Release_Low_Band,
                Names::Bypassed_Low_Band,
                Names::Solo_Low_Band,
                Names::Mute_Low_Band
            };
            
            activeBand = &lowBand;
            
            break;
        }
        case Mid:
        {
            names = std::vector<Names>
            {
                Names::Threshold_Mid_Band,
                Names::Ratio_Mid_Band,
                Names::Attack_Mid_Band,
                Names::Release_Mid_Band,
                Names::Bypassed_Mid_Band,
                Names::Solo_Mid_Band,
                Names::Mute_Mid_Band
            };
            
            activeBand = &midBand;
            
            break;
        }
        case High:
        {
            names = std::vector<Names>
            {
                Names::Threshold_High_Band,
                Names::Ratio_High_Band,
                Names::Attack_High_Band,
                Names::Release_High_Band,
                Names::Bypassed_High_Band,
                Names::Solo_High_Band,
                Names::Mute_High_Band
            };
            
            activeBand = &highBand;
            
            break;
        }
    }
    
    enum Pos
    {
        Threshold,
        Ratio,
        Attack,
        Release,
        Bypass,
        Solo,
        Mute
    };
    
    const auto& params = GetParams();
    
    auto getParamHelper = [&apvts = this->apvts, &params, &names](const auto& pos) -> auto&
    {
        return getParam(apvts, params, names.at(pos));
    };
    
    thresholdSliderAttachment.reset();
    ratioSliderAttachment.reset();
    attackSliderAttachment.reset();
    releaseSliderAttachment.reset();
    bypassButtonAttachment.reset();
    soloButtonAttachment.reset();
    muteButtonAttachment.reset();
    
    auto& thresholdParam = getParamHelper(Pos::Threshold);
    addLabelPairs(thresholdSlider.labels, thresholdParam, "dB");
    thresholdSlider.changeParam(&thresholdParam);
    
    auto& attackParam = getParamHelper(Pos::Attack);
    addLabelPairs(attackSlider.labels, attackParam, "ms");
    attackSlider.changeParam(&attackParam);
    
    auto& releaseParam = getParamHelper(Pos::Release);
    addLabelPairs(releaseSlider.labels, releaseParam, "ms");
    releaseSlider.changeParam(&releaseParam);
    
    auto& ratioParamRap = getParamHelper(Pos::Ratio);
    ratioSlider.labels.clear();
    ratioSlider.labels.add({0.0f, "1:1"});
    auto ratioParam = dynamic_cast<juce::AudioParameterChoice*>(&ratioParamRap);
    ratioSlider.labels.add({1.0f,
        juce::String(ratioParam->choices.getReference(ratioParam->choices.size() - 1).getIntValue()) + ":1"});
    ratioSlider.changeParam(ratioParam);
    
    auto makeAttachmentHelper = [&apvts = this->apvts, &params](auto& attachment,
                                                                const auto& name,
                                                                auto& slider)
    {
        makeAttachment(attachment, apvts, params, name, slider);
    };
    
    makeAttachmentHelper(thresholdSliderAttachment, names[Pos::Threshold], thresholdSlider);
    makeAttachmentHelper(ratioSliderAttachment, names[Pos::Ratio], ratioSlider);
    makeAttachmentHelper(attackSliderAttachment, names[Pos::Attack], attackSlider);
    makeAttachmentHelper(releaseSliderAttachment, names[Pos::Release], releaseSlider);
    makeAttachmentHelper(bypassButtonAttachment, names[Pos::Bypass], bypassButton);
    makeAttachmentHelper(soloButtonAttachment, names[Pos::Solo], soloButton);
    makeAttachmentHelper(muteButtonAttachment, names[Pos::Mute], muteButton);
}
