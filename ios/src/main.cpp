#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "game_data.h"

#include <cstdio>
#include <string>
#include <vector>

namespace {
void DrawLine(SDL_Renderer* renderer, float x, float y, const char* text) {
    SDL_RenderDebugText(renderer, x, y, text);
}
}

int main(int, char**) {
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_AUDIO)) {
        std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    std::string folderError;
    const bool foldersReady = ios_game_data::PrepareFolders(folderError);
    const std::string gameFolder = ios_game_data::GameDirectory();
    const std::vector<std::string> candidates = ios_game_data::FindCandidates();

    SDL_Window* window = SDL_CreateWindow(
        "Shaolin Monks",
        1280,
        720,
        SDL_WINDOW_FULLSCREEN | SDL_WINDOW_HIGH_PIXEL_DENSITY
    );
    if (!window) {
        std::fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Log("Game data folder: %s", gameFolder.c_str());
    if (!foldersReady) SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", folderError.c_str());
    for (const auto& candidate : candidates) SDL_Log("Game candidate: %s", candidate.c_str());

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT ||
                (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)) {
                running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 8, 8, 8, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 220, 20, 35, 255);
        SDL_FRect accent{42.0f, 52.0f, 96.0f, 8.0f};
        SDL_RenderFillRect(renderer, &accent);

        SDL_SetRenderDrawColor(renderer, 245, 245, 245, 255);
        DrawLine(renderer, 42.0f, 88.0f, "MORTAL KOMBAT: SHAOLIN MONKS");
        SDL_SetRenderDrawColor(renderer, 190, 190, 190, 255);
        DrawLine(renderer, 42.0f, 124.0f, "iOS PORT - GAME DATA SETUP");
        DrawLine(renderer, 42.0f, 184.0f, "Open the Files app, then On My iPhone > Shaolin Monks.");
        DrawLine(renderer, 42.0f, 216.0f, "Place your legally owned game files in the ShaolinMonks folder or its assets folder.");
        DrawLine(renderer, 42.0f, 248.0f, "Supported discovery formats: default.xbe and .iso");
        DrawLine(renderer, 42.0f, 304.0f, foldersReady ? "Storage folder: READY" : "Storage folder: ERROR");
        DrawLine(renderer, 42.0f, 336.0f, gameFolder.c_str());

        if (!candidates.empty()) {
            SDL_SetRenderDrawColor(renderer, 120, 220, 140, 255);
            DrawLine(renderer, 42.0f, 400.0f, "Game files detected:");
            SDL_SetRenderDrawColor(renderer, 190, 190, 190, 255);
            float y = 432.0f;
            for (const auto& candidate : candidates) {
                if (y > 650.0f) break;
                DrawLine(renderer, 42.0f, y, candidate.c_str());
                y += 28.0f;
            }
        } else {
            SDL_SetRenderDrawColor(renderer, 235, 190, 100, 255);
            DrawLine(renderer, 42.0f, 400.0f, "No game files detected yet.");
        }

        SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255);
        DrawLine(renderer, 42.0f, 680.0f, "This is the storage setup screen; game runtime and renderer integration are still in progress.");

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
