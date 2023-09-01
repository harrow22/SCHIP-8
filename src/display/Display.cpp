#include <iostream>
#include <algorithm>
#include "Display.h"

bool onError()
{
    std::cerr << SDL_GetError() << std::endl;
    return false;
}

bool Display::on() {
    window = SDL_CreateWindow(
            "CHIP-8 Emulator",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            SCREEN_WIDTH * SCALE_FACTOR,
            SCREEN_HEIGHT * SCALE_FACTOR,
            SDL_WINDOW_SHOWN);
    if (!window)
        return onError();

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
        return onError();

    texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_RGBA8888,
                                SDL_TEXTUREACCESS_STREAMING,
                                SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!texture)
        return onError();


    SDL_SetRenderDrawColor(renderer, 0x18, 0x14, 0x1C, 0xFF);
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    // initialize screen
    clear();
    return true;
}

void Display::off() {
    SDL_DestroyTexture(texture);
    texture = nullptr;
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    SDL_DestroyWindow(window);
    window = nullptr;
}

void Display::setres(int newScale)
{
    clear();
    if (scale == newScale)
        return;
    SDL_DestroyTexture(texture);

    scale = newScale;
    width_ = SCREEN_WIDTH * scale;
    height_ = SCREEN_HEIGHT * scale;
    texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_RGBA8888,
                                SDL_TEXTUREACCESS_STREAMING,
                                width_, height_);
    SDL_RenderSetLogicalSize(renderer, width_, height_);
    if (!texture)
        onError();
    buffer_.resize(width_ * height_);
}

void Display::draw() {
    std::uint32_t* pixels {nullptr};
    int pitch {};

    SDL_LockTexture(texture, nullptr, (void**) &pixels, &pitch);
    for (auto pixel : buffer)
        *pixels++ = pixel ? HEX_COLOR_ON : HEX_COLOR_OFF;
    SDL_UnlockTexture(texture);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void Display::scrollDown(std::uint8_t n)
{
    std::shift_right(buffer_.begin(), buffer_.end(), n * width_);
    std::fill(buffer_.begin(), buffer_.begin() + (n * width_), 0);
}

void Display::scrollRight()
{
    for (int i {0}; i != width_ * height_; i += width_) {
        auto row {buffer_.begin() + i};
        std::shift_right(row, row + width_, 4);
        std::fill(row, row + 4, 0);
    }
}

void Display::scrollLeft()
{
    for (int i {0}; i != width_ * height_; i += width_) {
        auto row {buffer_.begin() + i};
        std::shift_left(row, row + width_, 4);
        std::fill(row + width_ - 4, row + width_, 0);
    }
}

bool Display::flipPixel(std::uint8_t x, std::uint8_t y)
{
    std::uint8_t temp {buffer_[(x + y * width_) % (width_ * height_)]};
    buffer_[(x + y * width_) % (width_ * height_)] ^= 1;
    return temp;
}

void Display::clear() {
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    for (auto& pixel : buffer_)
        pixel = 0;
}
