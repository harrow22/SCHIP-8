cmake_minimum_required(VERSION 3.26)
project(CHIP-8
        VERSION 0.0
        DESCRIPTION "A CHIP-8 interpreter built in C++"
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(src)
#add_subdirectory(tests)
