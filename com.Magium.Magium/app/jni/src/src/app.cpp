#include "app.hpp"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_system.h>
#include <string>

#include "ui/ui_scene.hpp"
#include "ui/raw_image.hpp"
#include "ui/button.hpp"
#include "ui/scroller.hpp"
#include "parser/magium_decoder.hpp"

const char *placeHolderText = "Lorem ipsum odor amet, consectetuer adipiscing elit. Adipiscing integer aliquam netus aenean condimentum pretium turpis suscipit. Justo netus ut rutrum adipiscing leo phasellus. Rutrum litora nascetur fringilla natoque aliquam imperdiet neque erat. Ad tempus lacus erat est nascetur parturient. Mollis malesuada erat penatibus nisi posuere. Odio natoque dui lacinia augue sem nec consequat nunc. Velit quis tincidunt consequat tempus rhoncus vestibulum efficitur et ultricies.\n\nNeque facilisis ipsum ridiculus cursus, accumsan interdum. Quis condimentum scelerisque tortor lacinia urna habitasse taciti dui hac. Orci cubilia enim sem at dignissim vivamus. Parturient pretium dictumst tortor non, pellentesque magnis donec aenean justo. Sollicitudin dictum vivamus commodo molestie diam phasellus elit ullamcorper quam. Et etiam donec condimentum ad a porta bibendum feugiat. Hendrerit arcu gravida duis tincidunt at maecenas.\n\nEfficitur rutrum magna iaculis velit erat purus pretium. Viverra congue viverra placerat ad urna egestas congue. Risus ligula mi auctor litora tristique etiam est sapien. Taciti elementum fames mollis per lorem libero? Sit suspendisse pharetra sociosqu dui dapibus efficitur fermentum nullam inceptos. Ultrices lacinia ipsum ex pharetra placerat. Cras mauris viverra cras neque aenean nam mollis feugiat. Bibendum platea hendrerit sem id in hendrerit metus diam? Sem neque pulvinar fames; quam etiam sit condimentum habitasse tortor.\n\nPorttitor litora eleifend egestas per; natoque scelerisque auctor cras. Viverra lectus class; scelerisque ullamcorper ultrices torquent diam? Sodales augue est quis vivamus sem cras mus elementum. Aliquet phasellus suscipit arcu primis litora senectus sit. Maecenas semper felis porttitor tristique mus neque per metus. Vitae leo facilisi suscipit hendrerit feugiat accumsan fusce.\n\nScelerisque dapibus maecenas euismod per mauris penatibus vivamus. Curae urna dis volutpat porta praesent venenatis nullam dictumst himenaeos. Pellentesque neque ultricies tincidunt mollis natoque tempor laoreet. Integer eros inceptos erat a commodo aliquam sem efficitur. Aptent donec eget sapien primis sem amet non pellentesque. Himenaeos nullam amet nam orci eros inceptos nulla. Venenatis pretium augue sagittis torquent, lacinia ut ullamcorper.\n\nLorem ipsum odor amet, consectetuer adipiscing elit. Adipiscing integer aliquam netus aenean condimentum pretium turpis suscipit. Justo netus ut rutrum adipiscing leo phasellus. Rutrum litora nascetur fringilla natoque aliquam imperdiet neque erat. Ad tempus lacus erat est nascetur parturient. Mollis malesuada erat penatibus nisi posuere. Odio natoque dui lacinia augue sem nec consequat nunc. Velit quis tincidunt consequat tempus rhoncus vestibulum efficitur et ultricies.\n\nNeque facilisis ipsum ridiculus cursus, accumsan interdum. Quis condimentum scelerisque tortor lacinia urna habitasse taciti dui hac. Orci cubilia enim sem at dignissim vivamus. Parturient pretium dictumst tortor non, pellentesque magnis donec aenean justo. Sollicitudin dictum vivamus commodo molestie diam phasellus elit ullamcorper quam. Et etiam donec condimentum ad a porta bibendum feugiat. Hendrerit arcu gravida duis tincidunt at maecenas.\n\nEfficitur rutrum magna iaculis velit erat purus pretium. Viverra congue viverra placerat ad urna egestas congue. Risus ligula mi auctor litora tristique etiam est sapien. Taciti elementum fames mollis per lorem libero? Sit suspendisse pharetra sociosqu dui dapibus efficitur fermentum nullam inceptos. Ultrices lacinia ipsum ex pharetra placerat. Cras mauris viverra cras neque aenean nam mollis feugiat. Bibendum platea hendrerit sem id in hendrerit metus diam? Sem neque pulvinar fames; quam etiam sit condimentum habitasse tortor.\n\nPorttitor litora eleifend egestas per; natoque scelerisque auctor cras. Viverra lectus class; scelerisque ullamcorper ultrices torquent diam? Sodales augue est quis vivamus sem cras mus elementum. Aliquet phasellus suscipit arcu primis litora senectus sit. Maecenas semper felis porttitor tristique mus neque per metus. Vitae leo facilisi suscipit hendrerit feugiat accumsan fusce.\n\nScelerisque dapibus maecenas euismod per mauris penatibus vivamus. Curae urna dis volutpat porta praesent venenatis nullam dictumst himenaeos. Pellentesque neque ultricies tincidunt mollis natoque tempor laoreet. Integer eros inceptos erat a commodo aliquam sem efficitur. Aptent donec eget sapien primis sem amet non pellentesque. Himenaeos nullam amet nam orci eros inceptos nulla. Venenatis pretium augue sagittis torquent, lacinia ut ullamcorper.\n\n";

