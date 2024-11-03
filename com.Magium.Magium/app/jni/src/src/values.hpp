#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace {

// Positions and sizes
// Game
float STORY_TEXT_SIZE{.7f};
float STORY_BUTTON_WIDTH{.875f};
float STORY_BUTTON_HEIGHT{.055f};
float STORY_BUTTON_SPACING{.08f};
float STORY_BUTTON_BOTTOM_PADDING{.1f};

SDL_FRect RECT_ACHIEVEMENT_BOX{0.09f, 0.1f, 1.5f};         // w = scale
SDL_FRect RECT_ACHIEVEMENT_TROPHY{0.14f, 0.1125f, 1.5f};   // w = scale
SDL_FRect RECT_ACHIEVEMENT_TEXT{0.33f, 0.13f, 0.2f, .82f}; // w = wrapping length, h = font size

float STATS_TEXT_SIZE{.72f};

// Main menu
const float MAIN_MENU_BUTTON_WIDTH = .8f;
const float MAIN_MENU_BUTTON_HEIGHT = .05f;
const float MAIN_MENU_BUTTON_DISTANCE = 0.06f;
const float MAIN_MENU_BUTTON_PADDING = 0.05f;

// TODO(Ala): stat positions
const SDL_FRect RECT_strength{.275f, .25f};
const SDL_FRect RECT_speed{.8f, .25f};
const SDL_FRect RECT_toughness{.275f, .35f};
const SDL_FRect RECT_reflexes{.8f, .35f};
const SDL_FRect RECT_hearing{.275f, .45f};
const SDL_FRect RECT_observation{.8f, .45f};
const SDL_FRect RECT_ancientLanguages{.65f, .55f};
const SDL_FRect RECT_combatTechnique{.65f, .62f};
const SDL_FRect RECT_premonition{.65f, .69f};
const SDL_FRect RECT_bluff{.65f, .76f};
const SDL_FRect RECT_magicalSense{.345f, .83f};
const SDL_FRect RECT_auraHardening{.85f, .83f};

// Colors
SDL_Color COLOR_GAME_TEXT_TOP_BACKGROUND{180, 180, 180, 255};
SDL_Color COLOR_GAME_TEXT_BOTTOM_BACKGROUND{0, 0, 0, 255};
SDL_Color COLOR_GAME_TEXT_MENU_BUTTON{180, 180, 180, 255};
SDL_Color COLOR_GAME_TEXT_MENU_BUTTON_TEXT{0, 0, 0, 255};
SDL_Color COLOR_GAME_TEXT_STORY_TEXT{255, 255, 255, 255};
SDL_Color COLOR_GAME_TEXT_OPTION_BUTTON{80, 80, 80, 255};

SDL_Color COLOR_MAIN_MENU_BACKGROUND{255, 255, 255, 255};
SDL_Color COLOR_MAIN_MENU_BUTTON{20, 20, 20, 255};
SDL_Color COLOR_MAIN_MENU_BUTTON_TEXT{255, 255, 255, 255};

SDL_Color COLOR_STATS_MENU_BACKGROUND{255, 255, 255, 255};
SDL_Color COLOR_STATS_MENU_BUTTON{20, 20, 20, 255};
SDL_Color COLOR_STATS_MENU_BUTTON_TEXT{255, 255, 255, 255};
SDL_Color COLOR_STATS_MENU_STATS_BUTTON{255, 255, 255, 255};
SDL_Color COLOR_STATS_MENU_STATS_TEXT{0, 0, 0, 255};
SDL_Color COLOR_STATS_MENU_STATS_TEXT_CHANGED{0, 255, 0, 255};
SDL_Color COLOR_STATS_MENU_AVAILABLE_POINTS_TEXT{189, 89, 25, 255};
} // namespace
