#pragma once

#include "magium_structs.hpp"
#include <SDL3/SDL.h>

namespace MagiumSDL {
class DataView {
  private:
    std::vector<std::string> m_achievements;
    std::vector<VarVal> m_variables;
    int m_bookIndex;
    int m_fileIndex;
    int m_chapterIndex;
    std::string m_sceneId;

    std::string nullstring = "";

    // this is for playing the game with custom points, without messing with the 'persistent_data.txt' file
    void debugInit() {
        // m_variables =
        // {{"v_available_points","7"},{"v_available_points_aux","7"},{"v_strength","0"},{"v_toughness","0"},{"v_agility","0"},{"v_reflexes","0"},{"v_hearing","0"},{"v_perception","0"},{"v_ancient_languages","0"},{"v_combat_technique","0"},{"v_premonition","0"},{"v_bluff","0"},{"v_magical_sense","0"},{"v_aura_hardening","0"},{"v_magical_power","0"},{"v_magical_knowledge","0"},{"v_current_scene","Ch10-Never"},{"v_ch1_intro_feeling","3"},{"v_ch1_show_yourself","3"},{"v_ch1_stillwater","2"},{"v_ac_ch1_honesty","1"},{"v_ch2_human_reason","1"},{"v_chapter_save_counter","5"},{"v_checkpoint_rich","0"},{"v_ch2_kate_whisper","4"},{"v_ch2_toughness_happened","0"},{"v_ch2_kate_debt","1"},{"v_ch2_not_owe_debt","1"},{"v_ch2_deer_interaction","0"},{"v_next_chapter_crash","1"},{"v_ch3_tactics","3"},{"v_ch3_star_message","0"},{"v_ch3_seduce","3"},{"v_ch3_awkward","1"},{"v_ch3_daren_worry","1"},{"v_ch3_saved_from_dart","0"},{"v_ch3_treasure","0"},{"v_is_dead","1"},{"v_ch3_kate_badly_hurt","1"},{"v_ch4_objectives","1"},{"v_ch4_dave_defeat","0"},{"v_ch4_ending","6"},{"v_ch5_ignore_scream","0"},{"v_ch5_goblins","0"},{"v_available_stats","3"},{"v_available_stats_aux","3"},{"v_ch6_flirt","6"},{"v_max_stat","4"},{"v_ch6_eiden_answer","3"},{"v_ch6_improvise","2"},{"v_ch6_defense","3"},{"v_ch6_offense","3"},{"v_ch6_won_dragon","1"},{"v_ch6_lessathi","2"},{"v_ch8_talk","1"},{"v_ch8_first_puzzle","0"},{"v_ch8_smash","0"},{"v_ch8_old","1"},{"v_ch9_inventory","0"},{"v_ch9_take_rose","1"},{"v_ch9_ogres","1"},{"v_ch10_gems","0"},{"v_ch10_kate_scratched","1"}};
        m_fileIndex = 0;
        m_chapterIndex = 0;
        m_bookIndex = 0;
        m_sceneId = "";
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
    DataView();

    const int fileIndex() { return m_fileIndex; }
    void incrementFileIndex() { m_fileIndex++; }

    const int chapterIndex() { return m_chapterIndex; }
    void incrementChapterNumber() { m_chapterIndex++; }
    void resetChapterNumber() { m_chapterIndex = 0; }

    const int bookIndex() const { return m_bookIndex; }
    void incrementBookNumber() { m_bookIndex++; }

    const std::string sceneId() { return m_sceneId; }
    void setSceneId(std::string id) { m_sceneId = id; }

    std::vector<std::string> achievements() { return m_achievements; }

    std::vector<VarVal> getAllVariables() { return m_variables; }
    void clear() { m_variables.clear(); }

    std::string getChapterName() const;

	void nextChapter();

    void setupData(std::vector<std::string> achievements, DataSlot s);
    DataSlot slot();

    bool exists(std::string var);
    std::string &getVal(std::string var);
    void addVarVal(VarVal varVal);
    bool addAchievementExists(std::string achievement);
    bool evaluateVarVal(VarVal varVal, BoolOP op);
};
} // namespace MagiumSDL
