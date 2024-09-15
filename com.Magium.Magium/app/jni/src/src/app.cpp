#include "app.hpp"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_system.h>

#include <string>
#include <chrono>

#include "ui/ui_scene.hpp"
#include "ui/raw_image.hpp"
#include "ui/button.hpp"
#include "ui/scroller.hpp"
#include "parser/magium_decoder.hpp"

const char *placeHolderText = "I can't believe that we managed to get ourselves imprisoned twice in two days, Hadrik says. And by animals and ogres, to boot.";

namespace MagiumSDL
{
#pragma region // GLOBALS
    SDL_Window *g_window;
    SDL_Renderer *g_renderer;

    TTF_Font *g_fontMainText;
    TTF_Font *g_fontButtons;

    float g_width;
    float g_height;
#pragma endregion

#pragma region // STORY TEXT SETTINGS
    float STORY_TEXT_SIZE = .7f;
    float STORY_BUTTON_WIDTH = .875f;
    float STORY_BUTTON_HEIGHT = .055f;
    float STORY_BUTTON_SPACING = .08f;
    float STORY_BUTTON_BOTTOM_PADDING = .1f;
#pragma endregion

#pragma region // COLORS
    SDL_Color COLOR_GAME_TEXT_TOP_BACKGROUND         {180, 180, 180, 255};
    SDL_Color COLOR_GAME_TEXT_BOTTOM_BACKGROUND      {0,   0,   0,   255};
    SDL_Color COLOR_GAME_TEXT_MENU_BUTTON            {180, 180, 180, 255};
    SDL_Color COLOR_GAME_TEXT_MENU_BUTTON_TEXT       {0,   0,   0,   255};
    SDL_Color COLOR_GAME_TEXT_STORY_TEXT             {255, 255, 255, 255};
    SDL_Color COLOR_GAME_TEXT_OPTION_BUTTON          {80,  80,  80,  255};

    SDL_Color COLOR_MAIN_MENU_BACKGROUND             {255, 255, 255, 255};
    SDL_Color COLOR_MAIN_MENU_BUTTON                 {20,  20,  20,  255};
    SDL_Color COLOR_MAIN_MENU_BUTTON_TEXT            {255, 255, 255, 255};

    SDL_Color COLOR_STATS_MENU_BACKGROUND            {255, 255, 255, 255};
    SDL_Color COLOR_STATS_MENU_BUTTON                {20,  20,  20,  255};
    SDL_Color COLOR_STATS_MENU_BUTTON_TEXT           {255, 255, 255, 255};
    SDL_Color COLOR_STATS_MENU_STATS_BUTTON          {255, 255, 255, 255};
    SDL_Color COLOR_STATS_MENU_STATS_TEXT            {0,   0,   0,   255};
    SDL_Color COLOR_STATS_MENU_AVAILABLE_POINTS_TEXT {150, 20,  20,  255};
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

        std::shared_ptr<UIScene> gameTextScene = std::make_shared<UIScene>();
        game.add(gameTextScene);
        std::shared_ptr<UIScene> mainMenuScene = std::make_shared<UIScene>();
        game.add(mainMenuScene);
        std::shared_ptr<UIScene> statsMenuScene = std::make_shared<UIScene>();
        game.add(statsMenuScene);
#pragma endregion
 
#pragma region // Game Text Scene  SETUP
        const float sidePadding = .01;
        const float topPadding = .11;
        const float bottomPadding = .04f;

        const float scrollerBottomPadding = .1f;
        const float scrollerHeight = 1.f;

        std::shared_ptr<UIScene> scrollContent = std::make_shared<UIScene>();
        std::shared_ptr<RawText> contentText = std::make_shared<RawText>(g_width * sidePadding, g_height * topPadding, placeHolderText, .82f, COLOR_GAME_TEXT_STORY_TEXT, g_fontMainText, g_width * (1.f - sidePadding * 2.f));
        scrollContent->add(contentText);

