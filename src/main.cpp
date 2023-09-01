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

#define CYCLES_PER_FRAME 10.0
#define FRAME_RATE 60.0

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO < 0)) {
        std::cerr << "[Error] SDL failed to initialize." << std::endl;
        exit(0);
    }

    // setting up hardware
    Memory memory {};
    Display display {};
    Keyboard keyboard {};
    Cpu cpu {memory, display, keyboard};

    // settings
    double cyclesPerFrame {CYCLES_PER_FRAME};
    bool debugging {false};

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

    // superchip font
    constexpr std::uint8_t schipfont[] {
            0xFF, 0xFF, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xFF, 0xFF, // 0
            0x18, 0x78, 0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0xFF, 0xFF, // 1
            0xFF, 0xFF, 0x03, 0x03, 0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, // 2
            0xFF, 0xFF, 0x03, 0x03, 0xFF, 0xFF, 0x03, 0x03, 0xFF, 0xFF, // 3
            0xC3, 0xC3, 0xC3, 0xC3, 0xFF, 0xFF, 0x03, 0x03, 0x03, 0x03, // 4
            0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, 0x03, 0x03, 0xFF, 0xFF, // 5
            0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, 0xC3, 0xC3, 0xFF, 0xFF, // 6
            0xFF, 0xFF, 0x03, 0x03, 0x06, 0x0C, 0x18, 0x18, 0x18, 0x18, // 7
            0xFF, 0xFF, 0xC3, 0xC3, 0xFF, 0xFF, 0xC3, 0xC3, 0xFF, 0xFF, // 8
            0xFF, 0xFF, 0xC3, 0xC3, 0xFF, 0xFF, 0x03, 0x03, 0xFF, 0xFF, // 9
            0x7E, 0xFF, 0xC3, 0xC3, 0xC3, 0xFF, 0xFF, 0xC3, 0xC3, 0xC3, // A
            0xFC, 0xFC, 0xC3, 0xC3, 0xFC, 0xFC, 0xC3, 0xC3, 0xFC, 0xFC, // B
            0x3C, 0xFF, 0xC3, 0xC0, 0xC0, 0xC0, 0xC0, 0xC3, 0xFF, 0x3C, // C
            0xFC, 0xFE, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xFE, 0xFC, // D
            0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, // E
            0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, 0xC0, 0xC0, 0xC0, 0xC0  // F
    };

    // parsing command
    if (argc > 1) {
        ++argv; // skip first arg
        while (*argv) {
            if (!std::strcmp(*argv, "-rom")) {
                try {
                    // get the file path
                    std::filesystem::path p {*(++argv)};
                    if (!std::filesystem::exists(p))
                        throw std::runtime_error("Could not find file from path");

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
                    std::cout << "[Error] Failed to open rom (" << e.what() << ")."
                              << "\nEnding CHIP-8 program..."
                              << std::endl;
                    exit(0);
                }
            } else if (!std::strcmp(*argv, "-ips")) {
                try {
                    std::string n {*(++argv)};
                    cyclesPerFrame = std::stoi(n);
                } catch(std::exception& e) {
                    std::cout << "[Error] Could not read input for command line argument '-cycles_per_frame'."
                              << "\n\tUsing default value of " << cyclesPerFrame << " CYCLES/FRAME."
                              << std::endl;
                }
            } else if (!std::strcmp(*argv, "-superchip")) {
                cpu.superChip();
            } else if (!std::strcmp(*argv, "-quirk")) {
                std::string quirk {*(++argv)};
                if (!cpu.setQuirk(quirk))
                    std::cout << "[Error] Quirk setting '" << quirk << "' not found. "
                              << "\n\tTo set a quirk use format: -quirk name=bool"
                              << "\n\tAvailable quirk settings and their defaults are:"
                              << "\n\t\tvf_reset=true \t[clear vF after vx |= vy, vx &= vy, and vx ^= vy.]"
                              << "\n\t\tmemory=true \t[load and store operations increment i.]"
                              << "\n\t\tshifting=false \t[<<= and >>= modify vx in place and ignore vy.]"
                              << "\n\t\tjumping=false \t[4 high bits of target address determines the offset register of jump0 "
                              << "instead of v0.]"
                              <<  "\n\t\tioverflow=false \t[Fx1E sets Vf=0 on overflow of I = I + Vx]"<< std::endl;
                std::cout << "*NOTE: -quirk and -superchip will override each other, precedence matters!*" <<std::endl;
            } else if (!std::strcmp(*argv, "-debug")) {
                debugging = true;
            } else {
                std::cout << "[Error] Unrecognized command line argument: " << *argv
                          << "\n\tAvailable arguments are: -rom -cycles_per_frame -superchip -quirk"
                          << std::endl;
            }
            ++argv;
        }
    }

    // writing the font into memory
    std::uint8_t addr {FONT_MEMORY_LOC};
    for (auto byte : font)
        memory.write(byte, addr++);

    // writing the superchip font into memory
    addr = BIG_FONT_MEMORY_LOC;
    for (auto byte : schipfont)
        memory.write(byte, addr++);


    // main emulator loop
    if (display.on()) {
        SDL_Event e;
        bool quit {false};
        int frameTime {static_cast<int>(1 / FRAME_RATE * 1e3)};
        long long tickCount {0};

        // TO ENTER/EXIT DEBUG MODE PRESS 'I' (QWERTY)
        // TO STEP IN DEBUG MODE PRESS 'O' (QWERTY)
        while (!quit) {
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                } else if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_I) {
                    debugging = true;
                } else if (cpu.waitingForKeypress) {
                    if (e.type == SDL_KEYDOWN) {
                        keyboard.onKeyDown(e.key.keysym.scancode);
                    } else if (e.type == SDL_KEYUP) {
                        keyboard.onKeyUp(e.key.keysym.scancode);
                    }
                }
            }

            for (int i {0}; i != cyclesPerFrame && !quit; ++i) {
                cpu.cycle();
                ++tickCount;

                if (debugging) {
                    std::cout << "TICK [" << std::dec << tickCount << "]:\n";
                    std::cout << "\tINSTRUCTION EXECUTED: " << std::hex << cpu.cir
                              << "\n\tPC: " << cpu.pc << " | IR: " << cpu.i << std::endl;
                    for (int r {0}; r != 16; ++r) {
                        std::cout << "\tREG V[" << r << "]: " << (int) cpu.reg[r] << std::endl;
                    }

                    while (!quit && debugging) {
                        SDL_WaitEvent(&e);
                        if (e.type == SDL_QUIT)
                            quit = true;
                        else if (e.type == SDL_KEYDOWN) {
                            if (e.key.keysym.scancode == SDL_SCANCODE_I)
                                debugging = false;
                            else if (e.key.keysym.scancode == SDL_SCANCODE_O)
                                break;
                        }
                    }
                }
            }

            cpu.frameUpdate();
            SDL_Delay(frameTime);
        }
        display.off();
    }

    SDL_Quit();
    return 0;
}
