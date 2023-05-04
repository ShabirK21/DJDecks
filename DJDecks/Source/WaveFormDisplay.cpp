/*
  ==============================================================================

    WaveFormDisplay.cpp
    Created: 6 Feb 2023 9:30:42am
    Author:  shabirk

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WaveFormDisplay.h"
#include <random>
//==============================================================================
WaveFormDisplay::WaveFormDisplay(juce::AudioFormatManager & formatManagerToUse,
                                 juce::AudioThumbnailCache & cacheToUse) :
                                 audioThumbnail(1000,formatManagerToUse,cacheToUse),
                                 fileLoaded(false), position(0)
{
    audioThumbnail.addChangeListener(this);
}

WaveFormDisplay::~WaveFormDisplay()
{
}

void WaveFormDisplay::paint (juce::Graphics& g) {

     g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

     g.fillAll(juce::Colour::fromRGB(30,30,46));
     g.setColour (juce::Colours::black);
     g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

     // generate 3 random numbers to dynamically change the color of the waveform
     for(int i = 0; i < audioThumbnail.getTotalLength(); ++i) {
         std::random_device rd;
         std::mt19937 gen(rd());
         std::uniform_int_distribution<> dis(0, 255);
         int random_number1 = dis(gen);
         int random_number2 = dis(gen);
         int random_number3 = dis(gen);
         g.setColour(juce::Colour::fromRGB(random_number1, random_number2, random_number3));
     }
     if(fileLoaded){
         audioThumbnail.drawChannel(g,getLocalBounds(),0,audioThumbnail.getTotalLength(),0,1.0f);

         g.setColour(juce::Colour::fromRGB(243, 139, 168));
         g.drawRect(position * getWidth(), 0, getWidth()/25, getHeight());
     }
     else{
         g.setColour(juce::Colour::fromRGB(205, 214, 244));
         g.setFont (20.0f);
         g.drawText ("File not loaded yet...", getLocalBounds(),
                     juce::Justification::centred, true);
     }
}

void WaveFormDisplay::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void WaveFormDisplay::loadURL(juce::URL audioURL) {
    std::cout << "WaveFormDisplay::loadURL" << std::endl;
    audioThumbnail.clear();
    fileLoaded = audioThumbnail.setSource(new juce::URLInputSource(audioURL));
}

void WaveFormDisplay::changeListenerCallback(juce::ChangeBroadcaster *source) {
    std::cout << "Changed" << std::endl;
    repaint();
}

void WaveFormDisplay::setPositionRelative(double pos) {
    if(pos != position){
        position = pos;
        repaint();
    }
}


