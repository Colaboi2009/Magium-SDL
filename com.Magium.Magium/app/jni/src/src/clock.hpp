#include <SDL3/SDL.h>

namespace MagiumSDL{
    class Clock{
    private:
        Uint64 m_currentTime;
        Uint64 m_lastTime;
        float m_delta;
    public:
        const float dt() { return m_delta; }
        
        void tick()
        {
            m_lastTime = m_currentTime;
            m_currentTime = SDL_GetPerformanceCounter();
            m_delta = (float)((m_currentTime - m_lastTime) * 1000 / (float)SDL_GetPerformanceFrequency());
        }
    };
}