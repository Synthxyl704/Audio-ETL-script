#include "file_manager.h"
#include <iostream>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <vector>
#include <map>
#include <algorithm>
#include <set>
#include <cstdlib>
#include <limits> 

// #include "song_manager.h"
// #include "song_manager.h"
// #include "audio_utilities.h"
// #include "file_manager.h"


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

    if (!fs::is_directory(directoryPath) /*|| fs::is_directory(directoryPath) == NULL */ ) {
        cout << "[Error]: Path is not a directory: " << directoryPath << "\n";
        return;
    }

    // convert this into a struct later?
    // 9/23 - idea scraped
    map<string, int> extensionCount;
    map<string, uintmax_t> extensionSize;
    vector<fs::directory_entry> files;
    uintmax_t totalSize = 0;
    int totalFiles = 0;
    set<string> audioExtensions = {"mp3", "wav", "flac", "aac", "ogg", "m4a", "opus"};

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
        // bro is NOT an RDBM systems joker
        if (detailed && !files.empty()) {
            cout << "\n----------------------------------------\n";
            cout << "\nDetailed file listing:\n";
            cout << setw(40) << left << "Filename" 
                 << setw(12) << right << "Size" 
                 << setw(10) << "F-Ext/type" << "\n";
            cout << string(62, '-') << "\n";

            // fullisade of stream insertions 
        
            for (const auto &file : files) {
                string filename = file.path().filename().string();
                string extension = file.path().extension().string();
                transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

                // std::string toLower(const std::string& str) {
                //    std::string result = str;
                //    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
                //
                //    return result;
                // }               
                
                // std::string fileType = "Other/unknown";
                // if (audioExtensions.find(tolower(extension)) != audioExtensions.end()) {
                //     fileType = "Audio";
                // }

                // std::string fileType = "Other/unknown";
                // if (audioExtensions.find(extension) != audioExtensions.end()) {
                //     fileType = extension + " | Audio";
                // } // else { std::shit }
                
                string fileType;
                if (audioExtensions.find(extension) != audioExtensions.end()) {
                    fileType = extension + " | Audio";
                } else {
                //     if (fileType = extension.empty()) {
                //         fileType = "[no ext found]";
                //     } else {
                //         fileType = extension + " | other/undef";
                //     }
                // }

                fileType = (extension.empty()) ? "[no extension]" : extension;
                }

                if (filename.length() > 38) {
                    filename = filename.substr(0, 35) + "...";
                } // trunctate fns ()

                cout << setw(40) << left << filename << setw(12) << right << getReadableFileSize(file.file_size())
                     << setw(10) << fileType << /* std::endl << ; */ "\n";
            } // for
        } // if 
    } catch (const fs::filesystem_error &error) {
        cout << "[FSYS_READ_ERROR]: Could not calculate specified directory: " << error.what() << "\n";
    }
}

// okay 
// list available files
// give the files an index (inc = 0; [<< inc + 1])
// volition char/stream for user input, will convert to numerics if not ss 
// invoke ffmpeg for those filex and transcode those file(s) only

vector<fs::path> listAvailableFiles(const string& directoryPath) {
    vector<fs::path> filePaths;
    
    if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath)) {
        cout << "[FSYS_ERROR]: Directory not found at path: " << directoryPath << "\n";
        return filePaths; // empty
    }

    vector<fs::directory_entry> files;
    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        if (entry.is_regular_file()) { // yeah mfs named this "is_regular_file" 
            files.push_back(entry);
        }
    }

    sort(files.begin(), files.end(), [](const fs::directory_entry& a, const fs::directory_entry& b) {
        return a.path().filename().string() < b.path().filename().string();
    });

    cout << "\n=== Available Files in ~/" << fs::path(directoryPath).filename().string() << " ===\n";
    cout << string(45, '-') << "\n";

    if (files.empty() || files.size() == 0) {
        cout << "No files found in this directory.\n";
        return filePaths;
    }
    
    int index = 1;
    for (const auto& file : files) {
        fs::path currentPath = file.path();
        cout << "[" << setw(2) << right << index << "] " 
             << setw(30) << left << currentPath.filename().string() 
             << " (" << getReadableFileSize(file.file_size()) << ")\n";
        filePaths.push_back(currentPath);
        index += 1;
    }
    cout << string(45, '-') << "\n";
    
    return filePaths;
}

// okay 
// list available files
// give the files an index (inc = 0; [<< inc + 1])
// volition char/stream for user input, will convert to numerics if not ss 
// invoke ffmpeg for those files and transcode those file(s) only

// if no directory found (handled in main)
// if no file found?

