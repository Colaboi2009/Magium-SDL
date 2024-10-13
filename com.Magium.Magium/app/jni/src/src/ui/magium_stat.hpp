#pragma once

#include "../values.hpp"
#include "button.hpp"

namespace MagiumSDL {
class MagiumStat : public UIElement {
  private:
    std::unique_ptr<Button> m_button;
    int m_currentLevel;
    int m_temporaryLevel;
    int m_maxLevel;
    std::shared_ptr<int> m_pAvailablePoints;

    std::string m_name;
    int m_spaceCount;

    void formatName() {
        std::string level = std::to_string(m_temporaryLevel);
        std::string maxLevel = std::to_string(m_maxLevel);
        std::string space = ":" + std::string(m_spaceCount, ' ');
        std::string formatted = m_name + space + level + "/" + maxLevel;
        m_button->rawText()->changeText(formatted);
        m_button->fitToText();
    }

  public:
    MagiumStat(std::string name, SDL_FRect rect, std::shared_ptr<int> pavailablePoints, int spaceCount = 1)
        : m_currentLevel{0}, m_temporaryLevel{0}, m_maxLevel{3}, m_name{name}, m_pAvailablePoints{pavailablePoints},
          m_spaceCount{spaceCount} {
        m_rect = Helper::swiths(rect);
        int *pTemporaryLevel = &m_temporaryLevel;
        int maxLevel = m_maxLevel;
        m_button = std::make_unique<Button>(m_rect, COLOR_STATS_MENU_STATS_BUTTON, COLOR_STATS_MENU_STATS_TEXT, m_name, STATS_TEXT_SIZE,
                                            [pTemporaryLevel, maxLevel, pavailablePoints] {
                                                if (*pTemporaryLevel < maxLevel && *pavailablePoints > 0) {
                                                    *pTemporaryLevel += 1;
                                                    *pavailablePoints -= 1;
                                                }
                                            });
        m_rect = Helper::alignRight(m_button->rect());
        m_button->rect() = m_rect;

        formatName();
    }

    std::string name() const { return m_name; }
    int level() const { return m_currentLevel; }

    void setMaxLevel(int max) {
        m_maxLevel = max;
        formatName();
    }

    void confirmStatLevel() {
        m_currentLevel = m_temporaryLevel;
        m_button->rawText()->setColor(COLOR_STATS_MENU_STATS_TEXT);
        formatName();
    }

    void cancelStatLevel() {
		*m_pAvailablePoints += m_temporaryLevel - m_currentLevel;
        m_temporaryLevel = m_currentLevel;
        m_button->rawText()->setColor(COLOR_STATS_MENU_STATS_TEXT);
        formatName();
    }

    void render() override {
        if (m_temporaryLevel != m_currentLevel) {
            m_button->rawText()->setColor(COLOR_STATS_MENU_STATS_TEXT_CHANGED);
            formatName();
        }
        m_button->render();
    }

    void events(SDL_Event &e) override { m_button->events(e); }
};
} // namespace MagiumSDL
