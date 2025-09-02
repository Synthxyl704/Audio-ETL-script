#include "url_utilities.h"

bool isPlaylistURL(const std::string &url) {
    return (url.find("playlist") != std::string::npos ||     // youtube
            url.find("&list=") != std::string::npos ||      
            url.find("?list=") != std::string::npos ||
            url.find("/sets/") != std::string::npos);        // soundcloud
}