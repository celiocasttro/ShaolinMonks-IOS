#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "game_data.h"
#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

namespace {
enum class Page { Home, Cheats, Settings, About };
Page page = Page::Home;
bool running = true;
int width = 1280, height = 720;
struct Button { SDL_FRect rect; const char* label; int action; };
void text(SDL_Renderer* r, float x, float y, const char* value) {
    SDL_SetRenderDrawColor(r, 238, 230, 215, 255);
    SDL_RenderDebugText(r, x, y, value);
}
void box(SDL_Renderer* r, SDL_FRect rect, SDL_Color color) {
    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(r, &rect);
}
void button(SDL_Renderer* r, const Button& b, bool active=false) {
    box(r, b.rect, active ? SDL_Color{125,25,24,255} : SDL_Color{43,22,20,255});
    SDL_SetRenderDrawColor(r, 190,150,76,255);
    SDL_RenderRect(r, &b.rect);
    text(r, b.rect.x+18, b.rect.y+b.rect.h/2-4, b.label);
}
void header(SDL_Renderer* r, const char* subtitle) {
    box(r, {0,0,(float)width,(float)height}, {8,7,8,255});
    box(r, {0,0,(float)width,12}, {155,25,28,255});
    text(r, 48, 48, "MORTAL KOMBAT");
    SDL_SetRenderDrawColor(r, 205,164,82,255);
    SDL_RenderLine(r, 48, 70, (float)width-48, 70);
    text(r, 48, 91, subtitle);
}
void render(SDL_Renderer* r, const std::vector<ios_game_data::CandidateInfo>& games) {
    if (page == Page::Home) {
        header(r, "SHAOLIN MONKS  |  iOS PORT");
        text(r, 48, 142, "Bem-vindo. Selecione os arquivos do jogo para preparar a biblioteca.");
        Button b[] = {{{48,190,330,62},"SELECIONAR ISO / XBE",1},
                      {{48,270,330,62},"CHEATS  >",2},
                      {{48,350,330,62},"CONFIGURACOES  >",3},
                      {{48,430,330,62},"SOBRE  >",4}};
        for (const auto& v:b) button(r,v);
        text(r, 430, 205, "JOGO");
        if (games.empty()) text(r,430,242,"Nenhum arquivo de jogo importado.");
        else {
            float y=242;
            for (const auto& g:games) {
                text(r,430,y,g.path.c_str());
                y+=24;
            }
        }
        text(r,48,(float)height-42,"IMPORTANTE: esta versao e uma interface inicial; o motor do jogo ainda nao esta integrado.");
    } else if (page == Page::Cheats) {
        header(r, "EXTRAS  /  CHEATS");
        text(r,48,142,"Importe arquivos de codigos pelo app Arquivos.");
        Button back{{48,190,250,58},"VOLTAR",10};
        Button import{{320,190,300,58},"IMPORTAR ARQUIVO",11};
        button(r,back); button(r,import,true);
        const auto cheats=ios_game_data::ListCheatFiles();
        text(r,48,285,"ARQUIVOS IMPORTADOS");
        if (cheats.empty()) text(r,48,325,"Nenhum arquivo .cht, .pnach ou .txt encontrado.");
        else { float y=325; for (const auto& c:cheats) { text(r,64,y,c.c_str()); y+=25; } }
        text(r,48,(float)height-70,"A leitura/importacao de arquivos e a interface estao em preparacao.");
        text(r,48,(float)height-48,"Os codigos ainda NAO sao aplicados ao jogo: falta integrar o runtime e validar formatos.");
    } else {
        header(r, page == Page::Settings ? "CONFIGURACOES" : "SOBRE");
        Button back{{48,190,250,58},"VOLTAR",10}; button(r,back);
        if (page == Page::Settings) text(r,48,285,"Opcoes graficas, audio e controles serao habilitadas com o runtime.");
        else text(r,48,285,"Port iOS nao oficial. Requer os arquivos originais do jogo fornecidos pelo usuario.");
    }
}
int actionAt(float x,float y) {
    if (page==Page::Home) {
        if (x>=48&&x<=378&&y>=190&&y<=252) return 1;
        if (x>=48&&x<=378&&y>=270&&y<=332) return 2;
        if (x>=48&&x<=378&&y>=350&&y<=412) return 3;
        if (x>=48&&x<=378&&y>=430&&y<=492) return 4;
    } else if (page==Page::Cheats) {
        if (x>=48&&x<=298&&y>=190&&y<=248) return 10;
        if (x>=320&&x<=620&&y>=190&&y<=248) return 11;
    } else if (x>=48&&x<=298&&y>=190&&y<=248) return 10;
    return 0;
}
void handleAction(int a) {
    switch(a) {
        case 1: ios_game_data::PickGameFile(); break;
        case 2: page=Page::Cheats; break;
        case 3: page=Page::Settings; break;
        case 4: page=Page::About; break;
        case 10: page=Page::Home; break;
        case 11: ios_game_data::PickCheatFile(); break;
    }
}
}
int main(int, char**) {
    SDL_SetHint(SDL_HINT_RENDER_DRIVER,"metal");
    if (!SDL_Init(SDL_INIT_VIDEO|SDL_INIT_GAMEPAD|SDL_INIT_AUDIO)) return 1;
    std::string error;
    ios_game_data::PrepareFolders(error);
    SDL_Window* window=SDL_CreateWindow("Shaolin Monks",1280,720,SDL_WINDOW_FULLSCREEN|SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (!window) { SDL_Quit(); return 1; }
    SDL_Renderer* renderer=SDL_CreateRenderer(window,nullptr);
    if (!renderer) { SDL_DestroyWindow(window); SDL_Quit(); return 1; }
    SDL_GetWindowSize(window,&width,&height);
    while(running) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type==SDL_EVENT_QUIT) running=false;
            else if(event.type==SDL_EVENT_FINGER_DOWN) {
                int w=0,h=0; SDL_GetWindowSize(window,&w,&h);
                handleAction(actionAt(event.tfinger.x*w,event.tfinger.y*h));
            } else if(event.type==SDL_EVENT_MOUSE_BUTTON_DOWN) {
                handleAction(actionAt((float)event.button.x,(float)event.button.y));
            } else if(event.type==SDL_EVENT_KEY_DOWN && event.key.key==SDLK_ESCAPE) {
                if(page==Page::Home) running=false; else page=Page::Home;
            }
        }
        SDL_GetWindowSize(window,&width,&height);
        auto games=ios_game_data::InspectCandidates();
        SDL_SetRenderDrawColor(renderer,8,7,8,255); SDL_RenderClear(renderer);
        render(renderer,games);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); SDL_Quit(); return 0;
}
