#ifndef CHIP_8_DISPLAY_H
#define CHIP_8_DISPLAY_H

#include <vector>
#include <cstdint>
#include "SDL.h"

class Display {
public:
    bool on();
    void off();
    void clear();
    void draw();
    bool flipPixel(std::uint8_t, std::uint8_t);
    void setResolution(int);
    void scrollDown(std::uint8_t);
    void scrollRight();
    void scrollLeft();

    const int& width {width_};
    const int& height {height_};
private:
    static constexpr int screenWidth_ {64};
    static constexpr int screenHeight_ {32};
    static constexpr long long colorOn_ {0x9C5ECCFF};
    static constexpr long long colorOff_ {0x18141CFF};
    static constexpr int scaleFactor_ {10};

    SDL_Window* window_ {nullptr};
    SDL_Renderer* renderer_ {nullptr};
    SDL_Texture* texture_ {nullptr};
    std::vector<std::uint8_t> buffer_ {std::vector<std::uint8_t>(screenWidth_ * screenHeight_)};
    int currScale_ {1};
    int width_ {screenWidth_};
    int height_ {screenHeight_};
};


#endif //CHIP_8_DISPLAY_H
