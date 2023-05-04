/*
  ==============================================================================

    DJAudioPlayer.cpp
    Created: 5 Feb 2023 11:10:14pm
    Author:  shabirk

  ==============================================================================
*/

#include "DJAudioPlayer.h"

DJAudioPlayer::DJAudioPlayer(juce::AudioFormatManager &_formatManager)
    : formatManager(_formatManager) {
}
DJAudioPlayer::~DJAudioPlayer() {}

void DJAudioPlayer::prepareToPlay(int samplesPerBlockExpected,
                                  double sampleRate) {
  transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
  resamplingAudioSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
void DJAudioPlayer::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    resamplingAudioSource.getNextAudioBlock(bufferToFill);

    // loop current audio when loopAudio is set to true
    while(loopAudio && transportSource.hasStreamFinished()){
        transportSource.setPosition(0.0);
        transportSource.start();
    }

}

void DJAudioPlayer::releaseResources() {
  resamplingAudioSource.releaseResources();
}

void DJAudioPlayer::loadURL(juce::URL audioURL) {
  auto *reader =
      formatManager.createReaderFor(audioURL.createInputStream(false));
  if (reader != nullptr) {
    std::unique_ptr<juce::AudioFormatReaderSource> newSource(
        new juce::AudioFormatReaderSource(reader, true));
    transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
    readerSource.reset(newSource.release());
  }
  currentAudio = audioURL;
}
void DJAudioPlayer::setGain(double gain) {
  if (gain < 0 || gain > 1.0) {
    std::cout << "DJAudioPlayer::setGain gain should be between 0 and 1"
              << std::endl;
  } else {
    transportSource.setGain(gain);
  }
}
void DJAudioPlayer::setSpeed(double ratio) {
  if (ratio < 0 || ratio > 100) {
    std::cout << "DJAudioPlayer::setSpeed ratio should be between 0 and 100"
              << std::endl;
  } else {
    resamplingAudioSource.setResamplingRatio(ratio);
  }
}

void DJAudioPlayer::start() { transportSource.start(); }
void DJAudioPlayer::stop() {
  transportSource.stop();
  transportSource.setPosition(0.0);
}

void DJAudioPlayer::pause() { transportSource.stop(); }

double DJAudioPlayer::getPositionRelative() {
  return transportSource.getCurrentPosition() /
         transportSource.getLengthInSeconds();
}

void DJAudioPlayer::moveForward() {
  double seconds = transportSource.getLengthInSeconds() * 0.05;
  double newPos = transportSource.getCurrentPosition() + seconds;
  transportSource.setPosition(newPos);
}
void DJAudioPlayer::moveBackward() {
  double seconds = transportSource.getLengthInSeconds() * 0.05;
  double newPos = transportSource.getCurrentPosition() - seconds;
  if (newPos < 0.0) {
    newPos = 0.0;
  }
  transportSource.setPosition(newPos);
}

void DJAudioPlayer::loop(bool loopA) {
    loopAudio = loopA;
}

void DJAudioPlayer::enqueue(juce::URL audioFile){
    queueList.emplace_back(audioFile);
}

void DJAudioPlayer::dequeue(int id){
    queueList.erase(queueList.cbegin()+id);
}

juce::String DJAudioPlayer::currentAudioName(){
    if(transportSource.isPlaying()) {
        return currentAudio.getFileName();
    }
    else{
        return "";
    }
}

juce::URL DJAudioPlayer::currentWaveForm(){
    if(transportSource.isPlaying()){
        return currentAudio;
    }
}

bool DJAudioPlayer::setUpAudioQueue(){
    while(!queueList.empty() && transportSource.hasStreamFinished()){
        loadURL(queueList[0]);
        transportSource.start();
        queueList.erase(queueList.begin());
        return true;
    }
    return false;
}