/*
  ==============================================================================

    PlaylistComponent.h
    Created: 11 Feb 2023 12:14:39pm
    Author:  shabirk

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "DeckGUI.h"
//==============================================================================
/*
*/
class PlaylistComponent  : public juce::Component, public juce::TableListBoxModel,
public juce::Button::Listener, public juce::TextEditor::Listener, public juce::FileDragAndDropTarget
{
public:
    PlaylistComponent(DeckGUI* deckGui1, DeckGUI* deckGui2);
    ~PlaylistComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    int getNumRows() override;
    void paintRowBackground (juce::Graphics&, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(juce::Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

    juce::Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowsSelected,
                                             juce::Component *existingComponentToUpdate) override;

    // Listeners for button and serach box
    void buttonClicked(juce::Button* button) override;
    void textEditorTextChanged(juce::TextEditor& editor) override;

    // Listeners for drag and drop feature
    bool isInterestedInFileDrag(const juce::StringArray &files) override;
    void filesDropped (const juce::StringArray &files, int x, int y) override;

    /** Add new tracks into playlist */
    void addTracks(const juce::URL &audioURL);
    /** Checkf for duplicate audio files being*/
    bool checkIfDuplicate(const std::string &trackTitle);
    /** Read text file containing previously loaded audio files*/
    void trackPersistence();
    /** Remove audio files when deleted*/
    void removeFromPersistence(std::string_view trackTitle);

private:

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;

    // Buttons for playlist controls
    juce::TextButton* deckButton1;
    juce::TextButton* deckButton2;
    juce::TextButton* deleteButton;
    juce::TextButton loadButton{"Load to playlist"};

    // Data being loaded into playlist
    juce::TableListBox tableComponent;
    std::vector<std::string> trackTitles;
    std::vector<std::string> trackLengths;
    std::vector<std::string> playlistTracks;
    std::vector<juce::URL> trackURLs;

    // Temporary vectors to modified and help track data changes
    std::vector<std::string> tempTitle;
    std::vector<std::string> tempTitlePersistence;

    // Pointers to decks
    DeckGUI* deckGui1;
    DeckGUI* deckGui2;

    juce::FileChooser fChooser{"Select a file..."};

    /** Calculate tracks length in seconds*/
    double trackStats(const juce::URL &audioURL);
    /** Check if audio file is in a accepted format*/
    bool isBasicAudioFormat(const juce::String& fileExtension);
    /** Write to text file for track persistence*/
    void writeToFile(const std::string &filePathName);

    // Search box
    juce::TextEditor searchBox;
    juce::String lastSearchText;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistComponent)
};
