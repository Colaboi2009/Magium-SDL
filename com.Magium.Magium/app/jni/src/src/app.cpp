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
#include "helper/helper.hpp"

namespace MagiumSDL
{
#pragma region // GLOBALS
    SDL_Window *g_window;
    SDL_Renderer *g_renderer;

    TTF_Font *g_fontMainText;
    TTF_Font *g_fontButtons;
    TTF_Font *g_fontBold;

    float g_width;
    float g_height;

    std::vector<Helper::DelayedFunction> g_delayedFunctions;

    UIScene g_game = {};

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
    SDL_Color COLOR_STATS_MENU_AVAILABLE_POINTS_TEXT {189, 89,  25,  255};
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
        g_fontBold = TTF_OpenFont("LiberationSans-Bold.ttf", 64);
        TTF_SetFontWrappedAlign(g_fontBold, TTF_WRAPPED_ALIGN_CENTER);

        int iWidth, iHeight;

        SDL_GetWindowSize(g_window, &iWidth, &iHeight);
        g_width = static_cast<float>(iWidth);
        g_height = static_cast<float>(iHeight);

        g_game.enable();

        std::shared_ptr<UIScene> gameTextScene = std::make_shared<UIScene>();
        g_game.add(gameTextScene);
        std::shared_ptr<UIScene> mainMenuScene = std::make_shared<UIScene>();
        g_game.add(mainMenuScene);
        std::shared_ptr<UIScene> statsMenuScene = std::make_shared<UIScene>();
        g_game.add(statsMenuScene);
#pragma endregion
 
#pragma region // Game Text Scene  SETUP
        const float sidePadding = .01;
        const float topPadding = .11;
        const float bottomPadding = .04f;

        const float scrollerBottomPadding = .1f;
        const float scrollerHeight = 1.f;

        std::shared_ptr<UIScene> scrollContent = std::make_shared<UIScene>();
        std::shared_ptr<RawText> contentText = std::make_shared<RawText>(g_width * sidePadding, g_height * topPadding, "placeholder", .82f, COLOR_GAME_TEXT_STORY_TEXT, g_fontMainText, g_width * (1.f - sidePadding * 2.f));
        scrollContent->add(contentText);

        SDL_FRect scrollerRect = {0, 0, contentText->rect().w, g_height * (scrollerHeight - bottomPadding - scrollerBottomPadding)};
        std::shared_ptr<Scroller> scroller = std::make_shared<Scroller>(scrollerRect, scrollContent);
        gameTextScene->add(scroller);

        std::shared_ptr<RawImage> topBar = std::make_shared<RawImage>(Helper::swiths(0, .05f, 1, .05f), COLOR_GAME_TEXT_TOP_BACKGROUND);
        gameTextScene->add(topBar);

        std::shared_ptr<Button> mainMenuButton = std::make_shared<Button>(Helper::swiths(0, .05f, .3f, .05f), COLOR_GAME_TEXT_MENU_BUTTON, COLOR_GAME_TEXT_MENU_BUTTON_TEXT, "MAIN MENU", .8f, [gameTextScene, mainMenuScene](){
            mainMenuScene->enable();
            gameTextScene->disable();
        });
        gameTextScene->add(mainMenuButton);

        std::shared_ptr<Button> statsButton = std::make_shared<Button>(Helper::swiths(.8f, .05f, .2f, .05f), COLOR_GAME_TEXT_MENU_BUTTON, COLOR_GAME_TEXT_MENU_BUTTON_TEXT, "STATS", .8f, [gameTextScene, statsMenuScene](){
            statsMenuScene->enable();
            gameTextScene->disable();
        });
        gameTextScene->add(statsButton);

        std::shared_ptr<RawText> chapterText = std::make_shared<RawText>(g_width * 0.3f, g_height * .0618f, "Chapter 1", .8f, COLOR_GAME_TEXT_MENU_BUTTON_TEXT, g_fontButtons);
        gameTextScene->add(chapterText);

        std::shared_ptr<RawImage> bottomBar = std::make_shared<RawImage>(Helper::swiths(0, 1.f - bottomPadding, 1.f, bottomPadding), COLOR_GAME_TEXT_BOTTOM_BACKGROUND);
        gameTextScene->add(bottomBar);

