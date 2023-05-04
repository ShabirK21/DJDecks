/*
  ==============================================================================

    DeckGUI.cpp
    Created: 2 Feb 2023 6:59:59pm
    Author:  shabirk

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DeckGUI.h"
//==============================================================================
DeckGUI::DeckGUI(DJAudioPlayer* _player,juce::AudioFormatManager & formatManagerToUse,
                 juce::AudioThumbnailCache & cacheToUse) : player(_player),
                 waveDisplay(formatManagerToUse, cacheToUse)
{

    playIcon = juce::ImageCache::getFromMemory(BinaryData::play_png, BinaryData::play_pngSize);
    stopIcon = juce::ImageCache::getFromMemory(BinaryData::stop_png, BinaryData::stop_pngSize);
    forwardIcon = juce::ImageCache::getFromMemory(BinaryData::forward_png, BinaryData::forward_pngSize);
    backwardIcon = juce::ImageCache::getFromMemory(BinaryData::backward_png, BinaryData::backward_pngSize);
    pauseIcon = juce::ImageCache::getFromMemory(BinaryData::pause_png, BinaryData::pause_pngSize);
    loopIcon = juce::ImageCache::getFromMemory(BinaryData::loop_png, BinaryData::loop_pngSize);

    playButton.setImages(false, true, true, playIcon, 1.0f, juce::Colours::transparentBlack, playIcon, 1.0f, juce::Colours::transparentBlack, playIcon, 1.0f, juce::Colours::transparentBlack);
    stopButton.setImages(false, true, true, stopIcon, 1.0f, juce::Colours::transparentBlack, stopIcon, 1.0f, juce::Colours::transparentBlack, stopIcon, 1.0f, juce::Colours::transparentBlack);
    pauseButton.setImages(false, true, true, pauseIcon, 1.0f, juce::Colours::transparentBlack, pauseIcon, 1.0f, juce::Colours::transparentBlack, pauseIcon, 1.0f, juce::Colours::transparentBlack);
    forwardButton.setImages(false, true, true, forwardIcon, 1.0f, juce::Colours::transparentBlack, forwardIcon, 1.0f, juce::Colours::transparentBlack, forwardIcon, 1.0f, juce::Colours::transparentBlack);
    backwardButton.setImages(false, true, true, backwardIcon, 1.0f, juce::Colours::transparentBlack, backwardIcon, 1.0f, juce::Colours::transparentBlack, backwardIcon, 1.0f, juce::Colours::transparentBlack);
    loopButton.setImages(false, true, true, loopIcon, 1.0f, juce::Colours::transparentBlack, loopIcon, 1.0f, juce::Colours::transparentBlack, loopIcon, 1.0f, juce::Colours::transparentBlack);


    volSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    volSlider.setTextBoxStyle(juce::Slider::TextBoxLeft,false,70,20);

    speedSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxRight,false,70,20);

    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(pauseButton);
    addAndMakeVisible(forwardButton);
    addAndMakeVisible(backwardButton);
    addAndMakeVisible(loopButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(queueBox);
    addAndMakeVisible(dequeueButton);
    addAndMakeVisible(volSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(waveDisplay);

    playButton.addListener(this);
    stopButton.addListener(this);
    pauseButton.addListener(this);
    backwardButton.addListener(this);
    forwardButton.addListener(this);
    loopButton.addListener(this);
    loadButton.addListener(this);
    volSlider.addListener(this);
    speedSlider.addListener(this);
    dequeueButton.addListener(this);

    volSlider.setRange(0.0,1.0);
    speedSlider.setRange(0.0,100.0);
    loadButton.setButtonText("Load to deck");

    // Audio queue box
    queueBox.getHeader().addColumn("",0,200);
    queueBox.setModel(this);

    startTimer(100);
}

DeckGUI::~DeckGUI()
{
    stopTimer();
}

void DeckGUI::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    g.fillAll(juce::Colour::fromRGB(30,30,46));

    g.setColour (juce::Colours::black);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("DeckGUI", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void DeckGUI::resized()
{
    double rowH = getHeight() / 8;
    volSlider.setBounds(10,0,getWidth()/2,rowH*3);
    speedSlider.setBounds(getWidth()/2-10,0,getWidth()/2,rowH*3);
    waveDisplay.setBounds(0,rowH*3,getWidth(),rowH*2);
    stopButton.setBounds(getWidth()-(stopIcon.getWidth()/7)-20,rowH*5+10,stopIcon.getWidth()/7,stopIcon.getHeight()/7);
    pauseButton.setBounds(getWidth()-(pauseIcon.getWidth()/7)-70,rowH*5+10,pauseIcon.getWidth()/7,pauseIcon.getHeight()/7);
    playButton.setBounds(getWidth()-(playIcon.getWidth()/14)-120,rowH*5+10,playIcon.getWidth()/14,playIcon.getHeight()/14);
    loadButton.setBounds(155,rowH*5+10,80,50);
    loopButton.setBounds(getWidth()-(loopIcon.getWidth()/14)-20,rowH*7-10,loopIcon.getWidth()/14,loopIcon.getWidth()/14);
    forwardButton.setBounds(getWidth()-(forwardIcon.getWidth()/14)-70,rowH*7-10,forwardIcon.getWidth()/14,forwardIcon.getWidth()/14);
    backwardButton.setBounds(getWidth()-(backwardIcon.getWidth()/14)-120,rowH*7-10,backwardIcon.getWidth()/14,backwardIcon.getWidth()/14);
    queueBox.setBounds(0,(rowH*5),150,getHeight());
    dequeueButton.setBounds(155,(rowH*7),80,30);
}

int DeckGUI::getNumRows(){
    return queuedAudioList.size();
}

void DeckGUI::paintRowBackground (juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected){
    if(rowIsSelected){
        g.fillAll(juce::Colour::fromRGB(249, 226, 175));
    }
    else{
        g.fillAll(juce::Colour::fromRGB(116, 199, 236));
    }
}

void DeckGUI::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) {
    if (columnId == 0) {
        g.drawText(queuedAudioList[rowNumber].getFileName(), 2, 0, width - 4, height, juce::Justification::centredLeft, true);
    }
}

void DeckGUI::buttonClicked(juce::Button * button) {
    if(button == &playButton){
        std::cout << "play clicked" << std::endl;
        player->start();
    }
    else if (button == &stopButton){
        std::cout << "stop clicked" << std::endl;
        player->stop();
    }
    else if(button == &loadButton){

        auto fileChooserFlags =
                juce::FileBrowserComponent::canSelectFiles;

        fChooser.launchAsync(fileChooserFlags, [this](const juce::FileChooser& chooser) {
            juce::File chosenFile = chooser.getResult();
            juce::URL audioURL{chosenFile};
            if (QList.empty()){
                player->loadURL(audioURL);
                waveDisplay.loadURL(audioURL);
                QList.emplace_back(audioURL);
            }
            else{queuedAudioList.emplace_back(audioURL);
                player->enqueue(audioURL);
                queueBox.updateContent();}
        });
    }
    else if(button == &forwardButton){
        std::cout << "move forward 10" << std::endl;
        player->moveForward();
    }
    else if(button == &backwardButton){
        std::cout << "move backward 10" << std::endl;
        player->moveBackward();
    }
    else if(button == &pauseButton){
        std::cout << "pause" << std::endl;
        player->pause();
    }
    else if(button == &loopButton){
        shouldILoop = !shouldILoop;
        player->loop(shouldILoop);
        std::cout << "loop " << shouldILoop <<std::endl;
    }
    else if(button == &dequeueButton){
        if(queueBox.getNumRows()>0) {
            int id = queueBox.getSelectedRow();
            queuedAudioList.erase(queuedAudioList.cbegin() + id);
            queueBox.updateContent();
            player->dequeue(id);
        }
    }
}

void DeckGUI::sliderValueChanged(juce::Slider *slider) {
    if(slider == &volSlider){
        player->setGain(slider->getValue());
    }
    if (slider == & speedSlider){
        player->setSpeed(slider->getValue());
    }
}

bool DeckGUI::isInterestedInFileDrag(const juce::StringArray &files){
    std::cout << "DeckGUI::isInterestedInFileDrag" << std::endl;
    return true;
}
void DeckGUI::filesDropped (const juce::StringArray &files, int x, int y) {
    std::cout << "DeckGUI::filesDropped" << std::endl;
    if (files.size() == 1){
        juce::URL audioFile{juce::File{files[0]}};
        player->loadURL(audioFile);
        waveDisplay.loadURL(audioFile);
    }
}

void DeckGUI::loadFromPlaylist(juce::URL audioURL) {

   if(QList.empty()){
        player->loadURL(audioURL);
        waveDisplay.loadURL(audioURL);
        player->start();
        QList.emplace_back(audioURL);
    }
   else {
        queuedAudioList.emplace_back(audioURL);
        player->enqueue(audioURL);
        queueBox.updateContent();
    }
}

void DeckGUI::timerCallback() {
    waveDisplay.setPositionRelative(player->getPositionRelative());
}

void DeckGUI::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                       const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)
                       {
                           auto radius = (float) juce::jmin (width / 2, height / 2) - 4.0f;
                           auto centreX = (float) x + (float) width  * 0.5f;
                           auto centreY = (float) y + (float) height * 0.5f;
                           auto rx = centreX - radius;
                           auto ry = centreY - radius;
                           auto rw = radius * 2.0f;
                           auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

                           // fill
                           g.setColour (juce::Colour::fromRGB(137, 180, 250));
                           g.fillEllipse (rx, ry, rw, rw);

                           // outline
                           g.setColour (juce::Colour::fromRGB(17, 17, 27));
                           g.drawEllipse (rx, ry, rw, rw, 1.0f);

                           //text
                           juce::String text;
                            if(slider.getName() == "volSlider") {
                                text = "Volume";
                            }
                            if (slider.getName() == "speedSlider"){
                                text = "Speed";
                            }

                           g.setFont(juce::Font("Arial", height * 0.15f, juce::Font::bold));
                           float textWidth = g.getCurrentFont().getStringWidth(text);
                           g.drawText(text, x + width / 2 - textWidth / 2, y + height / 2 - g.getCurrentFont().getHeight() / 2,
                                      textWidth, g.getCurrentFont().getHeight(),
                                      juce::Justification::centred);

                           // pointer
                           juce::Path p;
                           auto pointerLength = radius * 0.33f;
                           auto pointerThickness = 2.0f;
                           p.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
                           p.applyTransform (juce::AffineTransform::rotation (angle).translated (centreX, centreY));
                           g.setColour (juce::Colours::yellow);
                           g.fillPath (p);
                       }

void DeckGUI::drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                           bool, bool isButtonDown){
    auto buttonArea = button.getLocalBounds();
    auto edge = 4;

    buttonArea.removeFromLeft (edge);
    buttonArea.removeFromTop (edge);

    // shadow
    g.setColour (juce::Colours::darkgrey.withAlpha (0.5f));
    g.fillRect (buttonArea);

    auto offset = isButtonDown ? -edge / 2 : -edge;
    buttonArea.translate (offset, offset);

    g.setColour (backgroundColour);
    g.fillRect (buttonArea);
}

void DeckGUI::drawButtonText (juce::Graphics& g, juce::TextButton& button, bool isMouseOverButton, bool isButtonDown){
    auto font = getTextButtonFont (button, button.getHeight());
    g.setFont (font);
    g.setColour (button.findColour (button.getToggleState() ? juce::TextButton::textColourOnId
                                                            : juce::TextButton::textColourOffId)
                         .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));

    auto yIndent = juce::jmin (4, button.proportionOfHeight (0.3f));
    auto cornerSize = juce::jmin (button.getHeight(), button.getWidth()) / 2;

    auto fontHeight = juce::roundToInt (font.getHeight() * 0.6f);
    auto leftIndent  = juce::jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnLeft()  ? 4 : 2));
    auto rightIndent = juce::jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
    auto textWidth = button.getWidth() - leftIndent - rightIndent;

    auto edge = 4;
    auto offset = isButtonDown ? edge / 2 : 0;

    if (textWidth > 0)
        g.drawFittedText (button.getButtonText(),
                          leftIndent + offset, yIndent + offset, textWidth, button.getHeight() - yIndent * 2 - edge,
                          juce::Justification::centred, 2);
}

void DeckGUI::dequeue(){
    if(!queuedAudioList.empty()) {
        queuedAudioList.erase(queuedAudioList.cbegin());
        queueBox.updateContent();
    }
}

void DeckGUI::updateCurrentAudio(juce::String currentAudioName){
        queueBox.getHeader().setColumnName(0, currentAudioName);
}

void DeckGUI::updateWaveFormDisplay(juce::URL currentWaveForm){

        waveDisplay.loadURL(currentWaveForm);

}