#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <vector>
#include <functional>

#include "raw_text.hpp"

namespace MagiumSDL{
    extern SDL_Renderer *g_renderer;
    extern TTF_Font *g_fontButtons;
    extern float g_width;
    extern float g_height;

    class Button : public UIElement{
    private:
        SDL_Color m_color;
        RawText m_rawText;
        std::function<void(void)> m_function;

    public:
        Button(SDL_FRect rect, SDL_Color color, SDL_Color textColor, std::string text, float textScale, std::function<void(void)> function)
        : m_color{color}, m_rawText {rect.x, rect.y, text, textScale, textColor, g_fontButtons, rect.w}, m_function{function}
        {
            m_rect = rect;
            changeRectPosition(m_rect.x, m_rect.y); // to change text position
        }

        RawText &rawText() { return m_rawText; }

        void changeRectPosition(float x, float y) override
        {
            UIElement::changeRectPosition(x, y);
            float textX = x + m_rect.w / 2.0f - m_rawText.rect().w / 2.0f;
            float textY = y + m_rect.h / 2.0f - m_rawText.rect().h / 2.0f;
            m_rawText.changeRectPosition(textX, textY);
        }

        void render() override
        {
            SDL_SetRenderDrawColor(g_renderer, m_color.r, m_color.g, m_color.b, m_color.a);
            SDL_RenderFillRect(g_renderer, &m_rect);
            m_rawText.render();
        }

        void events(SDL_Event& event) override
        {
            if (event.type == SDL_EVENT_FINGER_DOWN){
                SDL_FPoint pos {event.tfinger.x * g_width, event.tfinger.y * g_height};
                if (SDL_PointInRectFloat(&pos, &m_rect))
                    m_function();
            }
        }
    };
}