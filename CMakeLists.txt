cmake_minimum_required(VERSION 3.16)

project(AFF-Converter
    VERSION 1.0.0
    DESCRIPTION "Audio File Format (AFF) conversion script for automating music downloads"
    LANGUAGES CXX # basically C++ but alternate ver
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # strict std compliance thingy idk i forgot cmake

# if lack of specified build type, resort to "default = release ver"
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# compiler options sorcery
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -Wall")
elseif(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /Zi /W4")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG /W3")
endif()

# platform specs
if(WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN -DNOMINMAX)
elseif(UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
elseif(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15")
endif()

# sources executable (doesnt work for custom!)

# add_executable(argvmain argvmain.cpp)
# add_executable(argvmain src/argvmain.cpp src/audio_utilities.cpp src/dependency.cpp src/url_utilities.cpp src/file_manager.cpp src/song_manager.cpp src/metadata.cpp src/usage_helper.cpp)
add_executable(argvmain src/argvmain.cpp src/audio_utilities.cpp src/dependency.cpp src/url_utilities.cpp src/file_manager.cpp src/song_manager.cpp src/metadata.cpp src/usage_helper.cpp)

# add_executable(main src/argvmain.cpp src/audio_utilities.cpp src/dependency.cpp src/url_utilities.cpp src/file_manager.cpp src/song_manager.cpp src/metadata.cpp src/usage_helper.cpp)

# target_include_directories(argvmain PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(argvmain PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/headers)

# target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/headers)

# pplatform libraries
if(WIN32)
    target_link_libraries(argvmain PRIVATE ws2_32 winmm)
elseif(UNIX)
    target_link_libraries(argvmain PRIVATE pthread)
    if(NOT APPLE)
        target_link_libraries(argvmain PRIVATE dl)
    endif()
endif()

# dependency check for yt-dlp
# find_program(YT_DLP_EXECUTABLE yt-dlp)
# if(NOT YT_DLP_EXECUTABLE)
#     message(WARNING "yt-dlp not found in PATH. Please ensure it's installed for the application to work properly.")
# else()
#     message(STATUS "Found yt-dlp: ${YT_DLP_EXECUTABLE}")
# endif()

# honestly dont know what this does (installation directive)
install(TARGETS argvmain RUNTIME DESTINATION bin)

# dependency check helper o
# add_custom_target(check-deps
#     COMMAND ${CMAKE_COMMAND} -E echo "Checking system dependencies..."
#     COMMAND ${CMAKE_COMMAND} -E echo "Looking for yt-dlp..."
#     COMMAND yt-dlp --version || ${CMAKE_COMMAND} -E echo "yt-dlp not found - please install it"
#     COMMENT "Checking runtime dependencies"
# )

# CPack
include(CPack)
set(CPACK_PACKAGE_NAME "AFF-Converter")
set(CPACK_PACKAGE_VENDOR "idk_what_is_supposed_to_be_here_bro")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "audio download + conversion tool for terminal based usage")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
elseif(APPLE)
    set(CPACK_GENERATOR "TGZ;DragNDrop")
else()
    set(CPACK_GENERATOR "TGZ;DEB;RPM")
endif()

# print configuration summary
message(STATUS "")
message(STATUS "=== Build Configuration Summary ===")
message(STATUS "Project: AudioFileFormatConverter v${PROJECT_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "=====================================")
message(STATUS "")
