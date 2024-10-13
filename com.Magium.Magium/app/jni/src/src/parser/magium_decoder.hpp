#pragma once

#include <SDL3/SDL.h>

#include "../ui/magium_stat.hpp"
#include "../ui/raw_text.hpp"
#include "../ui/ui_scene.hpp"
#include "magium_data.hpp"
#include "magium_structs.hpp"

namespace MagiumSDL {
class MagiumDecoder {
  private:
    std::shared_ptr<UIScene> m_uiscene;
    std::shared_ptr<RawText> m_text;
    std::shared_ptr<RawText> m_chapterCounterText;

    bool m_clicked = false;
    Choice m_choiceTaken;

    MagiumData m_data;
    std::vector<MagiumScene> m_currentChapter;
    std::vector<std::shared_ptr<MagiumStat>> m_uiStats;
    std::shared_ptr<int> m_pAvailablePoints;

  private:
    std::string readFileSDLIOStream(SDL_IOStream *stream);
    std::vector<std::string> separateFiles(std::string &chapter);

    std::string findID(std::string &scene);
    std::string findText(std::string &scene);
    std::string findChoices(std::string &scene);

    std::vector<ConditionalText> parseText(std::string text, std::vector<VariableSet> &sets);
    std::vector<Choice> parseChoices(std::string text);

    std::string evaluateText(std::vector<ConditionalText> &texts);
    void evaluateSetCommands(std::vector<VariableSet> &set);
    std::vector<Choice> evaluateChoices(std::vector<Choice> &choices);
    bool evaluateCondition(Condition condition);

    std::string parseAchievementSpecial(std::string special, std::string &text);
    void showAchievement(std::string text);

    void applyChoice(Choice choice);
    void updateScene(MagiumScene scene);

    void processAndStoreFile(std::string filename);

  public:
    MagiumDecoder(std::shared_ptr<UIScene> scene, std::shared_ptr<RawText> text, std::shared_ptr<RawText> chapterCounterText);

    void setUIStats(std::vector<std::shared_ptr<MagiumStat>> stats, std::shared_ptr<int> pavailablePoints) {
        m_uiStats = stats;
        m_pAvailablePoints = pavailablePoints;
    }
    
	void update();
};
} // namespace MagiumSDL