        std::shared_ptr<RawImage> topHider = std::make_shared<RawImage>(SDL_FRect{0, -100.f, g_width, topBar->rect().h + 100.f}, COLOR_GAME_TEXT_BOTTOM_BACKGROUND);
        gameTextScene->add(topHider);
#pragma endregion

        Parser::MagiumDecoder decoder(scrollContent, contentText, chapterText);

#pragma region // Main Menu Scene  SETUP
        std::shared_ptr<RawImage> mainMenuBackground = std::make_shared<RawImage>(SDL_FRect{0, 0, g_width, g_height}, COLOR_MAIN_MENU_BACKGROUND);
        mainMenuScene->add(mainMenuBackground);

        const float mainMenuButtonsWidth = .8f;
        const float mainMenuButtonsHeight = .05f;
        const float mainMenuButtonsDistance = 0.06f;
        const float mainMenuButtonsPadding = 0.05f;
        SDL_FRect mainMenuButtonsPos = {g_width / 2.f, g_height * (mainMenuButtonsDistance + mainMenuButtonsPadding), g_width * mainMenuButtonsWidth, g_height * mainMenuButtonsHeight};
        mainMenuButtonsPos = Helper::centerRectX(mainMenuButtonsPos);

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

        const float statsMenuButtonY = .89f;
        const float statsMenuButtonWidth = .4f;
        const float statsMenuButtonHeight = .05f;
        SDL_FRect statsMenuButtonPos = Helper::swiths(0, statsMenuButtonY, statsMenuButtonWidth, statsMenuButtonHeight);
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

        std::shared_ptr<RawImage> statsTopbar = std::make_shared<RawImage>(Helper::swiths(0, 0, 1.f, .06f), SDL_Color{0, 0, 0, 255});
        statsMenuScene->add(statsTopbar);
        std::shared_ptr<RawText> statsText = std::make_shared<RawText>(.5f * g_width, .07f * g_height, "STATS", .9f, SDL_Color{0, 0, 0, 255}, g_fontBold);
        statsText->rect() = Helper::centerRectX(statsText->rect());
        statsMenuScene->add(statsText);
        std::shared_ptr<RawImage> statsBottombar = std::make_shared<RawImage>(Helper::swiths(0, .955f, 1.f, .5f), SDL_Color{0, 0, 0, 255});
        statsMenuScene->add(statsBottombar);

        std::shared_ptr<RawText> availablePointsText = std::make_shared<RawText>(.5f * g_width, .15f * g_height, "Available points: 26", .82f, COLOR_STATS_MENU_AVAILABLE_POINTS_TEXT, g_fontMainText);
        availablePointsText->rect() = Helper::centerRectX(availablePointsText->rect());
        statsMenuScene->add(availablePointsText);

        // TODO: positions
        const SDL_FRect strengthRect         = {.425f , .25f};
        const SDL_FRect speedRect            = {.9f   , .25f};
        const SDL_FRect toughnessRect        = {.425f , .35f};
        const SDL_FRect reflexesRect         = {.9f   , .35f};
        const SDL_FRect hearingRect          = {.425f , .45f};
        const SDL_FRect observationRect      = {.9f   , .45f};
        const SDL_FRect ancientLanguagesRect = {.75f  , .55f};
        const SDL_FRect combatTechniqueRect  = {.75f  , .62f};
        const SDL_FRect premonitionRect      = {.75f  , .69f};
        const SDL_FRect bluffRect            = {.75f  , .76f};
        const SDL_FRect magicalSenseRect     = {.445f , .83f};
        const SDL_FRect auraHardeningRect    = {.95f  , .83f};

        const float statsTextSize = .75f;

