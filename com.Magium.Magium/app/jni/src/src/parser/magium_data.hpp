#pragma once

#include "magium_structs.hpp"
#include <SDL3/SDL.h>

namespace MagiumSDL {
class MagiumData {
  private:
    std::vector<std::string> m_achievements;
    std::vector<VarVal> m_variables;
    int m_currentFileIndex;
    int m_currentChapterIndex;
    int m_currentBookIndex;
    std::string m_sceneId;

    std::string nullstring = "";

    void debugInit() {
        // m_variables =
        // {{"v_available_points","7"},{"v_available_points_aux","7"},{"v_strength","0"},{"v_toughness","0"},{"v_agility","0"},{"v_reflexes","0"},{"v_hearing","0"},{"v_perception","0"},{"v_ancient_languages","0"},{"v_combat_technique","0"},{"v_premonition","0"},{"v_bluff","0"},{"v_magical_sense","0"},{"v_aura_hardening","0"},{"v_magical_power","0"},{"v_magical_knowledge","0"},{"v_current_scene","Ch10-Never"},{"v_ch1_intro_feeling","3"},{"v_ch1_show_yourself","3"},{"v_ch1_stillwater","2"},{"v_ac_ch1_honesty","1"},{"v_ch2_human_reason","1"},{"v_chapter_save_counter","5"},{"v_checkpoint_rich","0"},{"v_ch2_kate_whisper","4"},{"v_ch2_toughness_happened","0"},{"v_ch2_kate_debt","1"},{"v_ch2_not_owe_debt","1"},{"v_ch2_deer_interaction","0"},{"v_next_chapter_crash","1"},{"v_ch3_tactics","3"},{"v_ch3_star_message","0"},{"v_ch3_seduce","3"},{"v_ch3_awkward","1"},{"v_ch3_daren_worry","1"},{"v_ch3_saved_from_dart","0"},{"v_ch3_treasure","0"},{"v_is_dead","1"},{"v_ch3_kate_badly_hurt","1"},{"v_ch4_objectives","1"},{"v_ch4_dave_defeat","0"},{"v_ch4_ending","6"},{"v_ch5_ignore_scream","0"},{"v_ch5_goblins","0"},{"v_available_stats","3"},{"v_available_stats_aux","3"},{"v_ch6_flirt","6"},{"v_max_stat","4"},{"v_ch6_eiden_answer","3"},{"v_ch6_improvise","2"},{"v_ch6_defense","3"},{"v_ch6_offense","3"},{"v_ch6_won_dragon","1"},{"v_ch6_lessathi","2"},{"v_ch8_talk","1"},{"v_ch8_first_puzzle","0"},{"v_ch8_smash","0"},{"v_ch8_old","1"},{"v_ch9_inventory","0"},{"v_ch9_take_rose","1"},{"v_ch9_ogres","1"},{"v_ch10_gems","0"},{"v_ch10_kate_scratched","1"}};
        m_currentFileIndex = 0;
        m_currentChapterIndex = 0;
        m_currentBookIndex = 0;
        addVarVal({
            .var = c_availablePointsVariable,
            .val = "3",
        });
        addVarVal({
            .var = c_availablePointsAuxVariable,
            .val = "3",
        });
        addVarVal({
            .var = c_maxStatVariable,
            .val = "3",
        });
        for (std::string s : c_statVariableNames) {
            addVarVal({
                .var = s,
                .val = "0",
            });
        }
    }

  public:
    MagiumData() { debugInit(); }

    bool exists(std::string var) {
        for (VarVal &v : m_variables) {
            if (v.var == var)
                return true;
        }
        return false;
    }

    std::string &getVal(std::string var) {
        for (VarVal &v : m_variables) {
            if (v.var == var)
                return v.val;
        }
        SDL_LogError(SDL_LOG_PRIORITY_ERROR, "Make sure var exists before calling getVal()!");
        return nullstring;
    }

    void addVarVal(VarVal varVal) {
        if (exists(varVal.var)) {
            if (varVal.val.find("+") != varVal.val.npos) {
                std::string &val = getVal(varVal.var);
                val = std::to_string(std::stoi(val) + std::stoi(varVal.val));
            } else {
                getVal(varVal.var) = varVal.val;
            }
        } else {
            if (int pos = varVal.val.find("+") != varVal.val.npos)
                varVal.val.erase(pos, 1);
            m_variables.push_back(varVal);
        }
    }

    bool addAchievementExists(std::string achievement) {
        for (std::string a : m_achievements) {
            if (a == achievement)
                return true;
        }
        m_achievements.push_back(achievement);
        return false;
    }

    const int fileIndex() { return m_currentFileIndex; }
    void incrementFileIndex() { m_currentFileIndex++; }

    const int chapterNumber() { return m_currentChapterIndex + 1; }
    void incrementChapterNumber() { m_currentChapterIndex++; }
    void resetChapterNumber() { m_currentChapterIndex = 0; }

    const int bookNumber() { return m_currentBookIndex + 1; }
    void incrementBookNumber() { m_currentBookIndex++; }

    const std::string sceneId() { return m_sceneId; }
    void setSceneId(std::string id) { m_sceneId = id; }

    void clear() { m_variables.clear(); }

    std::vector<VarVal> getAll() { return m_variables; }

    bool evaluateVarVal(VarVal varVal, BoolOP op) {
        if (exists(varVal.var)) {
            std::string &val = getVal(varVal.var);
            switch (op) {
            case BoolOP::EQUAL:
                return val == varVal.val;
            case BoolOP::NOT_EQUAL:
                return val != varVal.val;
            case BoolOP::GREATER_EQUAL:
                return std::stoi(val) >= std::stoi(varVal.val);
            case BoolOP::LESSER_EQUAL:
                return std::stoi(val) <= std::stoi(varVal.val);
            case BoolOP::GREATER:
                return std::stoi(val) > std::stoi(varVal.val);
            case BoolOP::LESSER:
                return std::stoi(val) < std::stoi(varVal.val);
            }
        } else {
            return varVal.val == "0" && op == BoolOP::EQUAL;
        }
    }
};
} // namespace MagiumSDL
