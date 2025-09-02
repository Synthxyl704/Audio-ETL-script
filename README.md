# AFF ETL script
Nothing fancy, just a simple intermediate ETL-type script for converting youtube, soundcloud & more playlists and videos to audio file formats set to your directory. <br>
<hr>

> [!NOTE]
> This script is still under incomplete construction, it is usable though for daily use - yes. <br>
> Bugs + compile fixes will be done soon with more functionality such as removal and stuff. <br>

<hr>

## Run Locally

Clone the project using this cmd in bash:
```bash
git clone https://github.com/Synthxyl704/Audio-ETL-script
```

CMake guide (simple, execute sequentially if you wish to)
```
-> cd <directory of storage>
-> mkdir build
-> cd build
-> CMake ..
-> make
-> <execute target main file, may be different than just "main">
```

Terminal compilation guide:
```
-> cd <directory of storage>
-> gcc argvmain.c -o main
-> ./main <link_to_youtube_urlL> <audio_file_format>
```

## Commands usage ancilliary:
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

-> 02/09/25 [FIXED] | `-smlist` listed dates may be incorrect at times for certain/random songs. [ !Highly unlikely! ]
-> `-LFI_t/f` added which will show songs/music metadata from your directory of storage for easier knowing of which file is consuming data/etc

## License

[CC 4.0]([https://creativecommons.org/licenses/by/4.0/deed.en)

<h3>IsoAris -REVITALIZE-</h3>
