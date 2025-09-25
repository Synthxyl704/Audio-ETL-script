#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <array>
#include <memory>
#include <sstream>

#include "headers/audio_utilities.h"
#include "headers/dependency.h"
#include "headers/url_utilities.h"
#include "headers/file_manager.h"
#include "headers/song_manager.h"
#include "headers/metadata.h"
#include "headers/usage_helper.h"

// #include "audio_utilities.cpp"
// #include "dependency.cpp"
// #include "url_utilities.cpp"
// #include "file_manager.cpp"
// #include "song_manager.cpp"
// #include "metadata.cpp"
// #include "usage_helper.cpp"

using namespace std; // im sorry

std::string execCommand(const std::string& cmd) {
    std::array<char, 4096> buffer;
    std::string result;

    using PCloseFn = int (*)(FILE*); // tacit warning suppression
    std::unique_ptr<FILE, PCloseFn> pipe(popen(cmd.c_str(), "r"), pclose);

    if (!pipe) {
        throw std::runtime_error("[R-Error]: popen() failed!");
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    return result;
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        parts.push_back(item);
    }
    return parts;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    // =============== MAIN ARGV HANDLING ===============

    // FEATS!!!
    // -> | -smlist 
    // -> | -rem 
    // -> | -metamsc
    // -> | -remul? (i wrote the f(x) for it, need name for first argument)

    // -x_ver_i_wanna_add-
    // -> | -transmux
    // -> | -demux? 
    // -> | -transcode

    // TODO: change this later for argv[3+] integration somehow, parse tree?
    //       make it a cli?
    //       nvm its a feature now deal with it no parse tree no shit 
    //       ftxui is too confusing

    string firstArg = argv[1];

    // ./main <url> mp3
    // ./main -

    if (firstArg == "-smlist") {
        string downloadPath = (argc >= 3) ? argv[2] : "";

        downloadPath = createOrGetDownloadFolder(downloadPath);

        if (downloadPath.empty()) {
            return EXIT_FAILURE;
        }

        listDownloadedSongs(downloadPath);
        return EXIT_SUCCESS;
    }
    
    if (firstArg == "-rem") {
        string downloadPath = (argc >= 3) ? argv[2] : "";

        downloadPath = createOrGetDownloadFolder(downloadPath);
        
        if (downloadPath.empty()) {
            return EXIT_FAILURE;
        }

        removeSong(downloadPath);
        return EXIT_SUCCESS;
    }

    // void removeMultipleSongs(const std::string &downloadPath);

    if (firstArg == "-remomul") { /* CHANGE THIS CMD LATER */
        string downloadPath = (argc >= 3) ? argv[2] : "";
        downloadPath = createOrGetDownloadFolder(downloadPath);


        if (downloadPath.empty()) {
            return EXIT_FAILURE;
            // std::exit(EXIT_FAILURE);
        }

        removeMultipleSongs(downloadPath);

        // if (!removeMultipleSongs || /* removeMultipleSongs == NULL */) {
            // return EXIT_FAILURE;
        // }
    }

    if (firstArg == "-transcode") {
        string downloadPath = (argc >= 3) ? argv[2] : "";

        downloadPath = createOrGetDownloadFolder(downloadPath);
        
        if (downloadPath.empty()) {
            return EXIT_FAILURE;
            // std::exit(EXIT_FAILURE);
        }

        transcodeSelectedFiles(downloadPath);

        return EXIT_SUCCESS;
    }
    
    if (firstArg == "-metamsc") {
        string downloadPath = (argc >= 3) ? argv[2] : "";
        downloadPath = createOrGetDownloadFolder(downloadPath);

        if (downloadPath.empty()) {
            return EXIT_FAILURE;
        }

        showSongMetadata(downloadPath);
        return EXIT_SUCCESS;
    }

    if (firstArg == "-help") {
        printUsage(argv[0]);
        return EXIT_SUCCESS;
    }    

    //refactor manually fixed for dynamic boolean changes to LFI_[x] in argv[2]
    std::string subStr = "LFI_";

    if (firstArg.find(subStr) != std::string::npos) {
        if (argv[2] == NULL || !argv[2]) {
            std::cerr << "\nFile/directory to analyse is not specified, specify in argv[2]\n";
            return EXIT_SUCCESS;
        }

        std::string downloadPath = argv[2];

        bool detailed = (firstArg.find("t") != std::string::npos);
        listDirectoryContents(downloadPath, detailed);

        return EXIT_SUCCESS;
    }

    // i cant stop using std::[x] LOL

    // my refactor conditional version in case shit breaks
    // if (firstArg == "-LFI_f") {
    //     if (argv[2] == NULL || !argv[2]) { 
    //         std::cerr << "\nFile/director to analyse is not specified, specify in argv[2]\n";
    //         return EXIT_SUCCESS;
    //     }

    //     // listDirectoryContents(const string &directoryPath, bool detailed = true)
    //     string downloadPath = (argc >= 3) ? argv[2] : "";
    //     listDirectoryContents(downloadPath, false);
    
    //     return EXIT_SUCCESS;
    // }

    // if (firstArg == "-LFI_t") {
    //     if (argv[2] == NULL || !argv[2]) { 
    //         std::cerr << "\nFile/director to analyse is not specified, specify in argv[2]\n";
    //         return EXIT_SUCCESS;
    //     }

    //     string downloadPath = (argc >= 3) ? argv[2] : "";
    //     listDirectoryContents(downloadPath, true);

    //     return EXIT_SUCCESS;
    // }

    // =============== END OF ADDED MORE COMMAND HANDLING ===============

    if (argc < 3 || argc > 4) {
        std::cerr << "((argc < 3 || argc > 4) || INVALID COMMAND: " << argv[1] <<  ")\n\n";

        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    const string url = argv[1];
    const string format = argv[2];
    const string customFolder = (argc == 4) ? argv[3] : "";

    // ============== ERROR conditionals ==============

    if (url.length() > 0x400) {
        cout << "\n\n[Error]: too many characters in [URL], overflow protection | terminating program\n\n";
        return EXIT_FAILURE;
    }

    if (format.length() > 5) {
        cout << "\n\n[Error]: too many characters in [FORMAT], overflow protection | terminating program\n\n";
        return EXIT_FAILURE;
    }

    if (!isTheFormatSupported(format) || isTheFormatSupported(format) == false)  {
        cerr << "[Error]: unsupported format [" << format << "]\n";
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    if (!checkDependenciesInstallation()) {
        cerr << "[Error]: dependencies not properly installed or not set properly in (path)\n";
        return EXIT_FAILURE;
    }

    // const std::string commandForRedundancyCheck = argv[1]; // url     

std::string metadataCmd = "yt-dlp --no-warnings --print \"%(title)s|%(artist)s|%(duration)s\" \"" + url + "\"";
std::string metadataOutput = execCommand(metadataCmd);

std::istringstream metaStream(metadataOutput);
std::string metaLine;
std::getline(metaStream, metaLine);  // Only the first line matters
std::vector<std::string> metaFields = split(metaLine, '|');

std::string title = "(unknown)";
std::string artist = "(unknown)";
std::string duration = "(unknown)";

if (metaFields.size() >= 3) {
    title = metaFields[0];
    artist = !metaFields[1].empty() ? metaFields[1] : "(unknown)";
    duration = metaFields[2];
} else {
    std::cerr << "[Warning]: Could not extract complete metadata for URL.\n";
}

cout << "\n=== [TRACK METADATA] ===\n";
cout << "Title   : " << title << "\n";
cout << "Artist  : " << artist << "\n";
cout << "Duration: " << duration << " seconds\n";
cout << "========================\n";

    // ============== END OF CHECKING THINGS ==============

    string downloadPath = createOrGetDownloadFolder(customFolder);
    if (downloadPath.empty()) {
        return EXIT_FAILURE;
    }

    isSongRedundant(downloadPath, title); 

    // if it is a playlist?
    bool isPlaylist = isPlaylistURL(url);
    
    char actualCommand[0x800];
    if (isPlaylist) {

        // playlist download command
        snprintf(actualCommand, sizeof(actualCommand),
            "yt-dlp --extract-audio --audio-format %s --output \"%s/%%(playlist_index)s - %%(title)s.%%(ext)s\" \"%s\"",
            format.c_str(), downloadPath.c_str(), url.c_str());
            // i like C-style printing but damn op cascading is hot 
        
        cout << "\n[PLAYLIST URL DETECTED]\n";
        cout << "Download URL: [" << url << "]\n";
        cout << "Audio format: [" << format << "]\n";
        cout << "Download folder: [" << downloadPath << "]\n";
        cout << "Files will be numbered by playlist order\n";
    } else {

        snprintf(actualCommand, sizeof(actualCommand),
            "yt-dlp --extract-audio --audio-format %s --output \"%s/%%(title)s.%%(ext)s\" \"%s\"",
            format.c_str(), downloadPath.c_str(), url.c_str()
            // single track download command
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
        cout << "- Invalid URL or private/restricted content | [DRM]\n";
        cout << "- Network connectivity issues\n";
        cout << "- Insufficient permissions for download folder\n\n";

        return 1;
    }

    return EXIT_SUCCESS;
}
