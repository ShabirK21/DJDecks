/*
  ==============================================================================

    DeckGUI.h
    Created: 2 Feb 2023 6:59:59pm
    Author:  shabirk

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <memory>
#include "DJAudioPlayer.h"
#include "WaveFormDisplay.h"
//==============================================================================
/*
*/
class DeckGUI  : public juce::Component, public juce::Button::Listener,
public juce::Slider::Listener, public juce::FileDragAndDropTarget,
public juce::Timer, public juce::LookAndFeel_V4, public juce::TableListBoxModel
{
public:
    DeckGUI(DJAudioPlayer* player, juce::AudioFormatManager & formatManagerToUse,
            juce::AudioThumbnailCache & cacheToUse);
    ~DeckGUI() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    /**Implement button event listener*/
    void buttonClicked(juce::Button* button) override;

    /**Implement slider event listener*/
    void sliderValueChanged (juce::Slider * slider) override;

    bool isInterestedInFileDrag(const juce::StringArray &files) override;
    void filesDropped (const juce::StringArray &files, int x, int y) override;

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override;

    void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                               bool, bool isButtonDown) override;
    void drawButtonText (juce::Graphics& g, juce::TextButton& button, bool isMouseOverButton, bool isButtonDown) override;

    void timerCallback() override;

    /** Audio queue visuals*/
    int getNumRows() override;
    void paintRowBackground (juce::Graphics&, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(juce::Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    /** Manual dequeue*/
    void dequeue();

    /** Loads audio from playlist when called from playlist*/
    void loadFromPlaylist(juce::URL audioURL);
    /** Update current audio status bar*/
    void updateCurrentAudio(juce::String currentAudioName);
    /** Update waveform to currently playing audio*/
    void updateWaveFormDisplay(juce::URL currentWaveForm);

private:

    // Audio control buttons and their corresponding images
    juce::ImageButton playButton{"Play"};
    juce::ImageButton stopButton{"Stop"};
    juce::ImageButton pauseButton{"Pause"};
    juce::ImageButton forwardButton{"forward"};
    juce::ImageButton backwardButton{"forward"};
    juce::ImageButton loopButton{"loop"};
    juce::Image playIcon;
    juce::Image stopIcon;
    juce::Image pauseIcon;
    juce::Image forwardIcon;
    juce::Image backwardIcon;
    juce::Image loopIcon;

    // Load button for direct track load to player
    juce::TextButton loadButton{"Load"};
    // Dequeue button for manual dequeue of audio
    juce::TextButton dequeueButton{"Dequeue"};

    // Sliders for track control
    juce::Slider volSlider{"volSlider"};
    juce::Slider speedSlider{"speedSlider"};

    juce::FileChooser fChooser{"Select a file..."};

    DJAudioPlayer* player;
    WaveFormDisplay waveDisplay;
    juce::TableListBox queueBox;

    // List tracking if audio file being loaded is first, to load others into queue
    std::vector<juce::URL> QList;
    // Loop control
    bool shouldILoop = false;
    // List of queued track files
    std::vector<juce::URL> queuedAudioList;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckGUI)
};