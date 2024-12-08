#pragma once

#include <SDL3/SDL.h>

#include "../ui/magium_stat.hpp"
#include "../ui/raw_text.hpp"
#include "../ui/ui_scene.hpp"
#include "data_view.hpp"
#include "magium_structs.hpp"
#include "parser.hpp"
#include "persister.hpp"

namespace MagiumSDL {
// ftr: Decoder meaning Engine
class MagiumDecoder {
  private:
    std::shared_ptr<UIScene> m_uiscene;
    std::shared_ptr<RawText> m_text;
    std::shared_ptr<RawText> m_chapterCounterText;

    bool m_clicked = false;
    Choice m_choiceTaken;

    Persister m_persister;
    DataView m_view;
    Parser m_parser;

    std::vector<std::shared_ptr<MagiumStat>> m_uiStats;
    std::shared_ptr<int> m_pAvailablePoints;

    std::string parseAchievementSpecial(std::string special, std::string &text);
    void showAchievement(std::string text);

    void applyChoice(Choice choice);
    void updateScene();

    // special cases
  private:
    int m_berserkPointsProxy;
    void berserkMode(int maxStat);

  public:
    MagiumDecoder(std::shared_ptr<UIScene> scene, std::shared_ptr<RawText> text, std::shared_ptr<RawText> chapterCounterText);

    void setUIStats(std::vector<std::shared_ptr<MagiumStat>> stats, std::shared_ptr<int> pavailablePoints) {
        m_uiStats = stats;
        m_pAvailablePoints = pavailablePoints;
    }

    void restart();
    void load(std::string name);
    void update();
};
} // namespace MagiumSDL
