#include <SDL.h>

#include <iostream>

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("SDL Keycode Display", SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Event event;
    bool quit = false;

    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                SDL_KeyboardEvent keyEvent = event.key;
                SDL_Scancode scancode      = keyEvent.keysym.scancode;
                SDL_Keycode keycode        = keyEvent.keysym.sym;

                std::cout << "Event: " << (event.type == SDL_KEYDOWN ? "Key Down" : "Key Up")
                          << std::endl;
                std::cout << "  Scancode: " << SDL_GetScancodeName(scancode) << " (" << scancode
                          << ")" << std::endl;
                std::cout << "  Keycode: " << SDL_GetKeyName(keycode) << " (" << keycode << ")"
                          << std::endl;
                std::cout << "  Modifier State: " << keyEvent.keysym.mod << std::endl;
                std::cout << std::endl;
            }
        }
        // You might want to add a small delay here to avoid busy-looping
        SDL_Delay(10);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