        std::vector<std::shared_ptr<Button>> statButtons = {
            std::make_shared<Button>(Helper::swiths(strengthRect), COLOR_STATS_MENU_STATS_BUTTON, COLOR_STATS_MENU_STATS_TEXT, "Strength: 0 / 4", statsTextSize, [](){}),
            std::make_shared<Button>(Helper::swiths(toughnessRect), COLOR_STATS_MENU_STATS_BUTTON, COLOR_STATS_MENU_STATS_TEXT, "Toughness: 0 / 4", statsTextSize, [](){}),
            std::make_shared<Button>(Helper::swiths(speedRect), COLOR_STATS_MENU_STATS_BUTTON, COLOR_STATS_MENU_STATS_TEXT, "Speed: 0 / 4", statsTextSize, [](){}),
            std::make_shared<Button>(Helper::swiths(reflexesRect), COLOR_STATS_MENU_STATS_BUTTON, COLOR_STATS_MENU_STATS_TEXT, "Reflexes: 0 / 4", statsTextSize, [](){}),
            std::make_shared<Button>(Helper::swiths(hearingRect), COLOR_STATS_MENU_STATS_BUTTON, COLOR_STATS_MENU_STATS_TEXT, "Hearing: 0 / 4", statsTextSize, [](){}),
            std::make_shared<Button>(Helper::swiths(observationRect), COLOR_STATS_MENU_STATS_BUTTON, COLOR_STATS_MENU_STATS_TEXT, "Observation: 0 / 4", statsTextSize, [](){}),
            std::make_shared<Button>(Helper::swiths(ancientLanguagesRect), COLOR_STATS_MENU_STATS_BUTTON, COLOR_STATS_MENU_STATS_TEXT, "Ancient languages:  0 / 4", statsTextSize, [](){}),
            std::make_shared<Button>(Helper::swiths(combatTechniqueRect), COLOR_STATS_MENU_STATS_BUTTON, COLOR_STATS_MENU_STATS_TEXT, "Combat technique:  0 / 4", statsTextSize, [](){}),
            std::make_shared<Button>(Helper::swiths(premonitionRect), COLOR_STATS_MENU_STATS_BUTTON, COLOR_STATS_MENU_STATS_TEXT, "Premonition:  0 / 4", statsTextSize, [](){}),
            std::make_shared<Button>(Helper::swiths(bluffRect), COLOR_STATS_MENU_STATS_BUTTON, COLOR_STATS_MENU_STATS_TEXT, "Bluff(Aura concealment):  0 / 4", statsTextSize, [](){}),
            std::make_shared<Button>(Helper::swiths(magicalSenseRect), COLOR_STATS_MENU_STATS_BUTTON, COLOR_STATS_MENU_STATS_TEXT, "Magical sense: 0 / 4", statsTextSize, [](){}),
            std::make_shared<Button>(Helper::swiths(auraHardeningRect), COLOR_STATS_MENU_STATS_BUTTON, COLOR_STATS_MENU_STATS_TEXT, "Aura hardening: 0 / 4", statsTextSize, [](){}),
        };
        // TODO: magical power and knowledge are not buttons
        // const SDL_FRect magicalPowerRect     = {};
        // const SDL_FRect magicalKnowledgeRect = {};
        // std::make_shared<Button>(magicalPowerRect, COLOR_STATS_MENU_BUTTON, COLOR_STATS_MENU_BUTTON_TEXT, "Magical power: ", .82f, [](){}),
        // std::make_shared<Button>(magicalKnowledgeRect, COLOR_STATS_MENU_BUTTON, COLOR_STATS_MENU_BUTTON_TEXT, "Magical knowledge: ", .82f, [](){}),

        for (int i = 0; i < statButtons.size(); i++) {
            statButtons[i]->rect() = Helper::alignRight(statButtons[i]->rect());
            statsMenuScene->add(statButtons[i]);
            statButtons[i]->setFunction([&decoder, i](){
                decoder.buttonIncrementStat(Parser::c_statNames[i]);
            });
        }

#pragma endregion

#pragma region // MAIN LOOP
        gameTextScene->enable();
        mainMenuScene->disable();
        statsMenuScene->disable();

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

            for (int i = 0; i < g_delayedFunctions.size(); i++) {
                if (g_delayedFunctions[i].checkCall()){
                    g_delayedFunctions.erase(g_delayedFunctions.begin() + i);
                }
            }

            g_game.events(event);
            g_game.render();
            decoder.update();

            SDL_RenderPresent(g_renderer);
        }
#pragma endregion

#pragma region // CLEANUP
        TTF_CloseFont(g_fontMainText);
        TTF_CloseFont(g_fontButtons);
        TTF_CloseFont(g_fontBold);
        TTF_Quit();

        SDL_DestroyRenderer(g_renderer);
        SDL_DestroyWindow(g_window);
        SDL_Quit();
#pragma endregion
    }
}