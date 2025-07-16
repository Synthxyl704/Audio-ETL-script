#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <filesystem>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <sstream>

using namespace std;
namespace fs = std::filesystem;

vector<std::string> supportedAudioFormats = { "mp3", "wav", "flac", "aac", "ogg", "m4a" };

bool isTheFormatSupported(const string &format) {
    for (int inc = 0; inc < supportedAudioFormats.size(); inc += 1) {
        if (format == supportedAudioFormats[inc]) {
            return true;
        }
    }
    return false;
}

bool checkDependenciesInstallation() {

    // check for yt-dlp
    if (system("which yt-dlp > /dev/null 2>&1") != 0) {
        cout << "[Error]: yt-dlp is not installed or not in PATH\n";
        cout << "Install with: pip install yt-dlp\n";
        return false;
    }
    
    // check for ffmpeg
    if (system("which ffmpeg > /dev/null 2>&1") != 0) {
        cout << "[Error]: ffmpeg is not installed or not set in correct path for execution\n";
        cout << "Install ffmpeg from: https://ffmpeg.org/download.html\n";
        return false;
    }

    return true;
}

// how 2 check if its a playlist and how to make files for it?
// playlist -> yt
// sets -> sc
// list

bool isPlaylistURL(const string &url) {
    return (url.find("playlist") != string::npos ||     // youtube
            url.find("&list=") != string::npos ||       
            url.find("?list=") != string::npos ||
            url.find("/sets/") != string::npos);        // soundcloud
}

string createOrGetDownloadFolder(const string &customPath = "") {
    string downloadPath;
    
    if (!customPath.empty()) {                          // if [!empty]
        downloadPath = customPath;
    } else {
        
        // check if downloads -> in current dir
        downloadPath = "./downloads";
        if (!fs::exists(downloadPath)) {  // if !exists(pathToDOwnload)
            cout << "[Info]: Creating downloads folder in current directory\n";

            // try to create a folder -> if yes -> pog
            // !attemptToCreateFolder -> catch (const fs::filesystem_error& ) whatever this shit is
            try {
                fs::create_directory(downloadPath); // create dir in current dir instead because we cool like that
            } catch (const fs::filesystem_error& e) {
                cout << "[Warning]: could not create downloads folder (dir usage: current)\n";
                downloadPath = ".";
            }
        }
    }

    if (!fs::exists(downloadPath)) {
        cout << "[Error]: Specified download path does not exist: " << downloadPath << "\n";
        return "";
    }
    
    return downloadPath;
}

struct SongInfo {
    string filename;
    string fullPath;
    string format;
    uintmax_t fileSize;
    string lastModified;
};

bool isAudioFile(const string &filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == string::npos) return false;
    
    string extension = filename.substr(dotPos + 1);
    transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    return isTheFormatSupported(extension);
}

string getReadableFileSize(uintmax_t bytes) {
    const char *units[] = {"B", "KB", "MB", "GB"};
    double size = bytes;
    int unitIndex = 0;
    
    while (size >= 1024 && unitIndex < 3) {
        size /= 1024;
        unitIndex += 1;
    }
    
    stringstream ss;
    ss << fixed << setprecision(2) << size << " " << units[unitIndex];
    return ss.str();
}

