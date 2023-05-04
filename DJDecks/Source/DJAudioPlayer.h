/*
  ==============================================================================

    DJAudioPlayer.h
    Created: 5 Feb 2023 11:10:14pm
    Author:  shabirk

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h>
#include "WaveFormDisplay.h"
class DJAudioPlayer : public juce::AudioSource
        {
public:

    DJAudioPlayer(juce::AudioFormatManager& _formatManager);
    ~DJAudioPlayer();

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    /** Load track into audio player*/
    void loadURL(juce::URL audioURL);
    /** Set volume of track*/
    void setGain(double gain);
    /** Set speed of track*/
    void setSpeed(double ratio);

    /** Playback controls of track*/
    void start();
    void stop();
    void pause();
    void moveForward();
    void moveBackward();
    void loop(bool loopA);
    /** Audio queue controls*/
    void enqueue(juce::URL audioFile);
    void dequeue(int id);
    // Keep track of current track for status bar
    juce::String  currentAudioName();
    juce::URL currentWaveForm();

    /** get relative position of playhead*/
    double getPositionRelative();

    /** Loads next file in audio queue and removes it from queue*/
    bool setUpAudioQueue();

private:
    juce::AudioFormatManager& formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    juce::ResamplingAudioSource resamplingAudioSource{&transportSource,false,2};
    bool loopAudio;
    std::vector<juce::URL> queueList;
    juce::URL currentAudio;
        };

