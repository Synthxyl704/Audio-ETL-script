#ifndef AUDIO_UTILITIES_H
#define AUDIO_UTILITIES_H

#include <string>
#include <vector>

extern std::vector<std::string> supportedAudioFormats;

bool isTheFormatSupported(const std::string &format);
bool isAudioFile(const std::string &filename);

#endif