namespace MagiumSDL
{
#pragma region // GLOBALS
    SDL_Window *g_window;
    SDL_Renderer *g_renderer;

    Clock g_clock;

    TTF_Font *g_fontMainText;
    TTF_Font *g_fontButtons;

    float g_width;
    float g_height;
#pragma endregion

#pragma region // COLORS
    const SDL_Color COLOR_GAME_TEXT_TOP_BACKGROUND         {180, 180, 180, 255};
    const SDL_Color COLOR_GAME_TEXT_BOTTOM_BACKGROUND      {0,   0,   0,   255};
    const SDL_Color COLOR_GAME_TEXT_MENU_BUTTON            {180, 180, 180, 255};
    const SDL_Color COLOR_GAME_TEXT_OPTION_BUTTON          {45,  45,  45,  255};
    const SDL_Color COLOR_GAME_TEXT_MENU_BUTTON_TEXT       {0,   0,   0,   255};
    const SDL_Color COLOR_GAME_TEXT_STORY_TEXT             {255, 255, 255, 255};

    const SDL_Color COLOR_MAIN_MENU_BACKGROUND             {255, 255, 255, 255};
    const SDL_Color COLOR_MAIN_MENU_BUTTON                 {20,  20,  20,  255};
    const SDL_Color COLOR_MAIN_MENU_BUTTON_TEXT            {255, 255, 255, 255};

    const SDL_Color COLOR_STATS_MENU_BACKGROUND            {255, 255, 255, 255};
    const SDL_Color COLOR_STATS_MENU_BUTTON                {20,  20,  20,  255};
    const SDL_Color COLOR_STATS_MENU_BUTTON_TEXT           {255, 255, 255, 255};
    const SDL_Color COLOR_STATS_MENU_STATS_BUTTON          {255, 255, 255, 255};
    const SDL_Color COLOR_STATS_MENU_STATS_TEXT            {0,   0,   0,   255};
    const SDL_Color COLOR_STATS_MENU_AVAILABLE_POINTS_TEXT {150, 20,  20,  255};
#pragma endregion

