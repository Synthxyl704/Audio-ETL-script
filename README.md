# AFF ETL script
Nothing fancy, just a simple intermediate ETL-type script for converting youtube, soundcloud & more playlists and videos to audio file formats set to your directory. <br>
<hr>

> [!NOTE]
> This script is mostly done with what it is supposed to do, please do message me bugs/any tweaks to make, if I am alive I will definitely respond to you back. <br>
> Some more fun features will still be added as passion features because I honestly had a lot of fun making this. <br>

<hr>

## RTU - Run Locally

Clone the project using this cmd in bash:
```bash
git clone https://github.com/Synthxyl704/Audio-ETL-script
```

CMake guide (simple, execute sequentially if you wish to)
```
-> cd <directory of storage>
-> mkdir build
-> cd build
-> cmake ..
-> make
-> <execute target main file, may be different than just "main">
```

Terminal compilation guide:
```
-> cd <directory of storage>
-> g++ -I. -Iheaders src/argvmain.cpp src/audio_utilities.cpp src/dependency.cpp src/url_utilities.cpp src/file_manager.cpp src/song_manager.cpp src/metadata.cpp src/usage_helper.cpp -o argvmain
-> ./main <link_to_youtube_urlL> <audio_file_format>
```

## Commands usage exposition:
1. Downloading a song/music/playlist -
```
./main <link to the video/music/playlist> <format> <custom folder/directory>
```
if the `argv[3]` is not specified, it will auto create a "downloads" named folder and download it there, wont just download anywhere or in the program itself.

2. Listing downloaded songs/music -
```
./main -smlist
```

3. Deleting a downloaded song/music piece from terminal itself -
```
./main -rem
```

4. Wanna see song metadata without filling up your terminal screen?
```
./main -metamsc
```

5. Folder/directory metadata (space allocation, usage, file formats listing, etc)
```
./main -LFI_t <folder/directory> // THIS WILL SHOW DETAILED INFO
./main -LFI_f <folder/directory> // THIS WILL NOT SHOW DETAILED INFO
```

<hr>

## UPD/RR logs:

-> 02/09/25 [FIXED] | `-smlist` listed dates may be incorrect at times for certain/random songs. [ !Highly unlikely! ] <br>
-> `-LFI_t` is restricted to only showing you the detailed version of directory metadata, anything else that isnt 't' will automatically show you the non-detailed version.
> [!NOTE]
> If you wish for this flexibility truncation, please uncomment the conditional code block (~line 135) in `argvmain.cpp` and comment/remove the standard refactor.

## License

[CC 4.0]([https://creativecommons.org/licenses/by/4.0/deed.en)

<h3>IsoAris -REVITALIZE-</h3>
