cmake_minimum_required(VERSION 3.26)
project(SCHIP-8
        VERSION 0.0
        DESCRIPTION "A CHIP-8 interpreter built in C++"
        LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 23)

option(AUTO_FETCH_SDL2 "AUTO_FETCH_SDL2" ON)
option(TESTING "TESTING" OFF)

find_package(SDL2 REQUIRED)
set(SDL2_FETCHED OFF)

if(NOT SDL2_FOUND AND AUTO_FETCH_SDL2)
    FetchContent_Declare(
            SDL
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG release-2.28.4
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
    )
    message(STATUS "Fetching SDL2...")
    FetchContent_MakeAvailable(SDL)
    set_property(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/_deps/sdl-src" PROPERTY EXCLUDE_FROM_ALL TRUE)

    message(STATUS "Fetched SDL2")
    set(SDL2_FETCHED ON)
endif()

if (SDL2_FOUND OR SDL2_FETCHED)
    add_subdirectory(src)
else()
    message(FATAL_ERROR "SDL2 dependency not available. Either specify -DCMAKE_PREFIX_PATH=path/to/SDL2 or AUTO_FETCH_SDL2=ON")
endif()

if (TESTING)

endif()
add_subdirectory(tests)
