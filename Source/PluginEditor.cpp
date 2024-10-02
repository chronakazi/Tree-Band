/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

template<typename T>
bool truncateKiloValue(T& value)
{
    if (value > static_cast<T>(999))
    {
        value /= static_cast<T>(1000);
        return true;
    }
    
    return false;
}

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
    g.setColour(enabled ? Colour(ColorPalette::Accent) : Colour(ColorPalette::Secondary));
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
        
        g.setColour(enabled ? Colour(ColorPalette::Accent) : Colour(ColorPalette::Secondary));
        g.fillRect(r);
        
        g.setColour(enabled ? Colour(ColorPalette::Primary) : Colour(ColorPalette::Accent));
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
}

//====================================================================================
void RotarySliderWithLabels::paint(juce::Graphics &g)
{
    using namespace juce;
    auto startAng = degreesToRadians(180.f + 45.f);
    auto endAng = degreesToRadians(180.f - 45.f) + MathConstants<float>::twoPi;
    
    auto range = getRange();
    
    auto sliderBounds = getSliderBounds();
    
    auto bounds = getLocalBounds();
//    parameter labels
    g.setColour(Colour(ColorPalette::Tertiary));
    g.drawText(getName(),
               bounds.removeFromTop(getTextHeight() + 5),
               Justification::centredBottom);
    
//    bounding box guides
//    g.setColour(Colours::red);
//    g.drawRect(getLocalBounds());
//    g.setColour(Colours::yellow);
//    g.drawRect(sliderBounds);
    
    getLookAndFeel().drawRotarySlider(g,
                                      sliderBounds.getX(),
                                      sliderBounds.getY(),
                                      sliderBounds.getWidth(),
                                      sliderBounds.getHeight(),
                                      jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0),
                                      startAng, endAng, *this);
    
//    perimeter labels
    auto center = sliderBounds.toFloat().getCentre();
    auto radius = sliderBounds.getWidth() * .5f;
    
    g.setColour(Colour(ColorPalette::Tertiary));
    g.setFont(getTextHeight());
    
    auto numChoices = labels.size();
    for (int i = 0; i < numChoices; ++i)
    {
        auto pos = labels[i].pos;
        jassert(0.f <= pos);
        jassert(pos <= 1.f);
        
        auto ang = jmap(pos, 0.f, 1.f, startAng, endAng);
        
        auto c = center.getPointOnCircumference(radius + getTextHeight() * 0.5f + 1, ang);
        
        Rectangle<float> r;
        auto str = labels[i].label;
        r.setSize(GlyphArrangement::getStringWidth(g.getCurrentFont(), str), getTextHeight());
        r.setCentre(c);
        r.setY(r.getY() + getTextHeight());
        
        g.drawFittedText(str, r.toNearestInt(), juce::Justification::centred, 1);
    }
}

juce::Rectangle<int> RotarySliderWithLabels::getSliderBounds() const
{
    auto bounds = getLocalBounds();
    
    bounds.removeFromTop(getTextHeight());
    
    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());
    
    size -= getTextHeight() * 2;
    juce::Rectangle<int> r;
    r.setSize(size, size);
    r.setCentre(bounds.getCentreX(), 0);
//    r.setY(10);
    r.setY(bounds.getY() + 8);
    
    return r;
}

juce::String RotarySliderWithLabels::getDisplayString() const
{
    if (auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param))
        return choiceParam->getCurrentChoiceName();
    
    juce::String str;
    bool addK = false;
    
    if (auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param))
    {
        float val = getValue();
//        if (val > 999.f)
//        {
//            val /= 1000.f;
//            addK = true;
//        }
        
        addK = truncateKiloValue(val);
        
//        TODO Fix str skipping -0.1
        if (abs(val) < 0.1f)
        {
            val = 0;
        }
        
        str = juce::String(val, (addK ? 2 : 0));
    }
    else
    {
        jassertfalse; //should never happen
    }
    
    if (suffix.isNotEmpty())
    {
        str << " ";
        if (addK)
            str << "k";
        
        str << suffix;
    }
    
    return str;
    
}

void RotarySliderWithLabels::changeParam(juce::RangedAudioParameter *p)
{
    param = p;
    repaint();
}
//==============================================================================

