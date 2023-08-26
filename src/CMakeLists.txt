add_executable(CHIP-8 main.cpp
        cpu.cpp
        cpu.h
        ram.cpp
        ram.h
        display.cpp
        display.h)
target_link_libraries(CHIP-8
        glfw
        glad)
