#include <SDL3/SDL.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_system.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "helper/helper.hpp"
#include "parser/magium_decoder.hpp"
#include "ui/button.hpp"
#include "ui/magium_stat.hpp"
#include "ui/raw_image.hpp"
#include "ui/scroller.hpp"
#include "ui/ui_scene.hpp"
#include "values.hpp"

using namespace MagiumSDL;

// Globals
SDL_Window *g_window;
SDL_Renderer *g_renderer;

TTF_Font *g_fontMainText;
TTF_Font *g_fontButtons;
TTF_Font *g_fontBold;

float g_width;
float g_height;

std::vector<Helper::DelayedFunction> g_delayedFunctions;

UIScene g_game = {};
std::shared_ptr<UIScene> gameTextScene = std::make_shared<UIScene>();
std::shared_ptr<UIScene> mainMenuScene = std::make_shared<UIScene>();
std::shared_ptr<UIScene> statsMenuScene = std::make_shared<UIScene>();

std::unique_ptr<MagiumDecoder> decoder;

void initSDL() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    g_window = SDL_CreateWindow("Magium", SDL_WINDOW_FULLSCREEN, SDL_WINDOW_FULLSCREEN, 0);
    g_renderer = SDL_CreateRenderer(g_window, 0);
    SDL_SetRenderVSync(g_renderer, SDL_RENDERER_VSYNC_DISABLED);

    IMG_Init(IMG_INIT_PNG);

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
}

void setupGameTextScene() {
    g_game.add(gameTextScene);

    const float sidePadding = .01;
    const float topPadding = .11;
    const float bottomPadding = .04f;

    const float scrollerBottomPadding = .1f;
    const float scrollerHeight = 1.f;

    std::shared_ptr<UIScene> scrollContent = std::make_shared<UIScene>();
    std::shared_ptr<RawText> contentText = std::make_shared<RawText>(g_width * sidePadding, g_height * topPadding, "placeholder", .82f,
                                                                     COLOR_GAME_TEXT_STORY_TEXT, g_fontMainText, g_width * (1.f - sidePadding * 2.f));
    scrollContent->add(contentText);

    SDL_FRect scrollerRect = {0, 0, contentText->rect().w, g_height * (scrollerHeight - bottomPadding - scrollerBottomPadding)};
    std::shared_ptr<Scroller> scroller = std::make_shared<Scroller>(scrollerRect, scrollContent);
    gameTextScene->add(scroller);

    std::shared_ptr<RawImage> topBar = std::make_shared<RawImage>(Helper::swiths(0, .05f, 1, .05f), COLOR_GAME_TEXT_TOP_BACKGROUND);
    gameTextScene->add(topBar);

    std::shared_ptr<Button> mainMenuButton =
        std::make_shared<Button>(Helper::swiths(0, .05f, .3f, .05f), COLOR_GAME_TEXT_MENU_BUTTON, COLOR_GAME_TEXT_MENU_BUTTON_TEXT, "MAIN MENU", .8f, []() {
            mainMenuScene->enable();
            gameTextScene->disable();
        });
    gameTextScene->add(mainMenuButton);

    std::shared_ptr<Button> statsButton =
        std::make_shared<Button>(Helper::swiths(.8f, .05f, .2f, .05f), COLOR_GAME_TEXT_MENU_BUTTON, COLOR_GAME_TEXT_MENU_BUTTON_TEXT, "STATS", .8f, []() {
            statsMenuScene->enable();
            gameTextScene->disable();
        });
    gameTextScene->add(statsButton);
    std::shared_ptr<RawText> chapterText =
        std::make_shared<RawText>(g_width * 0.3f, g_height * .0618f, "Chapter 1", .8f, COLOR_GAME_TEXT_MENU_BUTTON_TEXT, g_fontButtons);
    gameTextScene->add(chapterText);

    std::shared_ptr<RawImage> bottomBar =
        std::make_shared<RawImage>(Helper::swiths(0, 1.f - bottomPadding, 1.f, bottomPadding), COLOR_GAME_TEXT_BOTTOM_BACKGROUND);
    gameTextScene->add(bottomBar);

    std::shared_ptr<RawImage> topHider = std::make_shared<RawImage>(SDL_FRect{0, -100.f, g_width, topBar->rect().h + 100.f}, COLOR_GAME_TEXT_BOTTOM_BACKGROUND);
    gameTextScene->add(topHider);

    decoder = std::make_unique<MagiumDecoder>(scrollContent, contentText, chapterText);
}

