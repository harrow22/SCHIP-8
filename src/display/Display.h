#ifndef CHIP_8_DISPLAY_H
#define CHIP_8_DISPLAY_H

#include <bitset>
#include <vector>
#include <cstdint>
#include "SDL2/SDL.h"
#define SCALE_FACTOR 10
#define HI_RESOLUTION_SCALE 2
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define HEX_COLOR_ON 0x9C5ECCFF
#define HEX_COLOR_OFF 0x18141CFF

class Display {
public:
    bool on();
    void off();
    void clear();
    void draw();
    bool flipPixel(std::uint8_t, std::uint8_t);
    void setres(int);
    void scrollDown(std::uint8_t);
    void scrollRight();
    void scrollLeft();
    [[nodiscard]] bool isHighRes() const { return scale == HI_RESOLUTION_SCALE; }
    const std::vector<std::uint8_t>& buffer {buffer_};
    const int& width {width_};
    const int& height {height_};
private:
    SDL_Window* window {nullptr};
    SDL_Renderer* renderer {nullptr};
    SDL_Texture* texture {nullptr};
    std::vector<std::uint8_t> buffer_ {std::vector<std::uint8_t>(SCREEN_WIDTH * SCREEN_HEIGHT)};
    int scale {1};
    int width_ {SCREEN_WIDTH};
    int height_ {SCREEN_HEIGHT};
};


#endif //CHIP_8_DISPLAY_H
