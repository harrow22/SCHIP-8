#include "Keyboard.h"

bool Keyboard::isPressed(std::uint8_t key) {
    return SDL_GetKeyboardState(nullptr)[keyMap[key]];
}

void Keyboard::reset()
{
    released = NULL_KEY;
    keysDown.clear();
}

std::uint8_t Keyboard::wasPressed() {
    if (released != NULL_KEY) {
        auto temp {released};
        reset();
        return temp;
    }
    return NULL_KEY;
}

void Keyboard::onKeyDown(SDL_Scancode& scancode)
{
    if (released != NULL_KEY)
        return;

    for (int i {0}; i != keyMap.size(); ++i) {
        if (scancode == keyMap[i]) {
            keysDown[scancode] = i;
            return;
        }
    }
}

void Keyboard::onKeyUp(SDL_Scancode& scancode)
{
    auto key {keysDown.find(scancode)};
    if (key != keysDown.end())
        released = key->second;
}
