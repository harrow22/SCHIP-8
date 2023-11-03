#include <gtest/gtest.h>
#include "SDL.h"

int main(int argc, char **argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL_Init() failed. SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    ::testing::InitGoogleTest(&argc, argv);
    auto res {RUN_ALL_TESTS()};

    SDL_Quit();
    return res;
}