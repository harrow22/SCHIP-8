include(FetchContent)
FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG        release-2.28.2
)
FetchContent_MakeAvailable(SDL2)

add_executable(CHIP-8 main.cpp
        core/Cpu.cpp
        core/Cpu.h
        memory/Memory.cpp
        memory/Memory.h
        display/Display.cpp
        display/Display.h
        keyboard/Keyboard.cpp
        keyboard/Keyboard.h

)
set(CMAKE_EXE_LINKER_FLAGS "-static")
target_link_libraries(CHIP-8
        PRIVATE SDL2main
        PRIVATE SDL2-static)