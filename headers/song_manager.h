#ifndef HEADERS_SONG_MANAGER_H
#define HEADERS_SONG_MANAGER_H

#include <string>

void listDownloadedSongs(const std::string &downloadPath);

// cant get file path
// vector -> list all songs 1 <--> [X]
// "choice" type interactive

// removal
void removeSong(const std::string &downloadPath);
void isSongRedundant(const std::string &downloadPath, const std::string &songTitle);

// ---
void removeMultipleSongs(const std::string &downloadPath);

#endif 
