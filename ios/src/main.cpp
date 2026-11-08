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
std::string FormatSize(uint64_t bytes) {
    char value[64];
    if (bytes >= 1024ULL * 1024ULL * 1024ULL)
        std::snprintf(value, sizeof(value), "%.2f GiB", static_cast<double>(bytes) / (1024.0 * 1024.0 * 1024.0));
    else if (bytes >= 1024ULL * 1024ULL)
        std::snprintf(value, sizeof(value), "%.1f MiB", static_cast<double>(bytes) / (1024.0 * 1024.0));
    else
        std::snprintf(value, sizeof(value), "%llu bytes", static_cast<unsigned long long>(bytes));
    return value;
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
    const std::vector<ios_game_data::CandidateInfo> candidates = ios_game_data::InspectCandidates();

    SDL_Window* window = SDL_CreateWindow("Shaolin Monks", 1280, 720,
        SDL_WINDOW_FULLSCREEN | SDL_WINDOW_HIGH_PIXEL_DENSITY);
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
    for (const auto& candidate : candidates) {
        SDL_Log("Game candidate: %s (%s, %llu bytes)%s%s", candidate.path.c_str(), candidate.type.c_str(),
            static_cast<unsigned long long>(candidate.size), candidate.sha1.empty() ? "" : ", SHA-1 ",
            candidate.sha1.c_str());
    }

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT ||
                (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)) running = false;
        }
        SDL_SetRenderDrawColor(renderer, 8, 8, 8, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 220, 20, 35, 255);
        SDL_FRect accent{42.0f, 52.0f, 96.0f, 8.0f};
        SDL_RenderFillRect(renderer, &accent);
        SDL_SetRenderDrawColor(renderer, 245, 245, 245, 255);
        DrawLine(renderer, 42.0f, 88.0f, "MORTAL KOMBAT: SHAOLIN MONKS");
        SDL_SetRenderDrawColor(renderer, 190, 190, 190, 255);
        DrawLine(renderer, 42.0f, 124.0f, "iOS PORT - GAME DATA INSPECTION");
        DrawLine(renderer, 42.0f, 184.0f, "Files > On My iPhone > Shaolin Monks");
        DrawLine(renderer, 42.0f, 216.0f, "Put default.xbe or .iso in this folder or its assets subfolder.");
        DrawLine(renderer, 42.0f, 272.0f, foldersReady ? "Storage folder: READY" : "Storage folder: ERROR");
        DrawLine(renderer, 42.0f, 304.0f, gameFolder.c_str());
        if (!candidates.empty()) {
            SDL_SetRenderDrawColor(renderer, 120, 220, 140, 255);
            DrawLine(renderer, 42.0f, 360.0f, "Detected game files:");
            float y = 392.0f;
            for (const auto& candidate : candidates) {
                if (y > 620.0f) break;
                const std::string size = FormatSize(candidate.size);
                SDL_SetRenderDrawColor(renderer, 225, 225, 225, 255);
                DrawLine(renderer, 42.0f, y, candidate.path.c_str());
                y += 24.0f;
                std::string detail = candidate.type + " | " + size;
                if (!candidate.sha1.empty()) detail += " | SHA-1: " + candidate.sha1;
                SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
                DrawLine(renderer, 62.0f, y, detail.c_str());
                y += 32.0f;
            }
        } else {
            SDL_SetRenderDrawColor(renderer, 235, 190, 100, 255);
            DrawLine(renderer, 42.0f, 376.0f, "No game files detected yet.");
        }
        SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255);
        DrawLine(renderer, 42.0f, 680.0f, "SHA-1 inspection is active; XBE release matching and ISO mounting are next.");
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
