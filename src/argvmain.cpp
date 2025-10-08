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
    // friendship ended with mp3, opus is my best friend now

    signed int commandType = (-1);
         if (firstArg == "-smlist")     { commandType = 0; }
    else if (firstArg == "-rem")        { commandType = 1; }
    else if (firstArg == "-remMul")     { commandType = 2; }
    else if (firstArg == "-transcode")  { commandType = 3; }
    else if (firstArg == "-metamsc")    { commandType = 4; }
    else if (firstArg == "-help")       { commandType = 5; }
    else if (firstArg == "-search")     { commandType = 6;  }
    else if (firstArg.find("LFI_") != std::string::npos) 
                                        { commandType = 7; }

    switch (commandType) {
        case 0: // -smlist
        {
            string downloadPath = (argc >= 3) ? argv[2] : "";
            downloadPath = createOrGetDownloadFolder(downloadPath);

            if (downloadPath.empty()) {
                return EXIT_FAILURE;
            }

            listDownloadedSongs(downloadPath);
            return EXIT_SUCCESS;
        }
        
        case 1: // -rem
        {
            string downloadPath = (argc >= 3) ? argv[2] : "";
            downloadPath = createOrGetDownloadFolder(downloadPath);
            
            if (downloadPath.empty()) {
                return EXIT_FAILURE;
            }

            removeSong(downloadPath);
            return EXIT_SUCCESS;
        }

        case 2: // -remMul
        {
            string downloadPath = (argc >= 3) ? argv[2] : "";
            downloadPath = createOrGetDownloadFolder(downloadPath);

            if (downloadPath.empty()) {
                return EXIT_FAILURE;
            }

            removeMultipleSongs(downloadPath);
            return EXIT_SUCCESS;
        }

        case 3: // -transcode
        {
            string downloadPath = (argc >= 3) ? argv[2] : "";
            downloadPath = createOrGetDownloadFolder(downloadPath);
            
            if (downloadPath.empty()) {
                return EXIT_FAILURE;
            }

            transcodeSelectedFiles(downloadPath);
            return EXIT_SUCCESS;
        }
        
        case 4: // -metamsc
        {
            string downloadPath = (argc >= 3) ? argv[2] : "";
            downloadPath = createOrGetDownloadFolder(downloadPath);

            if (downloadPath.empty()) {
                return EXIT_FAILURE;
            }

            showSongMetadata(downloadPath);
            return EXIT_SUCCESS;
        }

        case 5: // -help
        {
            printUsage(argv[0]);
            return EXIT_SUCCESS;
        }

        case 6: // -LFI_t / -LFI_f commands
        {

           // -search <insert_query_token_here>
           // argv[2] = tokenQuery
           // argv[3] = directory = downloadPath

           string downloadPath = (argc >= 4) ? argv[3] : "downloads";
           searchForSong(downloadPath, static_cast<std::string>(argv[2]));

           return EXIT_SUCCESS;
        }

        case 7: 
        {
             // if (argv[2] == NULL || !argv[2]) {
            //     std::cerr << "\nFile/directory to analyse is not specified, specify in argv[2]\n";
            //     return EXIT_SUCCESS;
            // }

            // std::string downloadPath = argv[2];
            // string downloadPath = (argc >= 3) ? argv[2] : "";

            // CHANGE THE "downloads" IN CASE ITS SET TO YOUR DIRECTORY
            string downloadPath = (argc >= 3) ? argv[2] : "downloads";

            // if "t" is found, then detailed, else no
            bool detailed = (firstArg.find("t") != std::string::npos);
            listDirectoryContents(downloadPath, detailed);

            return EXIT_SUCCESS;
        }

        default:
            // n/a
            break;
    }

    // =============== END OF ADDED MORE COMMAND HANDLING ===============

    if (argc < 3 || argc > 4) {
        std::cerr << "\n((argc < 3 || argc > 4) || INVALID COMMAND: " << argv[1] <<  ")\n\n";

        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    const string url = argv[1];
    const string format = argv[2];
    const string customFolder = (argc == 4) ? argv[3] : "";

    // ============== ERROR conditionals ==============

    if (url.length() > 0x200 /* 0x400 */) { // CHANGE IN CASE YOU SOMEHOW HAVE A LARGER LINK
        cout << "\n\n[URL_INT_ERROR]: too many characters in [URL], overflow protection | terminating program\n\n";
        return EXIT_FAILURE;
    }

    if (format.length() > 5) {
        cout << "\n\n[CODEC_INT_ERROR]: too many characters in [FORMAT], overflow protection | terminating program\n\n";
        return EXIT_FAILURE;
    }

    if (!isTheFormatSupported(format) || isTheFormatSupported(format) == false)  {
        cerr << "\n[CODEC_ERROR]: unsupported format [" << format << "]\n\n";
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

std::getline(metaStream, metaLine);
std::vector<std::string> metaFields = split(metaLine, '|');

std::string title = "[UNKNOWN]";
std::string artist = "[UNKNOWN]";
std::string duration = "[UNKNOWN]";

if (metaFields.size() >= 3) {
    title = metaFields[0];
    artist = (!metaFields[1].empty()) ? metaFields[1] : "[ERROR_WHILE_FETCHING_MDT]";
    duration = metaFields[2];
} else {
    std::cerr << "[WARNING]: PCould not extract complete metadata for URL.\n";
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
        cout << "- Invalid URL or private/restricted content | [DRM]\n";
        cout << "- Network connectivity issues\n";
        cout << "- Insufficient permissions for download folder\n\n";

        return 1;
    }

    return EXIT_SUCCESS;
}