        SDL_FRect scrollerRect = {0, 0, contentText->rect().w, g_height * (scrollerHeight - bottomPadding - scrollerBottomPadding)};
        std::shared_ptr<Scroller> scroller = std::make_shared<Scroller>(scrollerRect, scrollContent);
        gameTextScene->add(scroller);

        std::shared_ptr<RawImage> topBar = std::make_shared<RawImage>(SDL_FRect{0, g_height * .05f, g_width, g_height * .05f}, COLOR_GAME_TEXT_TOP_BACKGROUND);
        gameTextScene->add(topBar);

        std::shared_ptr<Button> mainMenuButton = std::make_shared<Button>(SDL_FRect{0, g_height * .05f, g_width * .3f, g_height * .05f}, COLOR_GAME_TEXT_MENU_BUTTON, COLOR_GAME_TEXT_MENU_BUTTON_TEXT, "MAIN MENU", .8f, [gameTextScene, mainMenuScene](){
            mainMenuScene->enable();
            gameTextScene->disable();
        });
        gameTextScene->add(mainMenuButton);

        std::shared_ptr<Button> statsButton = std::make_shared<Button>(SDL_FRect{g_width * .8f, g_height * .05f, g_width * .2f, g_height * .05f}, COLOR_GAME_TEXT_MENU_BUTTON, COLOR_GAME_TEXT_MENU_BUTTON_TEXT, "STATS", .8f, [gameTextScene, statsMenuScene](){
            statsMenuScene->enable();
            gameTextScene->disable();
        });
        gameTextScene->add(statsButton);

        std::shared_ptr<RawText> chapterText = std::make_shared<RawText>(g_width * 0.3f, g_height * .0618f, "Chapter 1", .8f, COLOR_GAME_TEXT_MENU_BUTTON_TEXT, g_fontButtons);
        gameTextScene->add(chapterText);

        std::shared_ptr<RawImage> bottomBar = std::make_shared<RawImage>(SDL_FRect{0, g_height * (1.f - bottomPadding), g_width, g_height * bottomPadding}, COLOR_GAME_TEXT_BOTTOM_BACKGROUND);
        gameTextScene->add(bottomBar);

        std::shared_ptr<RawImage> topHider = std::make_shared<RawImage>(SDL_FRect{0, -100, g_width, topBar->rect().h + 100}, COLOR_GAME_TEXT_BOTTOM_BACKGROUND);
        gameTextScene->add(topHider);
#pragma endregion

#pragma region // Main Menu Scene  SETUP
        std::shared_ptr<RawImage> mainMenuBackground = std::make_shared<RawImage>(SDL_FRect{0, 0, g_width, g_height}, COLOR_MAIN_MENU_BACKGROUND);
        mainMenuScene->add(mainMenuBackground);

        const float mainMenuButtonsWidth = .8f;
        const float mainMenuButtonsHeight = .05f;
        const float mainMenuButtonsDistance = 0.06f;
        const float mainMenuButtonsPadding = 0.05f;
        SDL_FRect mainMenuButtonsPos = {g_width / 2.f - g_width * mainMenuButtonsWidth / 2.f, g_height * (mainMenuButtonsDistance + mainMenuButtonsPadding), g_width * mainMenuButtonsWidth, g_height * mainMenuButtonsHeight};