void transcodeSelectedFiles(const string& directoryPath) {
    vector<fs::path> availableFiles = listAvailableFiles(directoryPath);
    if (availableFiles.empty()) {
        std::cerr << "[FILE_OPEN_ERROR] Listed directory / list has detected absence of files for transcoding.";
        return;
    } 

    cout << "Enter the indexes of files to transcode (the ones in sq. brackets) | (EG - 6, 9, 420): ";

    string selectionInput;
    getline(cin, selectionInput);

    set<int> selectedIndices;
    stringstream ss(selectionInput);
    string segment;

    while (getline(ss, segment, ',')) {
        segment.erase(remove_if(segment.begin(), segment.end(), ::isspace), segment.end());

        if (segment.empty()) {
            continue;
        }

        try {
            long long idx = stoll(segment);

            if (idx < 1 || idx > static_cast<long long>(availableFiles.size())) {
                cout << "[UIDX_INPUT_ERROR]: Invalid index: [" << idx << "]\n";
                continue;
            }

            if (idx > INT32_MAX) {
                cout << "[INT_OVERFLOW_ERROR]: Index too large to fit: [" << idx << "] \n";
                continue;
            }

            selectedIndices.insert(static_cast<int>(idx));
        } catch (const std::exception& e) {
            cout << "[UIDX_INPUT_ERROR]: Invalid input: '" << segment << "' -> " << e.what() << "\n";
            return;
        }
    }

    if (selectedIndices.empty()) {
        cout << "[WARN_LOG]: No valid files selected.\n";
        return;
    }

    std::cout << "\n[INFO]: You have selected [" << selectedIndices.size() << "] files for codec transcoding\n\n";
    for (int fileNameAtIndex : selectedIndices) {
        std::cout << "-> " << availableFiles[fileNameAtIndex - 1] << std::endl;
    }

    // std::cout << std::endl; 
    std::cout << "\n";

    // for (int index : choices) {
        // std::cout << "-> " << songs[index /* idx */ - 1].filename << "\n";
    // }

    cout << "Enter the target audio format (e.g. mp3, flac, wav): ";

    string targetFormat;
    cin >> targetFormat;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (!targetFormat.empty() && targetFormat[0] == '.') {
        targetFormat = targetFormat.substr(1);
    }
    
    transform(targetFormat.begin(), targetFormat.end(), targetFormat.begin(), ::tolower);
    
    const set<string> validFormats = {"mp3", "flac", "wav", "ogg", "opus", "aac", "m4a"};

    if (validFormats.find(targetFormat) == validFormats.end()) {
        cout << "[INPUT_CODEC_ERROR]: Unsupported format: " << targetFormat << "\n";
        return;
    }

    cout << "[TRANSCODE_PROMPT]: Do you wish to remove the prior (codec) file(s) to eradicate file redundancy? (Y/N): ";
    
    char removeOriginalChoice;
    cin >> removeOriginalChoice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    bool removeOriginal = (removeOriginalChoice == 'Y' || removeOriginalChoice == 'y');

    cout << "\n=== TRANSCODING WITH FFMPEG ===\n";

    for (int index : selectedIndices) {
        const fs::path& inputPath = availableFiles[index - 1];
        fs::path outputPath = inputPath.parent_path() / (inputPath.stem().string() + "." + targetFormat);

        if (inputPath == outputPath) {
            cout << "[FFMPEG_TRANSCODE_SEQ]: File transcoding halted - " << inputPath.filename() << " | (already target format)\n";
            continue;
        }

        cout << "Transcoding '" << inputPath.filename().string() << "' -> '" << outputPath.filename().string() << "'\n";

        // DO NOT TOUCH THIS
        string command = "ffmpeg -i \"" + inputPath.string() + "\" -y \"" + outputPath.string() + "\" -hide_banner -threads 4 2>&1";

        FILE *pipe = popen(command.c_str(), "r");
        if (!pipe) {
            std::cerr << "\n\n[FFMPEG_ERROR]: Failed to execute and run FFmpeg, the transcoding process has been terminated due to this.\n" << std::endl;
            continue;
        }

        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            cout << buffer;
        }

        int returnCode = pclose(pipe);
        if (returnCode == 0) {
            cout << "[PROCESS_INFO_LOG]: Transcoding has been completed by FFmpeg!\n";
            
            if (removeOriginal && fs::exists(outputPath)) {
                try {
                    if (fs::file_size(outputPath) > 0) {
                        fs::remove(inputPath);
                        cout << "\n[TRANSCODE_OPERATION_LOG]: The original file codec has been removed: " << inputPath.filename() << "\n";
                    } else {
                        cout << "\n[TRANSCODE_OPERATION_WARNING]: Output file is empty, keeping original file: " << inputPath.filename() << "\n";
                    }
                } catch (const fs::filesystem_error& fileSysErr) {
                    cout << "[FILE_OPS_ERROR]: Failed to remove original file: [" 
                         << inputPath.filename() 
                         << "] - " << fileSysErr.what() << "\n";
                }
            }
        } else {
            cout << "[FFMPEG_INTERNAL_ERROR]: Exit code: [" << returnCode << "] \n";

            if (fs::exists(outputPath)) {
                try {
                    fs::remove(outputPath); 
                    cout << "[FFMPEG_CLEANUP_LOG]: Removed incomplete output file: " << outputPath.filename() << "\n";
                } catch (const fs::filesystem_error& outputPathCleanupErr) {
                    cout << "[FFMPEG_CLEANUP_ERROR]: Failed to remove incomplete output, manual removal is recommendend: " 
                         << outputPath.filename() 
                         << " - " << outputPathCleanupErr.what() 
                         << std::endl;
                }
            }
        }
    }

    cout << "\n[RE:PROCESS_TERMINATE_LOG]: The codec transcoding process has encountered no errors.\n";
}
