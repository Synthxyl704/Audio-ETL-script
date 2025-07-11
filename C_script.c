#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

volatile int errorCount = 100;

char *supportedAudioFormats[] = { "mp3", "wav", "flac", "aac", "ogg", "m4a", NULL };

int isTheFormatSupported(const char* format) {
    for (int inc = 0; supportedAudioFormats[inc] != NULL; inc += 1) {        // didnt know this was possible LOL
        if (strcmp(format, supportedAudioFormats[inc]) == 0) {               // if its a valid format from *sAF[] -> correct
            return true;
        }
    } 
    
    return false; // return NULL; doesnt work
}

int checkDependenciesInstallation() {
    // system(<cmd_here>); will actually run it in terminal and comply conditional[s] accordingly

    // check for yt-dlp
    if (system("which yt-dlp > /dev/null 2>&1") != 0) {
        printf("Error: yt-dlp is not installed or not in PATH\n");
        printf("Install with: pip install yt-dlp\n");

        return false;
    }
    
    // check for ffmpeg
    if (system("which ffmpeg > /dev/null 2>&1") != 0) {
        printf("Error: ffmpeg is not installed or not in PATH\n");
        printf("Install ffmpeg from: https://ffmpeg.org/download.html\n");

        return false;
    }
    
    return true;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        puts("");
        printf("[Usage]: %s <youtubeURL> <format>\n", argv[0]);

        printf("\n[Supported formats]: \n");
        for (int inc = 0; supportedAudioFormats[inc] != NULL; inc += 1) {
            printf("-> [%s]\n", supportedAudioFormats[inc]);
        }

        puts("");
        printf("[Example]: %s https://www.youtube.com/watch?v=dQw4w9WgXcQ <put_ur_file_format_here>\n", argv[0]);
            
        return EXIT_FAILURE;
    }
    
    const char *youtubeURL = argv[1];
    const char *format = argv[2];

    if (!isTheFormatSupported(format)) {
        fprintf(stderr, "[Error %d]: Unsupported format [%s]\n", errorCount, format);

        puts("");
        printf("[Usage]: %s <youtubeURL> <format>\n", argv[0]);

        printf("[Supported formats]: ");
        for (int inc = 0; supportedAudioFormats[inc] != NULL; inc += 1) {
            printf("[%s]\n", supportedAudioFormats[inc]);
        }

        puts("");
        printf("[Example]: %s https://www.youtube.com/watch?v=dQw4w9WgXcQ mp3\n", argv[0]);
            
        return EXIT_FAILURE;
    }


    if (!checkDependenciesInstallation()) {  // {{ checkDependenciesInstallation() == NULL }} is invalid since int -> pointer NULL
        fprintf(stderr, "%s", "[Error]: dependencies not properly installed there");

        return EXIT_FAILURE;
    }

    char actualCommand[0x400]; // 1024 but hex for no apparent reason (looks cool)

    snprintf(actualCommand, sizeof(actualCommand)
             ,"yt-dlp --extract-audio --audio-format %s --output \"%%(title)s.%%(ext)s\" \"%s\""
             ,format, youtubeURL);
    // snprintf is actually awesome

    printf("\n\nAudio extraction URL [YT/onlypls]: [%s]\n", youtubeURL);
    printf("Audio extraction file format: [%s]\n", format);
    printf("\n[Running cmd]: [%s]\n\n", actualCommand);
    
    int resultVal = system(actualCommand); // runs the cmd in terminal
    
    if (resultVal == 0) {
        printf("\n[Command execution = SUCCESS]: extracted audio in current directory path\n\n");
    } else {
        printf("\n\n[Error]: Audio extraction failure | (exit code: %d)\n\n", resultVal);
        return 1;
    }

    // (resultVal == 0) ? printf("[Command execution = SUCCESS]: extracted audio\n") 
    //                  : printf("[Error]: Audio extraction failure | (exit code: %d)\n", resultVal);
    
    return EXIT_SUCCESS;
}
