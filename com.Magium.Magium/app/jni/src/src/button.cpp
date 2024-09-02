#include "button.hpp"

namespace MagiumSDL{
    Button::Button(float x, float y, float w, float h, std::string text) : m_text{text}
    {
        m_rect = {x, y, w, h};
    }

    void Button::checkPressed(SDL_FPoint &pos)
    {
        if (SDL_PointInRectFloat(&pos, &m_rect)){
        }   
    }

    void Button::renderText()
    {
        SDL_Surface *textSurface = TTF_RenderUTF8_Solid(g_font, m_text.c_str(), {255, 255, 255, 255});
        SDL_Texture *texture = SDL_CreateTextureFromSurface(g_renderer, textSurface);
        SDL_RenderTexture(g_renderer, texture, NULL, NULL);
    }

    void Button::RenderButtons(std::vector<Button> buttons)
    {
        std::vector<SDL_FRect> rects(buttons.size());
        for (Button b : buttons){
            rects.push_back(b.rect());
        }
        SDL_RenderFillRects(g_renderer, rects.data(), rects.size());
        for (Button b : buttons){
            b.renderText();
        }
    }
}