juce::String RatioSlider::getDisplayString() const
{
    auto choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param);
    jassert(choiceParam != nullptr);
    
    auto currentChoice = choiceParam->getCurrentChoiceName();
    if (currentChoice.contains(".0"))
        currentChoice = currentChoice.substring(0, currentChoice.indexOf("."));
    
    currentChoice << ":1";
    
    return currentChoice;
}

//==============================================================================
Placeholder::Placeholder()
{
    juce::Random r;
    customColor = juce::Colour(r.nextInt(255), r.nextInt(255), r.nextInt(255));
}

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

//==============================================================================

CompressorBandControls::CompressorBandControls(juce::AudioProcessorValueTreeState& apv) :
apvts(apv),
thresholdSlider(nullptr, "dB", "THRESHOLD"),
attackSlider(nullptr, "ms", "ATTACK"),
releaseSlider(nullptr, "ms", "RELEASE"),
ratioSlider(nullptr, "")
{
    using namespace Params;
    const auto& params = GetParams();
    
    auto getParamHelper = [&apvts = this->apvts, &params](const auto& name) -> auto&
    {
        return getParam(apvts, params, name);
    };
    
    thresholdSlider.changeParam(&getParamHelper(Names::Threshold_Mid_Band));
    ratioSlider.changeParam(&getParamHelper(Names::Ratio_Mid_Band));
    attackSlider.changeParam(&getParamHelper(Names::Attack_Mid_Band));
    releaseSlider.changeParam(&getParamHelper(Names::Release_Mid_Band));
    
    addLabelPairs(thresholdSlider.labels, getParamHelper(Names::Threshold_Mid_Band), "dB");
    addLabelPairs(attackSlider.labels, getParamHelper(Names::Attack_Mid_Band), "ms");
    addLabelPairs(releaseSlider.labels, getParamHelper(Names::Release_Mid_Band), "ms");
    
    ratioSlider.labels.add({0.0f, "1:1"});
    auto ratioParam = dynamic_cast<juce::AudioParameterChoice*>(&getParamHelper(Names::Ratio_Mid_Band));
    ratioSlider.labels.add({1.0f,
        juce::String(ratioParam->choices.getReference(ratioParam->choices.size() - 1).getIntValue()) + ":1"});
    
    auto makeAttachmentHelper = [&apvts = this->apvts, &params](auto& attachment,
                                                  const auto& name,
                                                  auto& slider)
    {
        makeAttachment(attachment, apvts, params, name, slider);
    };
    
    makeAttachmentHelper(thresholdSliderAttachment,
                         Names::Threshold_Mid_Band,
                         thresholdSlider);
    makeAttachmentHelper(ratioSliderAttachment,
                         Names::Ratio_Mid_Band,
                         ratioSlider);
    makeAttachmentHelper(attackSliderAttachment,
                         Names::Attack_Mid_Band,
                         attackSlider);
    makeAttachmentHelper(releaseSliderAttachment,
                         Names::Release_Mid_Band,
                         releaseSlider);
    
    addAndMakeVisible(thresholdSlider);
    addAndMakeVisible(ratioSlider);
    addAndMakeVisible(attackSlider);
    addAndMakeVisible(releaseSlider);
}

void CompressorBandControls::resized()
{
    using namespace juce;
    
    auto bounds = getLocalBounds();
    
    FlexBox flexBox;
    flexBox.flexDirection = FlexBox::Direction::row;
    flexBox.flexWrap = FlexBox::Wrap::noWrap;
    
    auto endCap = FlexItem().withWidth(32);
    
    flexBox.items.add(endCap);
    flexBox.items.add(FlexItem(thresholdSlider).withFlex(1.0f));
    flexBox.items.add(FlexItem(ratioSlider).withFlex(1.0f));
    flexBox.items.add(FlexItem(attackSlider).withFlex(1.0f));
    flexBox.items.add(FlexItem(releaseSlider).withFlex(1.0f));
    flexBox.items.add(endCap);
    
    flexBox.performLayout(bounds);
}

void CompressorBandControls::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds();
    drawModuleBackground(g, bounds);
}
//==============================================================================

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
}

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
    
    auto spacer = FlexItem().withWidth(32);
    auto endCap = FlexItem().withWidth(32);
    
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

//==============================================================================
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
