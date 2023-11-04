#include <string>
#include <string_view>
#include <iostream>
#include <format>
#include "SDL.h"
#include "interpreter//Interpreter.h"
#include "memory/Memory.h"
#include "display/Display.h"
#include "keyboard/Keyboard.h"

bool startup(Display& display, bool romLoaded)
{
    if (!romLoaded) {
        std::cerr << "error: no rom specified, exiting...\n";
        return false;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << std::format("SDL_Error: {:s}\n", SDL_GetError());
        return false;
    }

    if (!display.on()) {
        std::cout << "Exiting...\n";
        return false;
    }

    return true;
}

int main(int argc, char** argv) {
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

    // setting up hardware
    Memory memory {};
    Display display {};
    Keyboard keyboard {};
    Interpreter interpreter {memory, display, keyboard};

    // settings
    double cycles_per_frame {20};
    bool debugging {false};
    bool romLoaded {false};
    std::string mode;

    // command line parsing
    using namespace std::string_view_literals;
    for (int i {1}; i < argc; ++i) {
        bool hasNext {i + 1 != argc};
        if (argv[i] == "-rom"sv and hasNext) {
            romLoaded = memory.load(argv[++i]);
        } else if (argv[i] == "--mode"sv and hasNext) {
            mode = argv[++i];
        } else if (argv[i] == "-quirk"sv and hasNext) {
            if (!interpreter.setQuirk(argv[++i]))
                std::cerr << std::format("error: failed to read interpreter '-quirk {:s}' option.\n", argv[i]);
        } else if (argv[i] == "-cycles_per_frame"sv and hasNext) {
            try {
                std::string n {argv[++i]};
                cycles_per_frame = std::stoi(n);
            } catch (std::exception& e) {
                std::cerr << std::format("error: failed to read integer for '-cycles_per_frame' option, using default={:f}.\n", cycles_per_frame);
            }
        } else if (argv[i] == "-debug"sv) {
            debugging = true;
        } else {
            std::cerr << std::format("error: unrecognized command line argument '{:s}'.\n", argv[i]);
        }
    }

    if (!mode.empty()) {
        if (interpreter.setMode(mode))
            std::cout << std::format("Running emulator in '{:s}' mode!\n", mode);
        else
            std::cerr << "error: failed to read interpreter '-mode' option, using default=chip8.\n";
    }

    // main emulator loop
    if (startup(display, romLoaded)) {
        SDL_Event e;
        bool quit {false};

        int frameLength {static_cast<int>(1.0 / 60.0 * 1e3)};
        unsigned long long tick {0};

        // TO ENTER/EXIT DEBUG MODE PRESS 'I' (QWERTY)
        // TO STEP IN DEBUG MODE PRESS 'O' (QWERTY)
        while (!quit) {
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                } else if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_I) {
                    debugging = true;
                } else if (interpreter.waiting) {
                    if (e.type == SDL_KEYDOWN) {
                        keyboard.onKeyDown(e.key.keysym.scancode);
                    } else if (e.type == SDL_KEYUP) {
                        keyboard.onKeyUp(e.key.keysym.scancode);
                    }
                }
            }

            for (int cycles {0}; cycles != cycles_per_frame and !quit; ++cycles) {
                interpreter.cycle();
                ++tick;

                if (debugging) {
                    std::cout << std::format("TICK [{:d}]:\n\tINSTRUCTION EXECUTED: {:0>4X}\n\tPC: {:0>4X} | IR: {:0>4X}:\n",
                                             tick, interpreter.cir, interpreter.pc, interpreter.i);
                    for (int r{0}; r != 16; ++r)
                        std::cout << std::format("\tREG V[{:X}]: {:0>2X}\n", r, interpreter.v[r]);

                    while (!quit and debugging) {
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

            SDL_Delay(frameLength);
            interpreter.endOfFrame();
        }
        display.off();
    }

    SDL_Quit();
    return 0;
}
