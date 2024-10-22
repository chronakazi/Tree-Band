/*
  ==============================================================================

    GlobalControls.cpp
    Created: 22 Oct 2024 5:38:24pm
    Author:  George Brewster

  ==============================================================================
*/

#include "GlobalControls.h"
#include "../dsp/Params.h"
#include "../dsp/CompressorBand.h"
#include "Utilities.h"


GlobalControls::GlobalControls(juce::AudioProcessorValueTreeState& apvts)
{
    using namespace Params;
    const auto& params = GetParams();
    
    auto getParamHelper = [&apvts, &params](const auto& name) -> auto&
    {
        return getParam(apvts, params, name);
    };
    
    auto& gainInParam = getParamHelper(Names::Gain_In);
    auto& lowMidParam = getParamHelper(Names::Low_Mid_Crossover_Freq);
    auto& midHighParam = getParamHelper(Names::Mid_High_Crossover_Freq);
    auto& gainOutParam = getParamHelper(Names::Gain_Out);
    
    inGainSlider = std::make_unique<RSWL>(&gainInParam, "dB", "INPUT TRIM");
    lowMidXoverSlider = std::make_unique<RSWL>(&lowMidParam, "Hz", "LOW X MID");
    midHighXoverSlider = std::make_unique<RSWL>(&midHighParam, "Hz", "MID X HIGH");
    outGainSlider = std::make_unique<RSWL>(&gainOutParam, "dB", "OUTPUT TRIM");
    
    auto makeAttachmentHelper = [&apvts, &params](auto& attachment,
                                                  const auto& name,
                                                  auto& slider)
    {
        makeAttachment(attachment, apvts, params, name, slider);
    };
    
    makeAttachmentHelper(inGainSliderAttachment,
                         Names::Gain_In,
                         *inGainSlider);
    makeAttachmentHelper(lowMidXoverSliderAttachment,
                         Names::Low_Mid_Crossover_Freq,
                         *lowMidXoverSlider);
    makeAttachmentHelper(midHighXoverSliderAttachment,
                         Names::Mid_High_Crossover_Freq,
                         *midHighXoverSlider);
    makeAttachmentHelper(outGainSliderAttachment,
                         Names::Gain_Out,
                         *outGainSlider);
    
    addLabelPairs(inGainSlider->labels, gainInParam, "dB");
    addLabelPairs(lowMidXoverSlider->labels, lowMidParam, "Hz");
    addLabelPairs(midHighXoverSlider->labels, midHighParam, "Hz");
    addLabelPairs(outGainSlider->labels, gainOutParam, "dB");
    
    addAndMakeVisible(*inGainSlider);
    addAndMakeVisible(*lowMidXoverSlider);
    addAndMakeVisible(*midHighXoverSlider);
    addAndMakeVisible(*outGainSlider);
};

void GlobalControls::paint(juce::Graphics &g)
{
    
    auto bounds = getLocalBounds();
    drawModuleBackground(g, bounds);
}

void GlobalControls::resized()
{
    using namespace juce;
    
    auto bounds = getLocalBounds();
    
    FlexBox flexBox;
    flexBox.flexDirection = FlexBox::Direction::row;
    flexBox.flexWrap = FlexBox::Wrap::noWrap;
    
    auto spacer = FlexItem().withWidth(0);
    auto endCap = FlexItem().withWidth(0);
    
    flexBox.items.add(endCap);
    flexBox.items.add(FlexItem(*inGainSlider).withFlex(1.0f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(*lowMidXoverSlider).withFlex(1.0f));
    flexBox.items.add(FlexItem(*midHighXoverSlider).withFlex(1.0f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(*outGainSlider).withFlex(1.0f));
    flexBox.items.add(endCap);
    
    flexBox.performLayout(bounds);
}
