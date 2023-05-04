/*
  ==============================================================================

    WaveFormDisplay.h
    Created: 6 Feb 2023 9:30:42am
    Author:  shabirk

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class WaveFormDisplay  : public juce::Component, public juce::ChangeListener
{
public:
    WaveFormDisplay(juce::AudioFormatManager & formatManagerToUse,
                    juce::AudioThumbnailCache & cacheToUse);
    ~WaveFormDisplay() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    /** Load track into waveform*/
    void loadURL(juce::URL audioURL);

    void changeListenerCallback(juce::ChangeBroadcaster *source) override;

    /** set the relative position of the playhead*/
    void setPositionRelative(double pos);

private:
    juce::AudioThumbnail audioThumbnail;
    bool fileLoaded;
    double position;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveFormDisplay)
};
