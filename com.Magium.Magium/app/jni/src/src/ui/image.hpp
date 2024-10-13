#pragma once

#include <SDL3_image/SDL_image.h>

#include "ui_element.hpp"

namespace MagiumSDL {
    class Image : public UIElement {
    private:
        std::string m_filepath;
        SDL_Texture *m_texture;
		float m_scale;

    public:
        Image(std::string filepath, SDL_FRect r, float scale) : m_filepath{filepath}, m_scale{scale} {
            m_rect = r;
            createImage();
        }

        void createImage() {
            SDL_DestroyTexture(m_texture);
            SDL_Surface *surface = IMG_Load(m_filepath.c_str());
            m_texture = SDL_CreateTextureFromSurface(g_renderer, surface);
            SDL_DestroySurface(surface);
            SDL_GetTextureSize(m_texture, &m_rect.w, &m_rect.h);
        }

        void render() override{
			SDL_FRect rr = { m_rect.x, m_rect.y, m_rect.w * m_scale, m_rect.h * m_scale};
            SDL_RenderTexture(g_renderer, m_texture, NULL, &rr);
        }
    };
}
