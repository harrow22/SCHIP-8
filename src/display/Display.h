#ifndef CHIP_8_DISPLAY_H
#define CHIP_8_DISPLAY_H

#include <bitset>
#include <vector>
#include <array>
#include <cstdint>
#include "SDL2/SDL.h"
#define SCALE_FACTOR 10
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define HEX_COLOR_ON 0x9C5ECCFF
#define HEX_COLOR_OFF 0x18141CFF

using PixelGrid = std::array<std::uint8_t, SCREEN_WIDTH * SCREEN_HEIGHT>;
class Display {
public:
    Display() : pixels{pixels_} {};
    bool on();
    void off();
    void draw();
    bool setPixel(std::uint8_t, std::uint8_t, std::uint8_t);
    void clear();
    const PixelGrid& pixels;
private:
    SDL_Window* window {nullptr};
    SDL_Renderer* renderer {nullptr};
    SDL_Texture* texture {nullptr};
    PixelGrid pixels_ {};
};


#endif //CHIP_8_DISPLAY_H