void setupMainMenuScene() {
    g_game.add(mainMenuScene);

    std::shared_ptr<RawImage> mainMenuBackground = std::make_shared<RawImage>(SDL_FRect{0, 0, g_width, g_height}, COLOR_MAIN_MENU_BACKGROUND);
    mainMenuScene->add(mainMenuBackground);

    SDL_FRect mainMenuButtonsPos = {g_width / 2.f, g_height * (MAIN_MENU_BUTTON_DISTANCE + MAIN_MENU_BUTTON_PADDING), g_width * MAIN_MENU_BUTTON_WIDTH,
                                    g_height * MAIN_MENU_BUTTON_HEIGHT};
    mainMenuButtonsPos = Helper::centerRectX(mainMenuButtonsPos);

    mainMenuButtonsPos.y += MAIN_MENU_BUTTON_DISTANCE * g_height;
    std::shared_ptr<Button> mainMenuReturnButton =
        std::make_shared<Button>(mainMenuButtonsPos, COLOR_MAIN_MENU_BUTTON, COLOR_MAIN_MENU_BUTTON_TEXT, "Return To Game", .8f, []() {
            gameTextScene->enable();
            mainMenuScene->disable();
        });
    mainMenuScene->add(mainMenuReturnButton);

    mainMenuButtonsPos.y += MAIN_MENU_BUTTON_DISTANCE * g_height;
    std::shared_ptr<Button> newGameButton =
        std::make_shared<Button>(mainMenuButtonsPos, COLOR_MAIN_MENU_BUTTON, COLOR_MAIN_MENU_BUTTON_TEXT, "New game / Restart Book", .8f, []() {

        });
    mainMenuScene->add(newGameButton);

    mainMenuButtonsPos.y += MAIN_MENU_BUTTON_DISTANCE * g_height;
    std::shared_ptr<Button> loadFromLastCheckpointButton =
        std::make_shared<Button>(mainMenuButtonsPos, COLOR_MAIN_MENU_BUTTON, COLOR_MAIN_MENU_BUTTON_TEXT, "Load from last checkpoint", .8f, []() {

        });
    mainMenuScene->add(loadFromLastCheckpointButton);

    mainMenuButtonsPos.y += MAIN_MENU_BUTTON_DISTANCE * g_height;
    std::shared_ptr<Button> saveLoadGameButton =
        std::make_shared<Button>(mainMenuButtonsPos, COLOR_MAIN_MENU_BUTTON, COLOR_MAIN_MENU_BUTTON_TEXT, "Save/Load game", .8f, []() {

        });
    mainMenuScene->add(saveLoadGameButton);

    mainMenuButtonsPos.y += MAIN_MENU_BUTTON_DISTANCE * g_height;
    std::shared_ptr<Button> aboutButton = std::make_shared<Button>(mainMenuButtonsPos, COLOR_MAIN_MENU_BUTTON, COLOR_MAIN_MENU_BUTTON_TEXT, "About", .8f, []() {

    });
    mainMenuScene->add(aboutButton);
}

