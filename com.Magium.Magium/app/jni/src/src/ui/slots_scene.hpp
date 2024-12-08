#pragma once

#include "../parser/magium_decoder.hpp"
#include "../values.hpp"
#include "ui_scene.hpp"

namespace MagiumSDL {

class SlotsScene : public UIScene {
  private:
    MagiumDecoder &m_decoder;

  public:
    SlotsScene(MagiumDecoder &decoder) : m_decoder{decoder} {
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 9; j++) {
                add(std::make_shared<Button>(
                    SDL_FRect{MAIN_SAVE_MENU_BUTTON_TOP_X_POS, MAIN_SAVE_MENU_BUTTON_TOP_Y_POS + MAIN_SAVE_MENU_BUTTON_TOP_Y_POS * j},
					COLOR_MAIN_MENU_BUTTON, COLOR_MAIN_MENU_BUTTON_TEXT, "Save", 1, []() {

					}));
                add(std::make_shared<Button>(SDL_FRect{}, COLOR_MAIN_MENU_BUTTON, COLOR_MAIN_MENU_BUTTON_TEXT, "Load", 1, []() {

                }));
            }
        }
    }

    void events(SDL_Event &event) override { UIScene::events(event); }
};

} // namespace MagiumSDL