        mainMenuButtonsPos.y += mainMenuButtonsDistance * g_height;
        std::shared_ptr<Button> mainMenuReturnButton = std::make_shared<Button>(mainMenuButtonsPos, COLOR_MAIN_MENU_BUTTON, COLOR_MAIN_MENU_BUTTON_TEXT, "Return To Game", .8f, [mainMenuScene, gameTextScene](){
            gameTextScene->enable();
            mainMenuScene->disable();
        });
        mainMenuScene->add(mainMenuReturnButton);
        mainMenuButtonsPos.y += mainMenuButtonsDistance * g_height;
        std::shared_ptr<Button> newGameButton = std::make_shared<Button>(mainMenuButtonsPos, COLOR_MAIN_MENU_BUTTON, COLOR_MAIN_MENU_BUTTON_TEXT, "New game / Restart Book", .8f, [](){
            
        });
        mainMenuScene->add(newGameButton);
        mainMenuButtonsPos.y += mainMenuButtonsDistance * g_height;
        std::shared_ptr<Button> loadFromLastCheckpointButton = std::make_shared<Button>(mainMenuButtonsPos, COLOR_MAIN_MENU_BUTTON, COLOR_MAIN_MENU_BUTTON_TEXT, "Load from last checkpoint", .8f, [](){
            
        });
        mainMenuScene->add(loadFromLastCheckpointButton);
        mainMenuButtonsPos.y += mainMenuButtonsDistance * g_height;
        std::shared_ptr<Button> saveLoadGameButton = std::make_shared<Button>(mainMenuButtonsPos, COLOR_MAIN_MENU_BUTTON, COLOR_MAIN_MENU_BUTTON_TEXT, "Save/Load game", .8f, [](){
            
        });
        mainMenuScene->add(saveLoadGameButton);
#pragma endregion
        
#pragma region // Stats Menu Scene SETUP
        std::shared_ptr<RawImage> statsMenuBackground = std::make_shared<RawImage>(SDL_FRect{0, 0, g_width, g_height}, COLOR_STATS_MENU_BACKGROUND);
        statsMenuScene->add(statsMenuBackground);

        const float statsMenuButtonY = .94f;
        const float statsMenuButtonWidth = .4f;
        const float statsMenuButtonHeight = .05f;
        SDL_FRect statsMenuButtonPos = {0, g_height * statsMenuButtonY, g_width * statsMenuButtonWidth, g_height * statsMenuButtonHeight};
        statsMenuButtonPos.x = g_width * 0.05f;
        std::shared_ptr<Button> statsMenuCancelButton = std::make_shared<Button>(statsMenuButtonPos, COLOR_STATS_MENU_BUTTON, COLOR_STATS_MENU_BUTTON_TEXT, "Cancel changes", .8f, [](){

        });
        statsMenuScene->add(statsMenuCancelButton);
        statsMenuButtonPos.x = g_width - statsMenuButtonPos.x - g_width * statsMenuButtonWidth;
        std::shared_ptr<Button> statsMenuReturnButton = std::make_shared<Button>(statsMenuButtonPos, COLOR_STATS_MENU_BUTTON, COLOR_STATS_MENU_BUTTON_TEXT, "Return to game", .8f, [statsMenuScene, gameTextScene](){
            gameTextScene->enable();
            statsMenuScene->disable();
        });
        statsMenuScene->add(statsMenuReturnButton);