// List all downloaded songs in the specified directory
void listDownloadedSongs(const string &downloadPath) {
    cout << "\n[DOWNLOADED SONGS] in: " << downloadPath << "]\n";
    cout << "========================================\n";
    
    vector<SongInfo> songs;

    // file ext
    // file create time
    
    try {
        for (const auto& entry : fs::directory_iterator(downloadPath)) {
            if (entry.is_regular_file()) {
                string filename = entry.path().filename().string();
                if (isAudioFile(filename)) {
                    SongInfo song;
                    song.filename = filename;
                    song.fullPath = entry.path().string();
                    song.fileSize = entry.file_size();
                    
                    // file ext
                    size_t dotPos = filename.find_last_of('.');
                    song.format = (dotPos != string::npos) ? filename.substr(dotPos + 1) : "unknown extension";
                    
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
        cout << "[Error]: Directory access failure: " << e.what() << "\n";
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
    
    for (size_t inc = 0; inc < songs.size(); inc += 1) {
        cout << "[" << (inc + 1) << "] " << songs[inc].filename << "\n";
        cout << "-----------------------------------------------" << std::endl;
        cout << "->   Format: " << songs[inc].format << std::endl;
        cout << "->     Size: " << getReadableFileSize(songs[inc].fileSize) << std::endl;
        cout << "-> Modified: " << songs[inc].lastModified << "\n\n";
    }
}

// add metadata, removal
void printUsage(const char *programName) {
    cout << "\n[Usage]: " << programName << " <URL> <format> [download_folder]\n";
    cout << "     OR: " << programName << " --list [download_folder]\n";
    cout << "\n[Supported formats]: \n";
    for (int inc = 0; inc < supportedAudioFormats.size(); inc += 1) {
        cout << "-> [" << supportedAudioFormats[inc] << "]\n";
    }
    cout << "\n[Supported platforms]: YouTube, SoundCloud (Spotify not supported due to DRM)\n";
    cout << "\n[Examples]:\n";
    cout << "  Single video: " << programName << " https://www.youtube.com/watch?v=dQw4w9WgXcQ mp3\n";
    cout << "  Playlist: " << programName << " https://www.youtube.com/playlist?list=PLxxxxxxx mp3\n";
    cout << "  Custom folder: " << programName << " https://youtu.be/example mp3 /path/to/folder_or_directory\n";
    cout << "  SoundCloud: " << programName << " https://soundcloud.com/user/track mp3\n";
    cout << std::endl;
    cout << "Utilitary command(s): " << std::endl; // CHANGE THIS WHEN YOU ADD OTHER UTIL CMDS
    cout << "  List songs: " << programName << " --list\n";
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    string firstArg = argv[1];
    
    if (firstArg == "-songlist") {
        string downloadPath = (argc == 3) ? argv[2] : "";

        downloadPath = createOrGetDownloadFolder(downloadPath);
        if (downloadPath.empty()) {
            return EXIT_FAILURE;
        }

        listDownloadedSongs(downloadPath);

       return EXIT_SUCCESS;
    } else if (firstArg == "argvlist") { // will remove this later
        for(int inc = 0; inc < argc; inc += 1) {
            std::cout << "argv[" << inc << "]: " << argv[inc] << std::endl;
        }

        return EXIT_SUCCESS;
    }

    if (argc < 3) {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    // argv[0] = ./shit
    // argv[1] = URL
    // argv[2] = extension
    // argv[3] = path
    // if !path -> list=true

    const std::string url = argv[1];
    const std::string format = argv[2];
    const std::string customFolder = (argc == 4) ? argv[3] : "";

    // ERROR conditionals --------------------------------------------

    if (url.length() > 0x400) {
        std::cout << "\n\n[Error]: too many characters in [URL], overflow protection | terminating program\n\n";
        return EXIT_FAILURE;
    }

    if (format.length() > 5) {
        std::cout << "\n\n[Error]: too many characters in [FORMAT], overflow protection | terminating program\n\n";
        return EXIT_FAILURE;
    }

    if (!isTheFormatSupported(format)) {
        std::cerr << "[Error]: unsupported format [" << format << "]\n";
        printUsage(argv[0]);
       return EXIT_FAILURE;
    }

    if (!checkDependenciesInstallation()) {
        std::cerr << "[Error]: dependencies not properly installed or not set properly in (path)\n";
        return EXIT_FAILURE;
    }

    string downloadPath = createOrGetDownloadFolder(customFolder);
    if (downloadPath.empty()) {
        return EXIT_FAILURE;
    }

     // ERROR conditionals --------------------------------------------

    // if it is a playlist?
    bool isPlaylist = isPlaylistURL(url);
    
    char actualCommand[0x800];
    if (isPlaylist) {

        // playlist download command
        snprintf(actualCommand, sizeof(actualCommand),
            "yt-dlp --extract-audio --audio-format %s --output \"%s/%%(playlist_index)s - %%(title)s.%%(ext)s\" \"%s\"",
            format.c_str(), downloadPath.c_str(), url.c_str());
        
        cout << "\n[PLAYLIST URL DETECTED]\n";
        cout << "Download URL: [" << url << "]\n";
        cout << "Audio format: [" << format << "]\n";
        cout << "Download folder: [" << downloadPath << "]\n";
        cout << "Files will be numbered by playlist order\n";
    } else {

        // single track download command
        snprintf(actualCommand, sizeof(actualCommand),
            "yt-dlp --extract-audio --audio-format %s --output \"%s/%%(title)s.%%(ext)s\" \"%s\"",
            format.c_str(), downloadPath.c_str(), url.c_str()
        );
       
        cout << "\n[SINGLE TRACK MODE]\n";
        cout << "Download URL: [" << url << "]\n";
        cout << "Audio format: [" << format << "]\n";
        cout << "Download folder: [" << downloadPath << "]\n";
    }

    cout << "\n[Running cmd]: [" << actualCommand << "]\n\n";

    int resultVal = system(actualCommand);
    if (resultVal == 0) {
        cout << "\n[SUCCESS]: Audio extraction completed in: " << downloadPath << "\n\n";
    } else {
        cout << "\n\n[Error]: Audio extraction failure | (exit code: " << resultVal << ")\n";
        cout << "Common issues:\n";
        cout << "- Invalid URL or private/restricted content [DRM]\n";
        cout << "- Network connectivity issues\n";
        cout << "- Insufficient permissions for download folder\n\n";

        return 1;
    }

    return EXIT_SUCCESS;
}
