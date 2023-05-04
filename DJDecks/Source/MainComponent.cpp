#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (0, 2);
    }

    addAndMakeVisible(deckGui1);
    addAndMakeVisible(deckGui2);
    addAndMakeVisible(playlist);

    juce::LookAndFeel_V4::setDefaultLookAndFeel(&deckGui1);

    formatManager.registerBasicFormats();
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected,sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected,sampleRate);

    mixerSource.prepareToPlay(samplesPerBlockExpected,sampleRate);

    mixerSource.addInputSource(&player1,false);
    mixerSource.addInputSource(&player2,false);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    mixerSource.getNextAudioBlock(bufferToFill);
    setUpAudioQueue();
    updateCurrentAudio();
}

void MainComponent::releaseResources()
{
    player1.releaseResources();
    player2.releaseResources();
    mixerSource.releaseResources();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    deckGui1.setBounds(0,0,getWidth()/2,getHeight()/2);
    deckGui2.setBounds(getWidth()/2,0,getWidth()/2,getHeight()/2);
    playlist.setBounds(0,getHeight()/2,getWidth(),getHeight()/2);
}

// When new audio file is loaded into deck, update waveformdisplay
void MainComponent::setUpAudioQueue() {
    if(player1.setUpAudioQueue()){
        deckGui1.updateWaveFormDisplay(player1.currentWaveForm());
        deckGui1.dequeue();
    }
    if(player2.setUpAudioQueue()){
        deckGui2.updateWaveFormDisplay(player2.currentWaveForm());
        deckGui2.dequeue();
    }
}

// When new audio file is loaded into deck, update current audio status bar
void MainComponent::updateCurrentAudio() {
    if(player1.currentAudioName().isEmpty()){
        deckGui1.updateCurrentAudio("No audio playing");
    }
    if(player2.currentAudioName().isEmpty()){
        deckGui2.updateCurrentAudio("No audio playing");
    }
    if(!player1.currentAudioName().isEmpty()){
        deckGui1.updateCurrentAudio(player1.currentAudioName());
    }
    if(!player2.currentAudioName().isEmpty()){
        deckGui2.updateCurrentAudio(player2.currentAudioName());
    }
}
