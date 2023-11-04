cmake_minimum_required(VERSION 3.26)
project(SCHIP-8
        VERSION 0.0
        DESCRIPTION "A CHIP-8 interpreter built in C++"
        LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 23)

find_package(SDL2 REQUIRED)
add_subdirectory(src)

option(TESTING "TESTING" OFF)
if (${TESTING})
    add_subdirectory(tests)
endif()

