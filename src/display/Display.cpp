#include "Display.h"
#include <algorithm>
#include <iostream>
#include <format>

bool onError()
{
    std::cerr << std::format("SDL_Error: {:s}\n", SDL_GetError());
    return false;
}

bool Display::on() {
    window_ = SDL_CreateWindow(
            "SCHIP-8",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            screenWidth_ * scaleFactor_,
            screenHeight_ * scaleFactor_,
            SDL_WINDOW_SHOWN);
    if (!window_)
        return onError();

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer_)
        return onError();

    texture_ = SDL_CreateTexture(renderer_,
                                 SDL_PIXELFORMAT_RGBA8888,
                                 SDL_TEXTUREACCESS_STREAMING,
                                 screenWidth_, screenHeight_);
    if (!texture_)
        return onError();

    SDL_SetRenderDrawColor(renderer_, 0x18, 0x14, 0x1C, 0xFF);
    SDL_RenderSetLogicalSize(renderer_, screenWidth_, screenHeight_);

    // initialize screen
    clear();
    return true;
}

void Display::off() {
    SDL_DestroyTexture(texture_);
    texture_ = nullptr;
    SDL_DestroyRenderer(renderer_);
    renderer_ = nullptr;
    SDL_DestroyWindow(window_);
    window_ = nullptr;
}

void Display::setResolution(int newScale)
{
    clear();
    if (currScale_ == newScale)
        return;
    SDL_DestroyTexture(texture_);

    currScale_ = newScale;
    width_ = screenWidth_ * currScale_;
    height_ = screenHeight_ * currScale_;
    texture_ = SDL_CreateTexture(renderer_,
                                 SDL_PIXELFORMAT_RGBA8888,
                                 SDL_TEXTUREACCESS_STREAMING,
                                 width_, height_);
    SDL_RenderSetLogicalSize(renderer_, width_, height_);
    if (!texture_)
        onError();
    buffer_.resize(width_ * height_);
}

void Display::draw() {
    std::uint32_t* pixels {nullptr};
    int pitch {};

    SDL_LockTexture(texture_, nullptr, (void**) &pixels, &pitch);
    for (auto pixel : buffer_)
        *pixels++ = pixel ? colorOn_ : colorOff_;
    SDL_UnlockTexture(texture_);

    SDL_RenderClear(renderer_);
    SDL_RenderCopy(renderer_, texture_, nullptr, nullptr);
    SDL_RenderPresent(renderer_);
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
    SDL_RenderClear(renderer_);
    SDL_RenderPresent(renderer_);
    for (auto& pixel : buffer_)
        pixel = 0;
}
