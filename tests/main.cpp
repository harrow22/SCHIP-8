#include <gtest/gtest.h>
#include <SDL2/SDL.h>

int main(int argc, char **argv)
{
    if (SDL_Init(SDL_INIT_VIDEO < 0)) {
        std::cout << "[ERROR] SDL failed to initialize" << std::endl;
        std::terminate();
    }

    ::testing::InitGoogleTest(&argc, argv);
    auto res {RUN_ALL_TESTS()};

    SDL_Quit();
    return res;
}