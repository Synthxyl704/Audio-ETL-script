#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

using namespace std;

[[ maybe_unused ]] volatile int errorCount = 100;                       // why is this line even a thing
vector<std::string> supportedAudioFormats = { "mp3", "wav", "flac", "aac", "ogg", "m4a" };

bool isTheFormatSupported(const string &format) {
    for (int inc = 0; inc < supportedAudioFormats.size(); inc += 1) {   // didnt know this was possible LOL
        if (format == supportedAudioFormats[inc]) {                     // if its a valid format from sAF[] -> correct
            return true;
        }
    }
    return false;                                                       // return NULL; doesnt work
}

bool checkDependenciesInstallation() {

    // system(<cmd_here>); will actually run it in terminal and comply conditional[s] accordingly
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

int main(int argc, char **argv) {
    if (argc != 3) {
        cout << std::endl;
        cout << "[Usage]: " << argv[0] << " <youtubeURL> <format>\n";
        cout << "\n[Supported formats]: \n";
        for (int inc = 0; inc < supportedAudioFormats.size(); inc += 1) {
            cout << "-> [" << supportedAudioFormats[inc] << "]\n";
        }
        cout << "\n";
        cout << "[Example]: " << argv[0] << " https://www.youtube.com/watch?v=dQw4w9WgXcQ <put_ur_file_format_here>\n";
                                              // RICKROLL LMFAO
        return EXIT_FAILURE;
    }

    const string youtubeURL = argv[1];

    if (sizeof(youtubeURL) > 0x400) {
        std::cout << "\n\ntoo many characters, overflow conduction | terminating program\n\n";

        return EXIT_FAILURE;
    }

    const std::string format = argv[2];

    if (sizeof(format) > 5) {
        std::cout << "\n\ntoo many characters, overflow conduction | terminating program\n\n";

        return EXIT_FAILURE;
    }

    if (!isTheFormatSupported(format)) {
        std::cerr << "[Error " << errorCount << "]: unsupported format [" << format << "]\n";

        errorCount += 1;

        cout << "\n";
        cout << "[Usage]: " << argv[0] << " <youtubeURL> <format>\n";
        cout << "[Supported formats]: ";
        for (int inc = 0; inc < supportedAudioFormats.size(); inc += 1) {
            cout << "[" << supportedAudioFormats[inc] << "]\n";
        }
        cout << "\n";
        cout << "[Example]: " << argv[0] << " https://youtu.be/Vqhv-OisoMU?si=MbPOazgla7AAxoOY mp3/wav/ogg/supported_format\n";
                                              // goodnight ojousama
        return EXIT_FAILURE;
    }

    if (!checkDependenciesInstallation()) { // {{ checkDependenciesInstallation() == NULL }} is invalid since int -> pointer NULL
        std::cerr << "[Error]: dependencies not properly installed there";
        return EXIT_FAILURE;
    }

    char actualCommand[0x400];                                  // 1024 but hex for no apparent reason (looks cool)
    snprintf(actualCommand
        ,sizeof(actualCommand)
        ,"yt-dlp --extract-audio --audio-format %s --output \"%%(title)s.%%(ext)s\" \"%s\"" // MAIN COMMAND
        ,format.c_str(), youtubeURL.c_str());
                                                                // snprintf is actually awesome

    cout << "\n\nAudio extraction URL [YT only pls]: [" << youtubeURL << "]\n";
    cout << "Audio extraction file format: [" << format << "]\n";
    cout << "\n[Running cmd]: [" << actualCommand << "]\n\n";

    int resultVal = system(actualCommand);                      // runs the cmd in terminal
    if (resultVal == 0) {
        cout << "\n[Command execution = SUCCESS]: extracted audio in current directory path\n\n";
    } else {
        cout << "\n\n[Error]: Audio extraction failure | (exit code: " << resultVal << ")\n\n";
        return 1;
    }

    // (resultVal == 0) ? printf("[Command execution = SUCCESS]: extracted audio\n")
    //                  : printf("[Error]: Audio extraction failure | (exit code: %d)\n", resultVal);

    return EXIT_SUCCESS;
}
