/*
  ==============================================================================

    PlaylistComponent.cpp
    Created: 11 Feb 2023 12:14:39pm
    Author:  shabirk

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PlaylistComponent.h"
//==============================================================================
PlaylistComponent::PlaylistComponent(DeckGUI* _deckGUI1, DeckGUI* _deckGUI2)
                                    : deckGui1(_deckGUI1), deckGui2(_deckGUI2)
{
    tableComponent.getHeader().addColumn("#",0,30);
    tableComponent.getHeader().addColumn("Track title",1,320);
    tableComponent.getHeader().addColumn("Duration (Seconds)",2,150);
    tableComponent.getHeader().addColumn("Deck 1",3,100);
    tableComponent.getHeader().addColumn("Deck 2",4,100);
    tableComponent.getHeader().addColumn("Delete", 5, 100);
    tableComponent.setModel(this);

    searchBox.setText("Search...");
    searchBox.setColour(juce::TextEditor::backgroundColourId, juce::Colour::fromRGB(127, 132, 156));

    addAndMakeVisible(tableComponent);
    addAndMakeVisible(searchBox);
    addAndMakeVisible(loadButton);
    searchBox.addListener(this);
    loadButton.addListener(this);
    trackPersistence();
}

PlaylistComponent::~PlaylistComponent()
{
}

void PlaylistComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    g.fillAll(juce::Colour::fromRGB(30,30,46));

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("PlaylistComponent", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void PlaylistComponent::resized()
{
    int rowH = getHeight()/11;
    tableComponent.setBounds(0,30,getWidth(),getHeight());
    searchBox.setBounds(0,0,getWidth()-150,rowH);
    loadButton.setBounds(getWidth()-150,0,150,rowH);
}
int PlaylistComponent::getNumRows(){
        return playlistTracks.size();
}
void PlaylistComponent::paintRowBackground (juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected){
    if(rowIsSelected){
        g.fillAll(juce::Colour::fromRGB(249, 226, 175));
    }
    else{
        g.fillAll(juce::Colour::fromRGB(116, 199, 236));
    }
}
void PlaylistComponent::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) {
    if (columnId == 0) {
        g.drawText(std::to_string(rowNumber + 1), 2, 0, width - 4, height, juce::Justification::centredLeft, true);

    }
    if (columnId == 1) {
        g.drawText(playlistTracks[rowNumber], 2, 0, width - 4, height, juce::Justification::centredLeft, true);
    }
    if (columnId == 2) {
        g.drawText(trackLengths[rowNumber], 2, 0, width - 4, height, juce::Justification::centredLeft, true);
    }
}


juce::Component* PlaylistComponent::refreshComponentForCell(int rowNumber, int columnId, bool isRowsSelected,
                                         juce::Component *existingComponentToUpdate) {
    if(columnId == 3 && existingComponentToUpdate== nullptr){
            deckButton1 = new juce::TextButton("Deck 1");
            juce::String id{std::to_string(rowNumber)};
            deckButton1->setComponentID(id);

            deckButton1->addListener(this);
            existingComponentToUpdate = deckButton1;
    }
    if(columnId == 4 && existingComponentToUpdate== nullptr){
            deckButton2 = new juce::TextButton("Deck 2");
            juce::String id{std::to_string(rowNumber)};
            deckButton2->setComponentID(id);

            deckButton2->addListener(this);
            existingComponentToUpdate = deckButton2;
    }
    if(columnId == 5 && existingComponentToUpdate== nullptr){
            deleteButton = new juce::TextButton("Delete");
            juce::String id{std::to_string(rowNumber)};
            deleteButton->setComponentID(id);

            deleteButton->addListener(this);
            existingComponentToUpdate = deleteButton;
    }

    return existingComponentToUpdate;
}

void PlaylistComponent::buttonClicked(juce::Button *button) {
    if(button->getName() != "Load to playlist") {
        int id = std::stoi(button->getComponentID().toStdString());
        juce::String total_path;
        if (button->getButtonText() == "Delete") {
            removeFromPersistence(trackTitles[id]);
            trackTitles.erase(trackTitles.begin() + id);
            trackLengths.erase(trackLengths.begin()+id);
            playlistTracks = trackTitles;
            tableComponent.updateContent();
            if(!tempTitle.empty()){tempTitle.erase(tempTitle.begin()+id);}
            if(!tempTitlePersistence.empty()){ tempTitlePersistence.erase(tempTitlePersistence.begin()+id);}
        }
        if (button->getButtonText() == "Deck 1") {
            for (const auto &track: trackURLs) {
                if (playlistTracks[id] == track.getFileName().toStdString()) {
                    std::cout << track.getFileName() << std::endl;
                    total_path = "file://" + track.getDomain() + "/" + track.getSubPath();
                    std::string final_path = total_path.toStdString();
                    std::cout << total_path << std::endl;
                    deckGui1->loadFromPlaylist(juce::URL{total_path});
                }
            }
        }
        if (button->getButtonText() == "Deck 2") {
            for (const auto &track: trackURLs) {
                if (playlistTracks[id] == track.getFileName().toStdString()) {
                    std::cout << track.getFileName() << std::endl;
                    total_path = "file://" + track.getDomain() + "/" + track.getSubPath();
                    std::string final_path = total_path.toStdString();
                    std::cout << total_path << std::endl;
                    deckGui2->loadFromPlaylist(juce::URL{total_path});
                }
            }
        }
    }
    else if (button->getName() == "Load to playlist") {
        auto fileChooserFlags = juce::FileBrowserComponent::canSelectMultipleItems | juce::FileBrowserComponent::openMode;

        fChooser.launchAsync(fileChooserFlags, [this](const juce::FileChooser& chooser)
        {
            const juce::Array<juce::File>& selectedFiles = chooser.getResults();
                for(const juce::File &file : selectedFiles){
                    addTracks(juce::URL{file});
                }
        });
    }
}


double PlaylistComponent::trackStats(const juce::URL &audioURL) {
    formatManager.registerBasicFormats();
    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false));
    if (reader != nullptr){
        std::unique_ptr<juce::AudioFormatReaderSource> newSource(new juce::AudioFormatReaderSource(reader, true));
        transportSource.setSource(newSource.get(),0, nullptr,reader->sampleRate);
        readerSource.reset(newSource.release());
        double lengthInSeconds = static_cast<double>(reader->lengthInSamples) / static_cast<double>(reader->sampleRate);
        std::cout << "PlaylistComponent::trackStats" << std::endl;
        std::cout << "Audio track length: " << lengthInSeconds << " seconds" << std::endl;
        return lengthInSeconds;
    }
}

void PlaylistComponent::addTracks(const juce::URL &audioURL) {
    std::string trackTitle = audioURL.getFileName().toStdString();
    std::string trackPath = audioURL.getDomain().toStdString() + "/" + audioURL.getSubPath().toStdString();
    if(checkIfDuplicate(trackTitle)){
        std::cout << "PlaylistComponent::addTracks" << std::endl;
        tempTitle.emplace_back(trackTitle);
        trackTitles.emplace_back(trackTitle);
        trackLengths.emplace_back(std::to_string(trackStats(audioURL)));
        trackURLs.emplace_back(audioURL);
        // append track file path to text file to be reused for persistent playlist
        writeToFile(trackPath);
        playlistTracks = trackTitles;
        tableComponent.updateContent();
    }
    else{
        std::cout << "Duplicate track" << std::endl;
    }
}

void PlaylistComponent::trackPersistence() {
    std::ifstream trackPersistenceFile("/home/shabirk/tracks.txt"); // 1
    if (!trackPersistenceFile.is_open()) {
        std::cout << "File not open" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(trackPersistenceFile, line)) {
        std::string filePath = "file://" + line;
        juce::URL audioFile{filePath};
        std::string trackTitle = audioFile.getFileName().toStdString();
        std::cout << line << std::endl;
        if (!trackTitle.empty()) {
            trackTitles.emplace_back(trackTitle);
            trackLengths.emplace_back(std::to_string(trackStats(audioFile)));
            tempTitlePersistence.emplace_back(std::move(trackTitle));
            trackURLs.emplace_back(std::move(audioFile));
        }
    }

    playlistTracks = trackTitles;
    tableComponent.updateContent();

    trackPersistenceFile.close();
}

void PlaylistComponent::removeFromPersistence(std::string_view trackTitle) {
    std::string line;
    std::ifstream fin("/home/shabirk/tracks.txt"); // 2
    std::ofstream temp("/home/shabirk/temp.txt"); // 3

    if (!fin.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return;
    }

    while (std::getline(fin, line)) {
        if (juce::URL{line}.getFileName().toStdString() == trackTitle) {
            std::cout << "PlaylistComponent::removeFromPersistence" << std::endl;
            std::cout << trackTitle << " found and removed" << std::endl;
        } else {
            temp << line << std::endl;
        }
    }

    temp.close();
    fin.close();

    const char* oldName = "/home/shabirk/tracks.txt"; // 4
    const char* newName = "/home/shabirk/temp.txt"; // 5

    if (std::remove(oldName) != 0) {
        std::cerr << "Error deleting file" << std::endl;
        return;
    }

    if (std::rename(newName, oldName) != 0) {
        std::cerr << "Error renaming file" << std::endl;
        return;
    }
}

bool PlaylistComponent::checkIfDuplicate(const std::string &trackTitle) {
    const int addTrackCount = std::count(tempTitle.cbegin(), tempTitle.cend(), trackTitle);
    const int persistenceCount = std::count(tempTitlePersistence.cbegin(), tempTitlePersistence.cend(), trackTitle);

    std::cout << "PlaylistComponent::checkIfDuplicate " << addTrackCount << " " << persistenceCount << std::endl;

    const bool add = (addTrackCount == 0) && (persistenceCount == 0);
    return add;
}

void PlaylistComponent::textEditorTextChanged(juce::TextEditor& editor){
    const juce::String searchText = searchBox.getText().trim();
    std::cout << "PlaylistComponent::textEditorTextChanged" << std::endl;
    if(searchText.isEmpty()){
        playlistTracks = trackTitles;
    }else if(searchText != lastSearchText){
        playlistTracks.clear();
        for(const auto& title : trackTitles){
            juce::String js(title);
            if(js.containsIgnoreCase(searchText)){
                playlistTracks.emplace_back(js.toStdString());
            }
        }
        lastSearchText = searchText;
    }
    tableComponent.updateContent();
}

bool PlaylistComponent::isInterestedInFileDrag(const juce::StringArray &files){
    std::cout << "DeckGUI::isInterestedInFileDrag" << std::endl;
    for(const auto &file : files){
        if(isBasicAudioFormat(juce::File{file}.getFileExtension())){
            return true;
        }
    }
    return false;
}
void PlaylistComponent::filesDropped (const juce::StringArray &files, int x, int y) {
    std::cout << "DeckGUI::filesDropped" << std::endl;

    for (const juce::File& file : files) {
        addTracks(juce::URL {juce::File{file}});
    }
}
bool PlaylistComponent::isBasicAudioFormat(const juce::String& fileExtension){
    static const std::vector<juce::String> basicFormats = { ".wav", ".aiff", ".aif", ".mp3", ".ogg", ".flac", ".wma" };
    return std::find(basicFormats.begin(), basicFormats.end(), fileExtension.toLowerCase()) != basicFormats.end();
}

void PlaylistComponent::writeToFile(const std::string &filePathName) {
    std::ofstream trackFile;
    trackFile.open("/home/shabirk/tracks.txt", std::fstream::app); // 6
    if(trackFile.is_open()){
        trackFile << filePathName << std::endl;
    }
    else{
        std::cout << "File is not open" << std::endl;
    }
}