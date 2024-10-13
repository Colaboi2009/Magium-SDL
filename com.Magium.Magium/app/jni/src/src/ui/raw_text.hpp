#pragma once

#include "ui_element.hpp"
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

extern SDL_Renderer *g_renderer;

namespace MagiumSDL {
class RawText : public UIElement {
  private:
    std::string m_text;
    float m_scale;
    SDL_Color m_color;
    float m_wrappingLength;
    TTF_Font *m_font;

    SDL_Texture *m_texture;

  public:
    RawText(float x, float y, std::string text, float scale, SDL_Color color, TTF_Font *font, float wrappingLength = -1)
        : m_text{text}, m_scale{scale}, m_color{color}, m_font{font}, m_wrappingLength{wrappingLength} {
        if (wrappingLength == -1)
            m_wrappingLength = 64 * text.size();
        m_rect = {x, y};
        m_texture = nullptr;
        recreateTexture();
    }

    ~RawText() {
        if (m_texture) {
            SDL_DestroyTexture(m_texture);
            m_texture = nullptr;
        }
    }

	void setColor(SDL_Color c) { m_color = c; }
    void setFont(TTF_Font *font) { m_font = font; }
	std::string getText() const { return m_text; }

    void changeText(std::string newText) {
        m_text = newText;
        recreateTexture();
    }

    void recreateTexture() {
        if (m_texture) {
            SDL_DestroyTexture(m_texture);
            m_texture = nullptr;
        }

        SDL_Surface *surface =
            TTF_RenderUTF8_Blended_Wrapped(m_font, m_text.c_str(), m_color, /*{35, 35, 35, 255},*/ m_wrappingLength / m_scale);
        m_texture = SDL_CreateTextureFromSurface(g_renderer, surface);
        SDL_DestroySurface(surface);

        SDL_GetTextureSize(m_texture, &m_rect.w, &m_rect.h);
        m_rect.w *= m_scale;
        m_rect.h *= m_scale;
    }

    void render() override { SDL_RenderTexture(g_renderer, m_texture, NULL, &m_rect); }
};
} // namespace MagiumSDL
