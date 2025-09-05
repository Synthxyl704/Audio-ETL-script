#include "song_manager.h"
#include "audio_utilities.h"
#include "file_manager.h"

#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <chrono>

using namespace std;
namespace fs = std::filesystem;

// how do i parse the song title seperately?
void isSongRedundant(const string &downloadPath, const string &songTitle) {
    // store extracted metadata filename in vec
    // iterate through vector to find matching name 
    // if match == true; tell the user
    std::vector<std::filesystem::path> matchingTitle; // matchingFiles to be precise
    try {
        for (const auto &entry : fs::directory_iterator(downloadPath)) {
            if (!entry.is_regular_file()) { continue; }

            std::string fileName = entry.path().filename().string();

            if (isAudioFile(fileName)) {
                std::string onlyName = entry.path().stem().string();

                // if same title 
                if (onlyName == songTitle) {
                    matchingTitle.push_back(entry.path());
                }
            }
        }
    } // end try

        catch (const fs::filesystem_error& error) {
            std::cout << "[FSYS-Error]: Some problem occured checking the dir/folder for redundancy check: " << error.what() << std::endl;
            return; 
        }

        if (matchingTitle.empty()) {
            std::cout << "\n[PROG_FLAG_REDUNDANT]: No redundant song found, good to go!!!";
        } else {
            std::cout << "\n[PROG_FLAG_REDUNDANT]: A song(s) with similar name/title found in your dir/folder: ";
            for (const auto &path : matchingTitle) {
                std::cout << " - " << path.filename().string() << std::endl;
                // return EXIT_FAILURE;
                // return;
            }

            // if they want to, the user will proceed or not depending on their choice 
            
            std::cout << "[FSYS-CBranch]: Do you wish to proceed regardless of the redundancy?" << endl << endl;

            char volitionChar;
            std::cout << "[yY/nN]: ";
            std::cin >> volitionChar; 
            if (volitionChar == 'n' || volitionChar == 'N') {
                cout << "\033[1m" << "[FSYS-FLAG_Cbranch_consequence]:" << "\033[0m" << "Song downloading action exec ceased due to redundancy." << endl;
                // return EXIT_FAILURE; 
                std::exit(EXIT_FAILURE); 

            } else if (volitionChar == 'y' || volitionChar == 'Y') {
                cout << "[FSYS-FLAG_Cbranch_consequence]: Proceeding with dowloading despite redundancy: " << endl;
                // continue;
            }
        }
    } 

// List all downloaded songs in the specified directory
void listDownloadedSongs(const string &downloadPath) {
    cout << "\n[DOWNLOADED SONGS in: " << downloadPath << "]\n";
    cout << "========================================\n";
    
    vector<SongInfo> songs;

    // file ext
    // file create time
    
    try {
        for (const auto &entry : fs::directory_iterator(downloadPath)) {
            if (entry.is_regular_file()) {
                string filename = entry.path().filename().string();
                if (isAudioFile(filename)) {
                    SongInfo song;
                    song.filename = filename;
                    song.fullPath = entry.path().string();
                    song.fileSize = entry.file_size();
                    
                    // file ext
                    size_t dotPos = filename.find_last_of('.');
                    song.format = (dotPos != string::npos) ? filename.substr(dotPos + 1) : "unknown";
                    
                    // file time
                    auto ftime = fs::last_write_time(entry.path());
                    auto sctp = chrono::time_point_cast<chrono::system_clock::duration>(
                        ftime - fs::file_time_type::clock::now() + chrono::system_clock::now());
                    time_t cftime = chrono::system_clock::to_time_t(sctp);
                    song.lastModified = ctime(&cftime);
                    song.lastModified.pop_back(); // remove newline
                    
                    songs.push_back(song);
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        cout << "[Error]: Could not access directory: " << e.what() << "\n";
        return;
    }
    
    if (songs.empty()) {
        cout << "No audio files found in the download/told directory.\n";
        return;
    }
    
    // sort alphabetically
    sort(songs.begin(), songs.end(), [](const SongInfo& a, const SongInfo& b) {
        return a.filename < b.filename;
    });
    
    cout << "[Total songs found]: " << songs.size() << "\n\n";
    
    for (size_t inc = 0; inc < songs.size(); inc++) {
        cout << "[" << (inc + 1) << "] " << songs[inc].filename << "\n";
        cout << "    Format: " << songs[inc].format << " | Size: " << getReadableFileSize(songs[inc].fileSize);
        cout << " | Modified: " << songs[inc].lastModified << "\n\n";
    }
}

// removal
void removeSong(const string &downloadPath) {
    vector<SongInfo> songs;
    
    // list all audio files
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
        cout << "[Error]: Directory access failure: " << e.what() << "\n"; // e what?
        return;
    }
    
    if (songs.empty()) {
        cout << "No audio files even downloaded for removal conduction.\n";
        return;
    }
    
    // alphabetic order
    sort(songs.begin(), songs.end(), [](const SongInfo& a, const SongInfo& b) {
        return a.filename < b.filename;
    });
    
    cout << "\n[REMOVE SONG - Select a song to delete]\n";
    cout << "=========================================\n";
    
    for (size_t inc = 0; inc < songs.size(); inc++) {
        cout << "[" << (inc + 1) << "] " << songs[inc].filename << "\n";
    }
    
    cout << "\nEnter song number to delete (1 <-> " << songs.size() << ") or 0 to cancel: ";
    int choice;
    cin >> choice;
    
    if (choice == 0) {
        cout << "Removal cancelled.\n";
        return;
    }
    
    if (choice < 1 || choice > static_cast<int>(songs.size())) {
        cout << "[Error]: Invalid selection.\n";
        return;
    }
    
    const SongInfo& selectedSong = songs[choice - 1];
    
    cout << "\nAre you sure you want to delete: " << "[ " << selectedSong.filename << " ]? (yY/nN): ";

    char confirm;
    cin >> confirm;
    
    if (confirm == 'y' || confirm == 'Y') {
        try {
            fs::remove(selectedSong.fullPath);
            cout << "[SUCCESS]: " << "[ " << selectedSong.filename << " ] " << "(song) deleted successfully.\n";
        } catch (const fs::filesystem_error& e) {
            cout << "[Error]: File deletion error: " << e.what() << "\n";
        }
    } else {
        cout << "Deletion cancelled.\n";
    }
}
