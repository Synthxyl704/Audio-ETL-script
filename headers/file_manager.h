#ifndef HEADERS_FILE_MANAGER_H
#define HEADERS_FILE_MANAGER_H

#include <string>
#include <vector>
#include <cstdint>

struct SongInfo {
    std::string filename;
    std::string fullPath;
    std::string format;
    uintmax_t fileSize;
    std::string lastModified;
};

std::string createOrGetDownloadFolder(const std::string &customPath = "");
std::string getReadableFileSize(uintmax_t bytes);
void listDirectoryContents(const std::string &directoryPath, bool detailed);

#endif
