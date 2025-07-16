#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <filesystem>
#include <algorithm>

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

void printUsage(const char *programName) {
    cout << "\n[Usage]: " << programName << " <URL> <format> [download_folder]\n";
    cout << "\n[Supported formats]: \n";
    for (int inc = 0; inc < supportedAudioFormats.size(); inc += 1) {
        cout << "-> [" << supportedAudioFormats[inc] << "]\n";
    }
    cout << "\n[Supported platforms]: YouTube, SoundCloud (Spotify not supported due to DRM)\n";
    cout << "\n[Examples]:\n";
    cout << "  Single video: " << programName << " https://www.youtube.com/watch?v=dQw4w9WgXcQ mp3\n";
    cout << "  Playlist: " << programName << " https://www.youtube.com/playlist?list=PLxxxxxxx mp3\n";
    cout << "  Custom folder: " << programName << " https://youtu.be/example mp3 /path/to/folder_or_directory\n";
    cout << "  SoundCloud: " << programName << " https://soundcloud.com/user/track mp3\n\n";
}

int main(int argc, char **argv) {
    if (argc < 3 || argc > 4) {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    const std::string url = argv[1];
    const std::string format = argv[2];
    const std::string customFolder = (argc == 4) ? argv[3] : "";

    // conditionals

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
            format.c_str(), downloadPath.c_str(), url.c_str());
        
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
