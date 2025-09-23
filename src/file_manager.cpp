#include "file_manager.h"
#include <iostream>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <vector>
#include <map>
#include <algorithm>
#include <set>

using namespace std;
namespace fs = std::filesystem;

string createOrGetDownloadFolder(const string &customPath) {
    string downloadPath;
    
    if (!customPath.empty()) {                          // if [!empty]
        downloadPath = customPath;
    } else {
        
        // check if downloads -> in current dir
        downloadPath = "./downloads";
        if (!fs::exists(downloadPath)) {  // if !exists(pathToDownload)
            cout << "[Info]: Creating downloads folder in current directory\n";

            // try to create a folder -> if yes -> pog
            // !attemptToCreateFolder -> catch (const fs::filesystem_error& e) whatever this shit is
            try {
                fs::create_directory(downloadPath); 
                // create dir in current dir if no path 
            } catch (const fs::filesystem_error& e) {
                cout << "[Warning]: could not create downloads folder (dir usage: current)\n";
                downloadPath = ".";
            }
        }
    }

    if (!fs::exists(downloadPath) || fs::exists(downloadPath) == false) {
        cout << "[Error]: Specified download path does not exist: " << downloadPath << "\n";
        return 0; // return EXIT_SUCCESS doesnt work for whatever rsn
    }
    
    return downloadPath;
}

string getReadableFileSize(uintmax_t bytes) {
    const char *units[] = {"B", "KB", "MB", "GB"};
    double size = bytes;
    int unitIndex = 0;

    // deliberately put 1024 and not 1E+3
    // b2 binary prefix, change in your sys in case b10

    while (size >= 1024 && unitIndex < 3) {
        size /= 1024;
        unitIndex += 1;
    }
    
    stringstream ss; // this thing is cool as fuck
    ss << fixed << setprecision(2) << size << " " << units[unitIndex];
    return ss.str(); 
}

// bool detailed = true
void listDirectoryContents(const string &directoryPath, bool detailed) {
    if (!fs::exists(directoryPath) || fs::exists(directoryPath) != (!NULL)) /* same thing */ {
        cout << "[Error]: Directory does not exist: " << directoryPath << "\n";
        return;
    }

    if (!fs::is_directory(directoryPath) || fs::is_directory(directoryPath) == NULL) {
        cout << "[Error]: Path is not a directory: " << directoryPath << "\n";
        return;
    }

    // convert this into a struct later?
    map<string, int> extensionCount;
    map<string, uintmax_t> extensionSize;
    vector<fs::directory_entry> files;
    uintmax_t totalSize = 0;
    int totalFiles = 0;
    set<string> audioExtensions = {"mp3", "wav", "flac", "aac", "ogg", "m4a"};

    try {
        for (const auto &entry : fs::directory_iterator(directoryPath)) {
            if (entry.is_regular_file()) {
                files.push_back(entry);
                totalFiles += 1;
                
                string extension = entry.path().extension().string();
                transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
                
                uintmax_t fileSize = entry.file_size();
                totalSize += fileSize;
                
                extensionCount[extension] += 1;
                extensionSize[extension] += fileSize;
            }
        }

        // predicate for alpha sort
        sort(files.begin(), files.end(), [](const fs::directory_entry &a, const fs::directory_entry &b) {
            return (a.path().filename().string()) < (b.path().filename().string());
        }); // nice 

        // i cant make this more aesthetic :(
        cout << "\n=== File/Directory info statistics: .../" << directoryPath << " ===\n\n";
        cout << "Total files: " << totalFiles << "\n";
        cout << "Total size: " << getReadableFileSize(totalSize) << "\n";
        cout << "----------------------------------------\n\n";

        // int audioFileCount = 0;
        // uintmax_t audioFileSize = 0;
// 
        // for (const auto &ext : audioExtensions) {
            // if (extensionCount.find(ext) != extensionCount.end()) {
                // audioFileCount += extensionCount[ext];
                // audioFileSize += extensionSize[ext];
            // }
        // }
// 
        // cout << "Audio files (songs): " << audioFileCount << "\n";
        // cout << "Audio files size: " << getReadableFileSize(audioFileSize) << "\n";
        // cout << "----------------------------------------\n";

        cout << "File extension statistics:\n";
        for (const auto &[ext, count] : extensionCount) {
            double percentage = (static_cast<double>(count) / totalFiles) * 100;
            cout << " | " << (ext.empty() ? "[no extension]" : ext) << ": " 
                 << count << " files (" << fixed << setprecision(1) << percentage << "%), "
                 << getReadableFileSize(extensionSize[ext]) << "\n";
        }

        // tabular (normalized) file listing
        if (detailed && !files.empty()) {
            cout << "\n----------------------------------------\n";
            cout << "\nDetailed file listing:\n";
            cout << setw(40) << left << "Filename" 
                 << setw(12) << right << "Size" 
                 << setw(10) << "F-Type" << "\n";
            cout << string(62, '-') << "\n";

            for (const auto &file : files) {
                string filename = file.path().filename().string();
                string extension = file.path().extension().string();
                transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
                
                string fileType = "Other/unknown";
                if (audioExtensions.find(extension) == audioExtensions.end()) {
                    fileType = audioExtensions.end() + " | Audio";
                }


                if (filename.length() > 38) {
                    filename = filename.substr(0, 35) + "...";
                } // trunctate fns ()

                cout << setw(40) << left << filename
                     << setw(12) << right << getReadableFileSize(file.file_size())
                     << setw(10) << fileType << "\n";
            }
        }

    } catch (const fs::filesystem_error &error) {
        cout << "[Error]: Could not calc file/directory: " << error.what() << "\n";
    }
}
