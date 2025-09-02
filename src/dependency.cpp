#include "dependency.h"
#include <iostream>
#include <cstdlib>

using namespace std;

bool checkDependenciesInstallation() {

    // check for yt-dlp
    if (system("which yt-dlp > /dev/null 2>&1") != 0) {
        cout << "[Error]: yt-dlp is not installed or not in PATH\n";
        cout << "Install with: pip install yt-dlp\n" << std::endl;

        // std::cout << "Do you wish to download it now? | [yY/nN]" << std::endl;

        return false;
    }
    
    // check for ffmpeg
    if (system("which ffmpeg > /dev/null 2>&1") != 0) {
        cout << "[Error]: ffmpeg is not installed or not set in correct path for execution\n";
        cout << "Install ffmpeg from: https://ffmpeg.org/download.html\n";
        
        cout << "Or if you are on Arch/Debian based linux distros just execute | [sudo pacman -S ffmpeg]";
        cout << "Arch based distributions download: sudo pacman -S ffmpeg -> ffmpeg --version\n";
        cout << "Debian based distributions download: sudo apt install ffmpeg -> ffmpeg --version\n";
        return false;
    }

    return true;
}
