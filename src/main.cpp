#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <thread>
#include <SDL2/SDL.h>
#include "Core/Cpu.h"
#include "memory/Memory.h"
#include "display/Display.h"
#include "keyboard/Keyboard.h"

#define FRAME_RATE 60.0
#define INSTRUCTIONS_PER_SECOND 8000.0

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO < 0)) {
        std::cerr << "[Error] SDL failed to initialize." << std::endl;
        exit(0);
    }

    // setting up hardware
    Memory memory {};
    Display display {};
    Keyboard keyboard {};

    // font sprite data
    constexpr std::uint8_t font[] {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    // parsing command line input;
    double ips {INSTRUCTIONS_PER_SECOND};
    if (argc > 1) {
        while (*argv) {
            if (!std::strcmp(*argv, "-rom")) {
                try {
                    // get the file path
                    std::filesystem::path p {*(++argv)};
                    if (!std::filesystem::exists(p))
                        throw std::runtime_error("Invalid file path");

                    // open the file
                    std::ifstream rom{p, std::ios::binary};
                    if (!rom)
                        throw std::runtime_error("Error when opening file");

                    // write program into memory
                    char byte {};
                    for (int addr {0x200}; rom.get(byte); ++addr) {
                        memory.write(byte, addr);
                    }
                    // close the file
                    rom.close();
                } catch(std::exception& e) {
                    std::cout << "Ending CHIP-8 program encountered:\n"
                              << "\t[Error] Failed to open rom (" << e.what() << ")."
                              << std::endl;
                    exit(0);
                }
            } else if (!std::strcmp(*argv, "-ips")) {
                try {
                    std::string n {*(++argv)};
                    ips = std::stoi(n);
                } catch(std::exception& e) {
                    std::cout << "[Error] when reading input for command line argument '-cycles_per_frame'.\n"
                              << "Using default value of " << INSTRUCTIONS_PER_SECOND << " instructions-per-second encountered:"
                              << std::endl;
                }
            }
            ++argv;
        }
    }

    // writing the font into memory
    std::uint8_t addr {FONT_MEMORY_LOC};
    for (auto byte : font) {
        memory.write(byte, addr);
        addr += 1;
    }

    // main emulator loop
    Cpu cpu {ips, memory, display, keyboard};
    if (display.on()) {
        SDL_Event e;
        bool quit {false};
        auto cycleLength {std::chrono::duration<double>(1.0 / ips)};

        //std::cout << dt << std::endl;
        //std::string line;
        while (!quit) {
            /* UNCOMMENT FOR DEBUGGING
            std::cout << "Press any key(s) and enter to step ('q' to quit): ";
            std::getline(std::cin, line);
            if (line == "q")
                quit = true;
            */

            cpu.cycle();

            /*
            std::cout << "AFTER STEP:\n";
            std::cout << "MEMORY POINTED AT: " << (int) memory.read(cpu.i) << std::endl;
            std::cout << "PC: " << cpu.pc << " CIR: " << std::hex << cpu.cir
                      << " IR: " << cpu.i << std::endl;
            for (int i {0}; i != 16; ++i) {
                std::cout << "REG V[" << i << "]: " << (int) cpu.reg[i] << std::endl;
            }
             */

            std::this_thread::sleep_for(std::chrono::duration_cast<std::chrono::microseconds>(cycleLength));
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                } else if (cpu.wkp) {
                    if (e.type == SDL_KEYDOWN) {
                        keyboard.onKeyDown(e.key.keysym.scancode);
                    } else if (e.type == SDL_KEYUP) {
                        keyboard.onKeyUp(e.key.keysym.scancode);
                    }
                }
            }
        }
        display.off();
    }

    SDL_Quit();
    return 0;
}