void setupStatsMenuScene() {
    g_game.add(statsMenuScene);

    std::shared_ptr<RawImage> statsMenuBackground = std::make_shared<RawImage>(SDL_FRect{0, 0, g_width, g_height}, COLOR_STATS_MENU_BACKGROUND);
    statsMenuScene->add(statsMenuBackground);

    std::shared_ptr<RawText> availablePointsText =
        std::make_shared<RawText>(.5f * g_width, .15f * g_height, "Available points: 26", .82f, COLOR_STATS_MENU_AVAILABLE_POINTS_TEXT, g_fontMainText);
    availablePointsText->rect() = Helper::centerRectX(availablePointsText->rect());
    statsMenuScene->add(availablePointsText);

    std::shared_ptr<int> pAvailablePoints = std::make_shared<int>(36);
    std::vector<std::shared_ptr<MagiumStat>> stats = {
        std::make_shared<MagiumStat>("Strength", RECT_strength, pAvailablePoints),
        std::make_shared<MagiumStat>("Toughness", RECT_toughness, pAvailablePoints),
        std::make_shared<MagiumStat>("Speed", RECT_speed, pAvailablePoints),
        std::make_shared<MagiumStat>("Reflexes", RECT_reflexes, pAvailablePoints),
        std::make_shared<MagiumStat>("Hearing", RECT_hearing, pAvailablePoints),
        std::make_shared<MagiumStat>("Observation", RECT_observation, pAvailablePoints),
        std::make_shared<MagiumStat>("Ancient languages", RECT_ancientLanguages, pAvailablePoints, 2),
        std::make_shared<MagiumStat>("Combat technique", RECT_combatTechnique, pAvailablePoints, 2),
        std::make_shared<MagiumStat>("Premonition", RECT_premonition, pAvailablePoints, 2),
        std::make_shared<MagiumStat>("Bluff (Aura concealment)", RECT_bluff, pAvailablePoints, 2),
        std::make_shared<MagiumStat>("Magical sense", RECT_magicalSense, pAvailablePoints),
        std::make_shared<MagiumStat>("Aura hardening", RECT_auraHardening, pAvailablePoints),
    };

    // TODO(Ala): magical power and knowledge.

    for (int i = 0; i < stats.size(); i++) {
        statsMenuScene->add(stats[i]);
    }
    decoder->setUIStats(stats, pAvailablePoints);

    const float statsMenuButtonY = .89f;
    const float statsMenuButtonWidth = .4f;
    const float statsMenuButtonHeight = .05f;
    SDL_FRect statsMenuButtonPos = Helper::swiths(0, statsMenuButtonY, statsMenuButtonWidth, statsMenuButtonHeight);
    statsMenuButtonPos.x = g_width * 0.05f;
    std::shared_ptr<Button> statsMenuCancelButton =
        std::make_shared<Button>(statsMenuButtonPos, COLOR_STATS_MENU_BUTTON, COLOR_STATS_MENU_BUTTON_TEXT, "Cancel changes", .8f, [stats]() {
            for (std::shared_ptr<MagiumStat> s : stats) {
                s->cancelStatLevel();
            }
        });
    statsMenuScene->add(statsMenuCancelButton);
    statsMenuButtonPos.x = g_width - statsMenuButtonPos.x - g_width * statsMenuButtonWidth;
    std::shared_ptr<Button> statsMenuReturnButton =
        std::make_shared<Button>(statsMenuButtonPos, COLOR_STATS_MENU_BUTTON, COLOR_STATS_MENU_BUTTON_TEXT, "Return to game", .8f, []() {
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
}

void cleanup() {
    TTF_CloseFont(g_fontMainText);
    TTF_CloseFont(g_fontButtons);
    TTF_CloseFont(g_fontBold);
    TTF_Quit();

    IMG_Quit();

    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    SDL_Quit();
}

int main(int argc, char *argv[]) {
    initSDL();
    setupGameTextScene();
    setupMainMenuScene();
    setupStatsMenuScene();

    gameTextScene->enable();
    mainMenuScene->disable();
    statsMenuScene->disable();

    while (true) {
        SDL_Event event;
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                break;
        }
        SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255);
        SDL_RenderClear(g_renderer);
        // TODO(Ala): Clean delayed functions code (and move it into a function to be called here, in delayed_functions.hpp).
        for (int i = 0; i < g_delayedFunctions.size(); i++) {
            if (g_delayedFunctions[i].checkCall()) {
                g_delayedFunctions.erase(g_delayedFunctions.begin() + i);
            }
        }

        g_game.events(event);
        g_game.render();
        decoder->update();

        SDL_RenderPresent(g_renderer);
    }

    cleanup();

    return 0;
}
