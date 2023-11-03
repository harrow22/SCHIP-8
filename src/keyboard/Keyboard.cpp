#include "Keyboard.h"

bool Keyboard::isPressed(std::uint8_t key) {
    return SDL_GetKeyboardState(nullptr)[keyMap[key]];
}

void Keyboard::reset()
{
    released = nullKey;
    keysDown.clear();
}

std::uint8_t Keyboard::wasPressed() {
    if (released != nullKey) {
        std::uint8_t temp {released};
        reset();
        return temp;
    }
    return nullKey;
}

void Keyboard::onKeyDown(SDL_Scancode& scancode)
{
    if (released != nullKey)
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
