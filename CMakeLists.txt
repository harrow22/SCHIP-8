cmake_minimum_required(VERSION 3.26)
project(CHIP-8
        VERSION 0.0
        DESCRIPTION "A CHIP-8 interpreter built in C++"
        LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(src)
add_subdirectory(libs)
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING)
        AND BUILD_TESTING)
    #add_subdirectory(tests)
endif()

