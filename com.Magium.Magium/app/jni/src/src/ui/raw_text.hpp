#pragma once

#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include "ui_element.hpp"

namespace MagiumSDL{
    extern SDL_Renderer *g_renderer;

    class RawText : public UIElement{
    private:
        std::string m_text;
        float m_scale;
        SDL_Color m_color;
        float m_wrappingLength;
        TTF_Font *m_font;
        
        SDL_Texture *m_texture;

    public:
        RawText(float x, float y, std::string text, float scale, SDL_Color color, TTF_Font *font, float wrappingLength = -1)
        : m_text{text}, m_scale{scale}, m_color{color}, m_font{font}, m_wrappingLength{wrappingLength}
        {
            if (wrappingLength == -1)
                m_wrappingLength = 64 * text.size();
            changeRectPosition(x, y);
            recreateTexture();
        }

        ~RawText()
        {
            SDL_DestroyTexture(m_texture);
        }

        void changeText(std::string newText){
            m_text = newText;
            recreateTexture();
        }

        void recreateTexture()
        {
            SDL_DestroyTexture(m_texture);
            SDL_Surface *textSurface = TTF_RenderUTF8_Blended_Wrapped(m_font, m_text.c_str(), m_color, m_wrappingLength / m_scale);
            m_texture = SDL_CreateTextureFromSurface(g_renderer, textSurface);
            SDL_DestroySurface(textSurface);
            SDL_GetTextureSize(m_texture, &m_rect.w, &m_rect.h);
            m_rect.w *= m_scale;
            m_rect.h *= m_scale;
        }

        void render() override
        {
            SDL_RenderTexture(g_renderer, m_texture, NULL, &m_rect);
        }
    };
}