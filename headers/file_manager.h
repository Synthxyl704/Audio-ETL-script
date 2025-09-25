#ifndef HEADERS_FILE_MANAGER_H
#define HEADERS_FILE_MANAGER_H

#include <string>
#include <vector>
#include <cstdint>
#include <filesystem>  

using namespace std;
namespace fs = std::filesystem; 

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

// codec shit
void transcodeSelectedFiles(const std::string& directoryPath);
std::vector<fs::path> listAvailableFiles(const std::string& directoryPath);

#endif
