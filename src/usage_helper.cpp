#include "usage_helper.h"
#include "audio_utilities.h"

#include <iostream>

using namespace std;

// -help is automatically set to ./<prog>

void printUsage(const char *programName) {
    cout << "\n[HELP]: " << programName << " -help" << std::endl; 

    cout << "\n[Usage]: " << programName << " <URL> <format> [download_folder]\n";
    cout << "     OR: " << programName << " -smlist [download_folder]\n";
    cout << "     OR: " << programName << " -rem [download_folder]\n";
    cout << "     OR: " << programName << " -metamsc [download_folder]\n";

    cout << "\n[Supported formats]: \n";
    for (int inc = 0; inc < supportedAudioFormats.size(); inc += 1) {
        cout << "-> [" << supportedAudioFormats[inc] << "]\n";
    }

    cout << "\n[Supported platforms]: YouTube, YT-Music, SoundCloud, Google Drive, Bandcamp, Mixcloud, etc.:\n";
    cout << "[Unsupported platforms]: Spotify, TIDAL, Nonoki, etc. (DRM restrictions)\n";

    cout << "\n[Examples]:\n";

    for (int inc = 0; inc < 10 + 1; inc += 1) {
        std::cout << "-";
    }

    cout << endl;
    cout << "Single video: " << programName << " https://www.youtube.com/watch?v=dQw4w9WgXcQ mp3\n";
    cout << "Playlist: " << programName << " https://www.youtube.com/playlist?list=PLxxxxxxx mp3\n";
    cout << "Custom folder: " << programName << " https://youtu.be/example mp3 /path/to/folder_or_directory\n";
    cout << "SoundCloud: " << programName << " https://soundcloud.com/user/track mp3\n";

    cout << std::endl;

    cout << "List song(s) info: " << programName << " -smlist\n";
    cout << "Remove a song: " << programName << " -rem\n";
    cout << "Show specific song metadata: " << programName << " -metamsc\n";
    cout << "Show detail (set) file data: " << programName << " -LFI_t";
    cout << "Show undetailed (set) file data: " << programName << " -LFI_f";
}

// i shouldnt have allocated an entire header for this...
