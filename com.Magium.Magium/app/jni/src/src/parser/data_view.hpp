#pragma once

#include "magium_structs.hpp"
#include <SDL3/SDL.h>

namespace MagiumSDL {
class DataView {
  private:
    std::vector<std::string> m_achievements{};
    std::vector<VarVal> m_variables{};
    int m_bookIndex{0};
    int m_fileIndex{0};
    int m_chapterIndex{0};
    std::string m_sceneId;

    std::string nullstring{"0"};

  public:
    DataView();
    const int fileIndex() { return m_fileIndex; }
    void incrementFileIndex() { m_fileIndex++; }

    const int chapterNumber() { return m_chapterIndex + 1; }
    void incrementChapterNumber() { m_chapterIndex++; }
    void resetChapterNumber() { m_chapterIndex = 0; }

    const int bookNumber() { return m_bookIndex + 1; }
    void incrementBookNumber() { m_bookIndex++; }

    const std::string sceneId() { return m_sceneId; }
    void setSceneId(std::string id) { m_sceneId = id; }

    std::vector<std::string> achievements() { return m_achievements; }

    void clear() { m_variables.clear(); }

    std::vector<VarVal> getAll() { return m_variables; }

	std::string getChapterName() const;
	void nextChapter();

    void setupData(std::vector<std::string> achievements, DataSlot s);
    DataSlot slot();
    bool exists(std::string var);
    std::string &getVal(std::string var);

    void addVarVal(VarVal varVal);
    bool addAchievementExists(std::string achievement);
    bool evaluateVarVal(VarVal varVal, BoolOP op);

    std::string evaluateText(std::vector<ConditionalText> &texts);
	// returns stat check
	std::vector<std::pair<std::string, bool>> evaluateSetCommands(std::vector<VariableSet> &set);
    std::vector<Choice> evaluateChoices(std::vector<Choice> &choices);
    bool evaluateCondition(Condition condition, std::string *conditionPassed = nullptr);
};
} // namespace MagiumSDL
