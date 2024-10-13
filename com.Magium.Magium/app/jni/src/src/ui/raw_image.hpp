#pragma once

#include "ui_element.hpp"

extern SDL_Renderer *g_renderer;

namespace MagiumSDL {

class RawImage : public UIElement {
  private:
    SDL_Color m_color;

  public:
    RawImage(SDL_FRect rect, SDL_Color color) : m_color{color} { m_rect = rect; }

    void render() override {
        SDL_SetRenderDrawColor(g_renderer, m_color.r, m_color.g, m_color.b, m_color.a);
        SDL_RenderFillRect(g_renderer, &m_rect);
    }
};
} // namespace MagiumSDL
