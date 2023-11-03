#ifndef CHIP_8_KEYBOARD_H
#define CHIP_8_KEYBOARD_H

#include "SDL.h"
#include <cstdint>
#include <array>
#include <unordered_map>

class Keyboard {
public:
    static constexpr int nullKey {255};

    static bool isPressed(std::uint8_t) ;
    std::uint8_t wasPressed();
    void onKeyDown(SDL_Scancode&);
    void onKeyUp(SDL_Scancode&);
    void reset();
private:
    static constexpr std::array<SDL_Scancode, 16> keyMap {
        SDL_SCANCODE_X, // 0
        SDL_SCANCODE_1, // 1
        SDL_SCANCODE_2, // 2
        SDL_SCANCODE_3, // 3
        SDL_SCANCODE_Q, // 4
        SDL_SCANCODE_W, // 5
        SDL_SCANCODE_E, // 6
        SDL_SCANCODE_A, // 7
        SDL_SCANCODE_S, // 8
        SDL_SCANCODE_D, // 9
        SDL_SCANCODE_Z, // 10
        SDL_SCANCODE_C, // 11
        SDL_SCANCODE_4, // 12
        SDL_SCANCODE_R, // 13
        SDL_SCANCODE_F, // 14
        SDL_SCANCODE_V, // 15
    };
    std::unordered_map<SDL_Scancode, std::uint8_t> keysDown;
    std::uint8_t released {nullKey};
};


#endif //CHIP_8_KEYBOARD_H
