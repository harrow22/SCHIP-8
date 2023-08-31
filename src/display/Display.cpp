#include <iostream>
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

void Display::draw() {
    std::uint32_t* pixel {nullptr};
    int pitch {};

    SDL_LockTexture(texture, nullptr, (void**) &pixel, &pitch);
    for (int i {0}; i != SCREEN_WIDTH * SCREEN_HEIGHT; ++i, ++pixel)
        *pixel = pixels[i] ?  HEX_COLOR_ON : HEX_COLOR_OFF;
    SDL_UnlockTexture(texture);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}


bool Display::setPixel(std::uint8_t x, std::uint8_t y, std::uint8_t bit)
{
    auto temp {pixels_[(x + y * SCREEN_WIDTH)]};
    pixels_[(x + y * SCREEN_WIDTH)] ^= bit;
    return temp == 128 && bit == 128;
}

void Display::clear() {
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    for (auto& pixel : pixels_)
        pixel = 0;
}
