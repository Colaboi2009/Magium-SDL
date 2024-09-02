#include "app.hpp"

#include <SDL3/SDL.h>

#include "button.hpp"

namespace MagiumSDL{

    SDL_Window *g_window;
    SDL_Renderer *g_renderer;
    TTF_Font *g_font;
    int g_width;
    int g_height;
    
    SDL_Color BUTTON_COLOR {45, 45, 45, 255};

    App::App(){
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        g_window = SDL_CreateWindow("Magium", SDL_WINDOW_FULLSCREEN, SDL_WINDOW_FULLSCREEN, 0);
        g_renderer = SDL_CreateRenderer(g_window, 0);

        g_font = TTF_OpenFont("LiberationSans.ttf", 8);
        if (g_font == nullptr){
            BUTTON_COLOR = {100, 0, 100, 255};
        }

        SDL_GetWindowSize(g_window, &g_width, &g_height);

        std::vector<Button> buttons;
        buttons.emplace_back(Button(500, 500, 0.2f * g_width, 0.2f * g_width, "Main Menu"));
        buttons.emplace_back(Button(300, 200, 0.2f * g_width, 0.2f * g_width, "Back"));

        while(true){
            SDL_Event event;
            if(SDL_PollEvent(&event)){
                if (event.type == SDL_EVENT_QUIT)
                    break;
            }

            SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255);
            SDL_RenderClear(g_renderer);

            SDL_SetRenderDrawColor(g_renderer, BUTTON_COLOR.r, BUTTON_COLOR.g, BUTTON_COLOR.b, BUTTON_COLOR.a);
            Button::RenderButtons(buttons);

            SDL_RenderPresent(g_renderer);
        }

        TTF_CloseFont(g_font);
        TTF_Quit();

        SDL_DestroyRenderer(g_renderer);
        SDL_DestroyWindow(g_window);
        SDL_Quit();
    }
}