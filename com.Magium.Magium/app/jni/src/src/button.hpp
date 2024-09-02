#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <vector>
#include <string>


namespace MagiumSDL{
    extern SDL_Renderer *g_renderer;
    extern TTF_Font *g_font;

    class Button{
    private:
        SDL_FRect m_rect;
        std::string m_text;
        
    public:
        Button(float x, float y, float w, float h, std::string text);
    
        SDL_FRect& rect() { return m_rect; };
        void checkPressed(SDL_FPoint &pos);
        void renderText();

    public:
        static void RenderButtons(std::vector<Button> buttons);
    };
}