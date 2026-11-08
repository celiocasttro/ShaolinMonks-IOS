#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <cstdio>

int main(int, char**) {
    // Prefer SDL3's Metal renderer on iOS. SDL can still select another
    // available backend if the platform build does not expose Metal.
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_AUDIO)) {
        std::fprintf(stderr, "SDL_Init failed: %s\\n", SDL_GetError());
        return 1;
    }

    // iOS presents the app full-screen; non-zero logical dimensions are
    // required by SDL_CreateWindow even when the fullscreen flag is set.
    SDL_Window* window = SDL_CreateWindow(
        "Shaolin Monks",
        1280,
        720,
        SDL_WINDOW_FULLSCREEN | SDL_WINDOW_HIGH_PIXEL_DENSITY
    );
    if (!window) {
        std::fprintf(stderr, "SDL_CreateWindow failed: %s\\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::fprintf(stderr, "SDL_CreateRenderer failed: %s\\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } else if (event.type == SDL_EVENT_KEY_DOWN &&
                       event.key.key == SDLK_ESCAPE) {
                running = false;
            }
        }

        // Temporary host diagnostic. The game's Vulkan renderer and Android
        // host code are not linked into this iOS shell yet.
        SDL_SetRenderDrawColor(renderer, 8, 8, 8, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 220, 20, 35, 255);
        SDL_FRect marker{24.0f, 24.0f, 72.0f, 8.0f};
        SDL_RenderFillRect(renderer, &marker);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
