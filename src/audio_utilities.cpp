#include "audio_utilities.h"

#include <algorithm>
#include <cctype>

std::vector<std::string> supportedAudioFormats = { "mp3", "wav", "flac", "aac", "ogg", "m4a", "opus"};
// 24/xx - added opus

bool isTheFormatSupported(const std::string &format) {
    for (int inc = 0; inc < supportedAudioFormats.size(); inc += 1) { // too lazy for [n] external storage
        if (format == supportedAudioFormats[inc]) {
            return true;
        }
    }
    return false;
}

bool isAudioFile(const std::string &filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos) { return false; }
    
    std::string extension = filename.substr(dotPos + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    return isTheFormatSupported(extension);
}
