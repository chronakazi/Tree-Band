/*
  ==============================================================================

    Utilities.h
    Created: 22 Oct 2024 5:28:00pm
    Author:  George Brewster

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

template<
    typename Attachment,
    typename APVTS,
    typename Params,
    typename ParamName,
    typename SliderType
    >
void makeAttachment(std::unique_ptr<Attachment>& attachment,
                    APVTS& apvts,
                    const Params& params,
                    const ParamName& name,
                    SliderType& slider)
{
    attachment = std::make_unique<Attachment>(apvts,
                                              params.at(name),
                                              slider);
}

template<typename APVTS, typename Params, typename ParamName>
juce::RangedAudioParameter& getParam(APVTS& apvts, const Params& params, const ParamName& name)
{
    auto param = apvts.getParameter(params.at(name));
    jassert(param != nullptr);
    
    return *param;
}

juce::String getValString(const juce::RangedAudioParameter& param, bool getLow, juce::String suffix);

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

template<typename Labels, typename ParamType, typename SuffixType>
void addLabelPairs(Labels& labels, const ParamType& param, const SuffixType& suffix)
{
    labels.clear();
    labels.add({0.0f, getValString(param, true, suffix)});
    labels.add({1.0f, getValString(param, false, suffix)});
}

void drawModuleBackground(juce::Graphics &g,
                          juce::Rectangle<int> bounds);
