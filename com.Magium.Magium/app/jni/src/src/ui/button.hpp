#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <functional>

#include "../helper/helper.hpp"
#include "raw_image.hpp"
#include "raw_text.hpp"

extern SDL_Renderer *g_renderer;
extern TTF_Font *g_fontButtons;
extern float g_width;
extern float g_height;

namespace MagiumSDL {
class Button : public UIElement {
  private:
    SDL_Color m_color;
    std::shared_ptr<RawText> m_rawText;
    std::function<void(void)> m_function;

    SDL_FRect m_renderedPosition;

  public:
    Button(SDL_FRect rect, SDL_Color color, SDL_Color textColor, std::string text, float textScale, std::function<void(void)> function)
        : m_color{color}, m_function{function} {
        m_rect = rect;
        m_rawText = make_shared<RawText>(rect.x, rect.y, text, textScale, textColor, g_fontButtons, rect.w);
        if (rect.w == 0 || rect.h == 0) {
            fitToText();
        }
    }

    std::shared_ptr<RawText> rawText() { return m_rawText; }

    void setFunction(std::function<void(void)> func) { m_function = func; }

    void fitToText() {
        m_rect.w = m_rawText->rect().w;
        m_rect.h = m_rawText->rect().h;
    }

    void render() override {
        SDL_SetRenderDrawColor(g_renderer, m_color.r, m_color.g, m_color.b, m_color.a);
        SDL_RenderFillRect(g_renderer, &m_rect);
        m_renderedPosition = m_rect;

        SDL_FRect tr = {m_rect.x + m_rect.w / 2.f, m_rect.y + m_rect.h / 2.f, m_rawText->rect().w, m_rawText->rect().h};
        m_rawText->rect() = Helper::centerRect(tr);
        m_rawText->render();
    }

    void events(SDL_Event &event) override {
        if (event.type == SDL_EVENT_FINGER_DOWN) {
            SDL_FPoint pos{event.tfinger.x * g_width, event.tfinger.y * g_height};
            if (SDL_PointInRectFloat(&pos, &m_renderedPosition)) {
                m_function();
            }
        }
    }
};
} // namespace MagiumSDL
