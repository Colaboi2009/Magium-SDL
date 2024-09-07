#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace MagiumSDL{
    extern SDL_Renderer *g_renderer;
    extern TTF_Font *g_fontButtons;

    class UIElement{
    protected:
        SDL_FRect m_rect;    
        bool m_enabled = true;
        bool m_destroyed = false;
        
    public:
        SDL_FRect rect() { return m_rect; }
        virtual void changeRectPosition(float x, float y) { m_rect.x = x; m_rect.y = y; }

        virtual void enable() { m_enabled = true; }
        virtual void disable() { m_enabled = false; }
        bool enabled() { return m_enabled; }

        virtual void destroy() { m_destroyed = true; }
        bool destroyed() { return m_destroyed; }

        virtual void events(SDL_Event& event) {}
        virtual void render() {}
    };
}