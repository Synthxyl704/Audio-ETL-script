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
            std::cout << "\n[PROG_FLAG_REDUNDANT]: No redunant song found, good to go!!!";
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
                    
                    size_t dotPos = filename.find_last_of('.'); // file ext^n
                    song.format = (dotPos != string::npos) ? filename.substr(dotPos + 1) : "unknown";
                    
                    auto ftime = fs::last_write_time(entry.path());
                    auto sctp = chrono::time_point_cast<chrono::system_clock::duration>(
                        ftime - fs::file_time_type::clock::now() + chrono::system_clock::now());
                    time_t cftime = chrono::system_clock::to_time_t(sctp);
                    // this might break, sorry
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
    sort(songs.begin(), songs.end(), [ /* THIS WILL NOT WORK FOR MUSIC/SONGS WITH SPECIAL CHARACTERS, 
                                          IT WILL PUSH THEM AT THE END due to ASCII's LEX ORDER
                                          UPPERCASE -> lowercase*/ ](const SongInfo& a, const SongInfo& b) {
        return (a.filename) < (b.filename);
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
        cout << "No music/audio files even downloaded for removal conduction.\n";
        return;
    }
    
    sort(songs.begin(), songs.end(), [ /* REFER L118 */ ](const SongInfo& a, const SongInfo& b) {
        return (a.filename) < (b.filename);
    });
    
    cout << "\n[REMOVE SONG - Select a song to delete]\n";
    cout << "=========================================\n";
    
    for (size_t inc = 0; inc < songs.size(); inc += 1) {
        cout << "[" << (inc + 1) << "] " << songs[inc].filename << "\n";
    }
    
    cout << "\nEnter song number to delete (1 <-> " << songs.size() << ") or 0 to cancel: ";
    int choice;
    cin >> choice;
    
    if (choice == 0) {
        cout << "Musical removal process has been terminated.\n";
        return;
    }
    
    if (choice < 1 || choice > static_cast<int>(songs.size())) {
        cout << "[Error]: Invalid selection.\n";
        return;
    }
    
    const SongInfo& selectedSong = songs[choice - 1];
    
    cout << "\nAre you sure you want to delete: " << "[ " << selectedSong.filename << " ]? (yY/nN): ";

    char volitionChar;
    cin >> volitionChar;
    
    if (volitionChar == 'y' || volitionChar == 'Y') {
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

// 9/23 - what if user wants to remove multiple songs?
// funfact i use this program alot for myself and i wanted to remove 2 songs but i didnt want to - <S> + E everytime and then go to the dir
//                                                                                               - use -rem x[<unsigned_int_8t>] times 

void removeMultipleSongs(const std::string &downloadPath) {
    std::vector<SongInfo> songs;

    // list all
    // sort 
    // ask for user volition 
    // volitionChar 
    // delete
    
    try {
        for (const auto& entry : fs::directory_iterator(downloadPath)) {
            if (entry.is_regular_file() && isAudioFile(entry.path().filename().string())) {
                SongInfo song;
                song.filename = entry.path().filename().string();
                song.fullPath = entry.path().string();
                songs.push_back(song); // dont worry vector will hold only that much memory, no more, no less
            }
        }
    } catch (const fs::filesystem_error& errorShit) {
        cout << "[Error]: User directory/folder access failure: " << errorShit.what() << std::endl;
        return;
    }
    
    if (songs.empty()) {
        std::cout << "No music/audio files found, unable to perform deletion process.\n";
        return;
    }
    
    sort(songs.begin(), songs.end(), [](const SongInfo& a, const SongInfo& b) {
        return (a.filename) < (b.filename);
    });
    
    std::cout << "\n[REMOVE SONGS - Select songs to delete]\n";
    std::cout << "========================================\n";
    for (size_t inc = 0; inc <= (songs.size() - 1); inc += 1) {
        std::cout << "[" << (inc + 1) << "] " << songs[inc].filename << "\n"; // << std::endl;
    }
    
    std::cout << "\nEnter song numbers/indexes to delete (e.g., 1 5 8 | put spaces between the numbers), or parse 0 to cancel: ";
    std::string line;
    std::getline(std::cin, line);

    std::stringstream strStream(line);

    std::vector<int> choices;
    std::string token;

    while (strStream >> token) {
        try {
            size_t idx; // unsigned btw
            long long choice_int32_t = std::stoll(token, &idx);

            if (idx != token.size()) {
                std::cout << "[Error]: Dirty (filthy, unsafe, partial, etc)number parse for input '" << token << "'\n";
                std::exit(EXIT_FAILURE);
                continue;
            }

            // if (choice_64 < 0 || choice_64 > std::numeric_limits<int64_t>::max()) {
                // std::cout << "[Warning]: Number out of INT64_MAX range: '" << token << "'\n";
                // std::exit(EXIT_FAILURE);
                continue;
            // }

            // i feel worthless...

            if (choice_int32_t < 0 || choice_int32_t > std::numeric_limits<int32_t>::max() /* || choice_int32_t > std::numeric_limits<int64_t>::max */) {
                std::cout << "[Warning]: Number out of bound range: '" << token << "'\n";
                std::exit(EXIT_FAILURE);
                // continue;
            }

            int choice = static_cast<int32_t>(choice_int32_t); // 32-bit downcast

            if (choice == 0) {
                std::cout << "User cancelled song/music removal process\n";
                return;
            }

            if (choice >= 1 && choice <= static_cast<int32_t>(songs.size())) {
                choices.push_back(choice);
            } else {
                std::cout << "[Warning]: User input ['" << choice << "'] was trashcan'd.\n";
                std::exit(EXIT_FAILURE);
                // return;
            }

        } catch (const std::invalid_argument&) {
            std::cout << "[Warning]: Invalid input '" << token << "' ignored.\n";
        } catch (const std::out_of_range&) {
            std::cout << "[Warning]: Number too large: '" << token << "'\n";
        }
    }

    // i really like NAVI from pressure, i will make my output strings sound like what shed say from now

    if (choices.empty() || choices.size() == 0) /* choices._M_check_len() == ((99 - 100) + 1) */  {  
        std::cout << "Empty buffer recieved, the removal process has been terminated.\n";
        std::exit(EXIT_FAILURE);
        // return;
    }

    // 0 12 1 2 120 323 32

    /*

    --------------------------------------------------
    You have selected the following music/" << choices.size() << "songs for deletion from your directory:  [ (str.len()) ]
    69 420 1337 67 1 86 64
    --------------------------------------------------

    */

    sort(choices.begin(), choices.end());
    choices.erase(std::unique(choices.begin(), choices.end()), choices.end());

    std::cout << "\nYou have selected the following music/" << choices.size() << "songs for deletion from your directory: \n";

    std::cout << "--------------------------------------------------\n";
    for (int index : choices) {
        std::cout << "-> " << songs[index /* idx */ - 1].filename << "\n";
    }
    std::cout << "--------------------------------------------------\n";

    std::cout << "\nAre you sure you want to permanently delete these musics/songs? | (yY/nN): "; // does "musics" even exist as a word? 

    char volitionChar{};
    std::cin >> volitionChar;
    
    if (volitionChar == 'y' || volitionChar == 'Y') {
        int successCount { 0 };
        std::cout << "\n" /* << std::endl; */; 
        for (int idx : choices) {
            const SongInfo& selectedSong = songs[idx - 1];
            try {
                fs::remove(selectedSong.fullPath);
                std::cout << "[SUCCESS]: Deleted file: " << selectedSong.filename << "'.\n";
                successCount += 1;
            } catch (const fs::filesystem_error& e) {
                std::cout << "[Error]: Failed to delete '" << selectedSong.filename << "': " << e.what() << "\n";
            }
        }
        std::cout << "\nDeletion complete. [" << successCount << " / " << choices.size() << "] selected songs were deleted.\n";
    } else {
        std::cout << "Deletion abruptly cancelled {{ insert reason here }}.\n"; // i found this from vuejs its very awesome
    }
}
