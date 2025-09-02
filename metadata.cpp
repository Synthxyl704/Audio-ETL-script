#include "metadata.h"
#include "audio_utilities.h"
#include "file_manager.h"

#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <cstdio>

using namespace std;
namespace fs = std::filesystem;

void displayMetadata(const string &filePath) {
    cout << "\n[METADATA for: " << fs::path(filePath).filename().string() << "]\n";
    cout << "----------------------------------------\n";
    
    char metadataCommand[0x800];
    snprintf(metadataCommand, sizeof(metadataCommand),
        "ffprobe -v quiet -print_format json -show_format -show_streams \"%s\" 2>/dev/null",
        filePath.c_str());
    
    // metashit extraction
    char simpleCommand[0x800];
    snprintf(simpleCommand, sizeof(simpleCommand),
        "ffprobe -v quiet -show_entries format=duration,bit_rate -show_entries stream=codec_name -of csv=p=0 \"%s\" 2>/dev/null",
        filePath.c_str());
    
    FILE *pipe = popen(simpleCommand, "r");
    if (pipe) {
        char buffer[256];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            cout << "Technical Info: " << buffer;
        }
        pclose(pipe);
    }
    
    // title, artist, album | ffprobe
    snprintf(simpleCommand, sizeof(simpleCommand),
        "ffprobe -v quiet -show_entries format_tags=title,artist,album -of csv=p=0 \"%s\" 2>/dev/null",
        filePath.c_str());
    
    pipe = popen(simpleCommand, "r");
    if (pipe) {
        char buffer[512];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            string metadata(buffer);
            if (!metadata.empty() && metadata != "\n") {
                cout << "[Tags]: " << metadata;
            } else {
                cout << "No embedded tags found\n";
            }
        }
        pclose(pipe);
    }
    
    cout << "----------------------------------------\n";
}

// metadata
void showSongMetadata(const string &downloadPath) {
    vector<SongInfo> songs;
    
    // list audio files
    try {
        for (const auto& entry : fs::directory_iterator(downloadPath)) {
            if (entry.is_regular_file() && isAudioFile(entry.path().filename().string())) {
                SongInfo song;
                song.filename = entry.path().filename().string();
                song.fullPath = entry.path().string();
                songs.push_back(song);
            }
        }
    } catch (const fs::filesystem_error& e) {
        cout << "[Error]: Could not access directory: " << e.what() << "\n";
        return;
    }
    
    if (songs.empty()) {
        cout << "No audio files found.\n";
        return;
    }
    
    // Sort songs by filename
    sort(songs.begin(), songs.end(), [](const SongInfo& a, const SongInfo& b) {
        return a.filename < b.filename;
    });
    
    cout << "\n[SONG METADATA] - Select a song to view details\n";
    cout << "==============================================\n";
    
    for (size_t inc = 0; inc < songs.size(); inc += 1) {
        cout << "[" << (inc + 1) << "] " << songs[inc].filename << "\n";
    }
    
    cout << "\nEnter song number to view metadata (1 <-> " << songs.size() << ") | 0 to cancel: ";
    int choice;
    cin >> choice;
    
    if (choice == 0) {
        cout << "Metadata displayInfo function cancelled.\n";
        return;
    }
    
    if (choice < 0 || choice > static_cast<int>(songs.size())) {
        cout << "[Error]: Invalid selection.\n";
        return;
    }
    
    const SongInfo& selectedSong = songs[choice - 1];
    displayMetadata(selectedSong.fullPath);
}
