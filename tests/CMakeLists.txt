include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(Test-CHIP-8 main.cpp
        ../src/keyboard/Keyboard.cpp
        ../src/memory/Memory.cpp
        ../src/memory/Memory.test.cpp
        ../src/display/Display.cpp
        ../src/core/Cpu.cpp
        ../src/core/Cpu.test.cpp
)
set(CMAKE_EXE_LINKER_FLAGS "-static")
target_link_libraries(Test-CHIP-8
        PRIVATE SDL2main
        PRIVATE SDL2-static
        PRIVATE GTest::gtest_main)