    App::App()
    {
#pragma region // INIT
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        g_window = SDL_CreateWindow("Magium", SDL_WINDOW_FULLSCREEN, SDL_WINDOW_FULLSCREEN, 0);
        g_renderer = SDL_CreateRenderer(g_window, 0);
        SDL_SetRenderVSync(g_renderer, SDL_RENDERER_VSYNC_DISABLED);

        TTF_Init();
        g_fontMainText = TTF_OpenFont("LiberationSans.ttf", 64);
        TTF_SetFontWrappedAlign(g_fontMainText, TTF_WRAPPED_ALIGN_LEFT);
        g_fontButtons = TTF_OpenFont("LiberationSans.ttf", 64);
        TTF_SetFontWrappedAlign(g_fontButtons, TTF_WRAPPED_ALIGN_CENTER);

        int iWidth, iHeight;

        SDL_GetWindowSize(g_window, &iWidth, &iHeight);
        g_width = static_cast<float>(iWidth);
        g_height = static_cast<float>(iHeight);

        UIScene game;
        game.enable();

        UIScene gameTextScene;
        game.add(&gameTextScene);
        UIScene mainMenuScene;
        game.add(&mainMenuScene);
        UIScene statsMenuScene;
        game.add(&statsMenuScene);

        Parser::MagiumDecoder decoder;
        decoder.processAndStoreFile("ch1.magium");
#pragma endregion
 
#pragma region // Game Text Scene  SETUP
        const float sidePadding = .01;
        const float topPadding = .1;
        const float bottomPadding = .04f;

        // const float scrollerHeight = g_width - (topPadding + bottomPadding);
        const float scrollerTopPadding = .01f;
        const float scrollerBottomPadding = .01f;
        const float scrollerHeight = 1.f;

        UIScene scrollContent;
        RawText contentText(g_width * sidePadding, g_height * topPadding, placeHolderText, .82f, COLOR_GAME_TEXT_STORY_TEXT, g_fontMainText, g_width * (1.f - sidePadding * 2.f));
        scrollContent.add(&contentText);
        Scroller scroller({contentText.rect().x, contentText.rect().y - scrollerTopPadding * g_height, contentText.rect().w, g_height * (scrollerHeight - bottomPadding - scrollerBottomPadding)}, scrollContent);
        gameTextScene.add(&scroller);


        RawImage topBar({0, g_height * .05f, g_width, g_height * .05f}, COLOR_GAME_TEXT_TOP_BACKGROUND);
        gameTextScene.add(&topBar);

        Button mainMenuButton({0, g_height * .05f, g_width * .3f, g_height * .05f}, COLOR_GAME_TEXT_MENU_BUTTON, COLOR_GAME_TEXT_MENU_BUTTON_TEXT, "MAIN MENU", .8f, [&gameTextScene, &mainMenuScene](){
            mainMenuScene.enable();
            gameTextScene.disable();
        });
        gameTextScene.add(&mainMenuButton);

        Button statsButton({g_width * .8f, g_height * .05f, g_width * .2f, g_height * .05f}, COLOR_GAME_TEXT_MENU_BUTTON, COLOR_GAME_TEXT_MENU_BUTTON_TEXT, "STATS", .8f, [&gameTextScene, &statsMenuScene](){
            statsMenuScene.enable();
            gameTextScene.disable();
        });
        gameTextScene.add(&statsButton);

        RawText chapterText(g_width * 0.3f, g_height * .0618f, "Chapter 1", .8f, COLOR_GAME_TEXT_MENU_BUTTON_TEXT, g_fontButtons);
        gameTextScene.add(&chapterText);

        RawImage bottomBar({0, g_height * (1.f - bottomPadding), g_width, g_height * bottomPadding}, COLOR_GAME_TEXT_BOTTOM_BACKGROUND);
        gameTextScene.add(&bottomBar);

        RawImage topHider({0, -100, g_width, topBar.rect().h + 100}, COLOR_GAME_TEXT_BOTTOM_BACKGROUND);
        gameTextScene.add(&topHider);
#pragma endregion

#pragma region // Main Menu Scene  SETUP
        RawImage mainMenuBackground({0, 0, g_width, g_height}, COLOR_MAIN_MENU_BACKGROUND);
        mainMenuScene.add(&mainMenuBackground);

        const float mainMenuButtonsWidth = .8f;
        const float mainMenuButtonsHeight = .05f;
        const float mainMenuButtonsDistance = 0.06f;
        const float mainMenuButtonsPadding = 0.05f;
        SDL_FRect mainMenuButtonsPos = {g_width / 2.f - g_width * mainMenuButtonsWidth / 2.f, g_height * (mainMenuButtonsDistance + mainMenuButtonsPadding), g_width * mainMenuButtonsWidth, g_height * mainMenuButtonsHeight};

        mainMenuButtonsPos.y += mainMenuButtonsDistance * g_height;
        Button mainMenuReturnButton(mainMenuButtonsPos, COLOR_MAIN_MENU_BUTTON, COLOR_MAIN_MENU_BUTTON_TEXT, "Return To Game", .8f, [&mainMenuScene, &gameTextScene](){
            gameTextScene.enable();
            mainMenuScene.disable();
        });
        mainMenuScene.add(&mainMenuReturnButton);
        mainMenuButtonsPos.y += mainMenuButtonsDistance * g_height;
        Button newGameButton(mainMenuButtonsPos, COLOR_MAIN_MENU_BUTTON, COLOR_MAIN_MENU_BUTTON_TEXT, "New game / Restart Book", .8f, [](){
            
        });
        mainMenuScene.add(&newGameButton);
        mainMenuButtonsPos.y += mainMenuButtonsDistance * g_height;
        Button loadFromLastCheckpointButton(mainMenuButtonsPos, COLOR_MAIN_MENU_BUTTON, COLOR_MAIN_MENU_BUTTON_TEXT, "Load from last checkpoint", .8f, [](){
            
        });
        mainMenuScene.add(&loadFromLastCheckpointButton);
        mainMenuButtonsPos.y += mainMenuButtonsDistance * g_height;
        Button saveLoadGameButton(mainMenuButtonsPos, COLOR_MAIN_MENU_BUTTON, COLOR_MAIN_MENU_BUTTON_TEXT, "Save/Load game", .8f, [](){
            
        });
        mainMenuScene.add(&saveLoadGameButton);
#pragma endregion
        
#pragma region // Stats Menu Scene SETUP
        RawImage statsMenuBackground({0, 0, g_width, g_height}, COLOR_STATS_MENU_BACKGROUND);
        statsMenuScene.add(&statsMenuBackground);

        const float statsMenuButtonY = .94f;
        const float statsMenuButtonWidth = .4f;
        const float statsMenuButtonHeight = .05f;
        SDL_FRect statsMenuButtonPos = {0, g_height * statsMenuButtonY, g_width * statsMenuButtonWidth, g_height * statsMenuButtonHeight};
        statsMenuButtonPos.x = g_width * 0.05f;
        Button statsMenuCancelButton(statsMenuButtonPos, COLOR_STATS_MENU_BUTTON, COLOR_STATS_MENU_BUTTON_TEXT, "Cancel changes", .8f, [](){

        });
        statsMenuScene.add(&statsMenuCancelButton);
        statsMenuButtonPos.x = g_width - statsMenuButtonPos.x - g_width * statsMenuButtonWidth;
        Button statsMenuReturnButton(statsMenuButtonPos, COLOR_STATS_MENU_BUTTON, COLOR_STATS_MENU_BUTTON_TEXT, "Return to game", .8f, [&statsMenuScene, &gameTextScene](){
            gameTextScene.enable();
            statsMenuScene.disable();
        });
        statsMenuScene.add(&statsMenuReturnButton);
#pragma endregion

#pragma region // MAIN LOOP
        gameTextScene.enable();
        mainMenuScene.disable();
        statsMenuScene.disable();

        while (true)
        {
            g_clock.tick();
            
            SDL_Event event;
            if (SDL_PollEvent(&event))
            {
                if (event.type == SDL_EVENT_QUIT)
                    break;
            }
            SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255);
            SDL_RenderClear(g_renderer);

            g_clock.tick();
            game.events(event);
            game.render();

            SDL_RenderPresent(g_renderer);
        }
#pragma endregion

#pragma region // CLEANUP
        TTF_CloseFont(g_fontMainText);
        TTF_CloseFont(g_fontButtons);
        TTF_Quit();

        SDL_DestroyRenderer(g_renderer);
        SDL_DestroyWindow(g_window);
        SDL_Quit();
#pragma endregion
    }
}