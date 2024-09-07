#include "raw_text.hpp"
#include "../clock.hpp"

namespace MagiumSDL{
    extern float g_height;
    extern Clock g_clock;

    class Scroller : public UIElement{
    private:
        UIScene &m_content;

        float m_previousDelta;

        float m_scrollPosition;
        float m_scrollVelocity;

        bool m_scrolling;

        float m_maxScrollPosition;

        const float c_scrollModifier             = .000005f;
        const float c_minimumScrollSpeedModifier = .0001f;
        const float c_maxScrollSpeedModifier     = .0035f;

        float m_scrollDeceleration;
        float m_minimumScrollSpeed;
        float m_maxScrollSpeed;

    public:
        Scroller(SDL_FRect rect, UIScene &content) : m_content{content}
        {
            m_rect = rect;
            m_scrollPosition = 0;
            m_maxScrollPosition = rect.y - rect.h;
            for (int i = 0; i < content.getAll().size(); i++){
                UIElement &u = content.get(i);
                if (i == 0){
                    m_maxScrollPosition += u.rect().h;
                    continue;
                }
                m_maxScrollPosition += u.rect().h + (u.rect().y - content.get(i - 1).rect().y);
            }

            m_scrollDeceleration = g_height * c_scrollModifier;
            m_minimumScrollSpeed = g_height * c_minimumScrollSpeedModifier;
            m_maxScrollSpeed = g_height * c_maxScrollSpeedModifier;
        }

        void events(SDL_Event& event) override
        {
            if (event.type == SDL_EVENT_FINGER_DOWN){
                m_scrollVelocity = 0;
                m_scrolling = true;
            }
            else if (event.type == SDL_EVENT_FINGER_MOTION){
                m_scrollPosition += event.tfinger.dy * g_height;
                m_scrollVelocity = event.tfinger.dy * g_height;
            } 
            else if (event.type == SDL_EVENT_FINGER_UP){
                m_scrolling = false;
            }
            else if (!m_scrolling){
                m_scrollPosition += m_scrollVelocity;
            }

            float signBefore = m_scrollVelocity > 0 ? 1 : -1;
            if (std::abs(m_scrollVelocity) > m_maxScrollSpeed){
                m_scrollVelocity = signBefore * m_maxScrollSpeed;
            }
            else if (std::abs(m_scrollVelocity) < m_minimumScrollSpeed){
                m_scrollVelocity = 0;
            } 
            m_scrollVelocity -= signBefore * m_scrollDeceleration;
            float signAfter = m_scrollVelocity > 0 ? 1 : -1;
            if (signAfter != signBefore){
                m_scrollVelocity = 0;
            }

            m_scrollPosition = std::clamp(m_scrollPosition, -m_maxScrollPosition, 0.f);
            m_content.changeRectPosition(m_content.rect().x, m_scrollPosition);
        }

        void render() override
        {
            m_content.render();
        }
    };
}