        // TODO: positions
        const SDL_FRect availablePointsRect  = {};
        const SDL_FRect strengthRect         = {};
        const SDL_FRect speedRect            = {};
        const SDL_FRect toughnessRect        = {};
        const SDL_FRect reflexesRect         = {};
        const SDL_FRect hearingRect          = {};
        const SDL_FRect observationRect      = {};
        const SDL_FRect ancientLanguagesRect = {};
        const SDL_FRect combatTechniqueRect  = {};
        const SDL_FRect premonitionRect      = {};
        const SDL_FRect bluffRect            = {};
        const SDL_FRect magicalSenseRect     = {};
        const SDL_FRect auraHardeningRect    = {};
        const SDL_FRect magicalPowerRect     = {};
        const SDL_FRect magicalKnowledgeRect = {};
        std::shared_ptr<RawText> availablePoints       = std::make_shared<RawText>(availablePointsRect.x, availablePointsRect.y, "Available points: ", .82f, COLOR_STATS_MENU_AVAILABLE_POINTS_TEXT, g_fontMainText);
        std::shared_ptr<Button> strengthButton         = std::make_shared<Button>(strengthRect, COLOR_STATS_MENU_BUTTON, COLOR_STATS_MENU_BUTTON_TEXT, "Strength: ", .82f, [](){});
        std::shared_ptr<Button> speedButton            = std::make_shared<Button>(speedRect, COLOR_STATS_MENU_BUTTON, COLOR_STATS_MENU_BUTTON_TEXT, "Speed: ", .82f, [](){});
        std::shared_ptr<Button> toughnessButton        = std::make_shared<Button>(toughnessRect, COLOR_STATS_MENU_BUTTON, COLOR_STATS_MENU_BUTTON_TEXT, "Toughness: ", .82f, [](){});
        std::shared_ptr<Button> reflexesButton         = std::make_shared<Button>(reflexesRect, COLOR_STATS_MENU_BUTTON, COLOR_STATS_MENU_BUTTON_TEXT, "Reflexes: ", .82f, [](){});
        std::shared_ptr<Button> hearingButton          = std::make_shared<Button>(hearingRect, COLOR_STATS_MENU_BUTTON, COLOR_STATS_MENU_BUTTON_TEXT, "Hearing: ", .82f, [](){});
        std::shared_ptr<Button> observationButton      = std::make_shared<Button>(observationRect, COLOR_STATS_MENU_BUTTON, COLOR_STATS_MENU_BUTTON_TEXT, "Observation: ", .82f, [](){});
        std::shared_ptr<Button> ancientLanguagesButton = std::make_shared<Button>(ancientLanguagesRect, COLOR_STATS_MENU_BUTTON, COLOR_STATS_MENU_BUTTON_TEXT, "Ancient languages: ", .82f, [](){});
        std::shared_ptr<Button> combatTechniqueButton  = std::make_shared<Button>(combatTechniqueRect, COLOR_STATS_MENU_BUTTON, COLOR_STATS_MENU_BUTTON_TEXT, "Combat technique: ", .82f, [](){});
        std::shared_ptr<Button> premonitionButton      = std::make_shared<Button>(premonitionRect, COLOR_STATS_MENU_BUTTON, COLOR_STATS_MENU_BUTTON_TEXT, "Premonition: ", .82f, [](){});
        std::shared_ptr<Button> bluffButton            = std::make_shared<Button>(bluffRect, COLOR_STATS_MENU_BUTTON, COLOR_STATS_MENU_BUTTON_TEXT, "Bluff: ", .82f, [](){});
        std::shared_ptr<Button> magicalSenseButton     = std::make_shared<Button>(magicalSenseRect, COLOR_STATS_MENU_BUTTON, COLOR_STATS_MENU_BUTTON_TEXT, "Magical sense: ", .82f, [](){});
        std::shared_ptr<Button> auraHardeningButton    = std::make_shared<Button>(auraHardeningRect, COLOR_STATS_MENU_BUTTON, COLOR_STATS_MENU_BUTTON_TEXT, "Aura hardening: ", .82f, [](){});
        std::shared_ptr<Button> magicalPowerButton     = std::make_shared<Button>(magicalPowerRect, COLOR_STATS_MENU_BUTTON, COLOR_STATS_MENU_BUTTON_TEXT, "Magical power: ", .82f, [](){});
        std::shared_ptr<Button> magicalKnowledgeButton = std::make_shared<Button>(magicalKnowledgeRect, COLOR_STATS_MENU_BUTTON, COLOR_STATS_MENU_BUTTON_TEXT, "Magical knowledge: ", .82f, [](){});

#pragma endregion

#pragma region // MAIN LOOP
        gameTextScene->enable();
        mainMenuScene->disable();
        statsMenuScene->disable();

        Parser::MagiumDecoder decoder(scrollContent, contentText, chapterText);
        decoder.setStatVariables(strengthButton, speedButton, toughnessButton, reflexesButton, 
                                hearingButton, observationButton, ancientLanguagesButton, combatTechniqueButton, 
                                premonitionButton, bluffButton, magicalKnowledgeButton, auraHardeningButton, 
                                magicalPowerButton, magicalKnowledgeButton);

        decoder.processAndStoreFile("ch1.magium");

        while (true)
        {
            SDL_Event event;
            if (SDL_PollEvent(&event))
            {
                if (event.type == SDL_EVENT_QUIT)
                    break;
            }
            SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255);
            SDL_RenderClear(g_renderer);

            game.events(event);
            game.render();
            decoder.update();

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