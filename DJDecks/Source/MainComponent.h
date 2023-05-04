#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "DeckGUI.h"
#include "PlaylistComponent.h"
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

    std::string fileNames;
    // Updates DeckGUI queue when audio file is changed
    void setUpAudioQueue();
    void updateCurrentAudio();

private:

    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache{100};

    DJAudioPlayer player1{formatManager};
    DeckGUI deckGui1{&player1, formatManager, thumbnailCache};

    DJAudioPlayer player2{formatManager};
    DeckGUI deckGui2{&player2, formatManager, thumbnailCache};

    juce::MixerAudioSource mixerSource;

    PlaylistComponent playlist{&deckGui1,&deckGui2};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};