#pragma once

#include <string>
#include <vector>
#include "parser_utility.hpp"

namespace MagiumSDL::Parser{
    const std::string c_magiumFormatFilepath = "magium_format/"; // in the assets folder directory
    const std::string c_fileExtension = ".magium";

    const std::string c_specialAchievement = "achievement";
    const std::string c_specialCheckpointSave = "checkpoint_save";
    const std::string c_specialRestart = "restart";
    const std::string c_specialSaves = "saves";
    const std::string c_specialStats = "stats";

    const std::string c_nextSceneIdVariable = "v_current_scene";
    const std::string c_availablePointsVariable = "v_available_points";
    const std::string c_availablePointsAuxVariable = "v_available_points_aux";
    const std::string c_maxStatVariable = "v_max_stat";
    const std::vector<std::string> c_statNames = { 
        "v_strength", "v_toughness",
        "v_agility", // Speed
        "v_reflexes", "v_hearing",
        "v_perception", // Observation
        "v_ancient_languages", "v_combat_technique", "v_premonition", "v_bluff", "v_magical_sense", "v_aura_hardening",
        "v_magical_power", "v_magical_knowledge", // special stats
    };
    const std::vector<std::string> c_fileNames = {"ch1", "ch2", "ch3", "ch4", "ch5", "ch6", "ch7", "ch8", "ch9", "ch10", "ch11a", "ch11b",
                                                         "b2ch1","b2ch2", "b2ch3", "b2ch4a", "b2ch4b", "b2ch5a", "b2ch5b", "b2ch6", "b2ch7", "b2ch8", "b2ch9a", "b2ch9b", "b2ch10a", "b2ch10b", "b2ch11a",  "b2ch11b", "b2ch11c",
                                                         "b3ch1","b3ch2a", "b3ch2b", "b3ch2c", "b3ch3a", "b3ch3b", "b3ch4a", "b3ch4b", "b3ch5a", "b3ch5b", "b3ch6a", "b3ch6b", "b3ch6c", "b3ch7a", "b3ch8a", "b3ch8b", "b3ch9a", "b3ch9b", "b3ch9c", "b3ch10a", "b3ch10b", "b3ch10c", "b3ch11a", "b3ch12a", "b3ch12b"};
    const std::vector<int> c_bookChapterCount = {11, 11, 12};

    struct VarVal {
        std::string var;
        std::string val;
    };

    struct Condition{
        std::string condition;
    };

    struct VariableSet {
        VarVal variable;
        Condition condition;
    };

    struct ConditionalText{
        std::string text;
        Condition condition;
    };

    struct Choice{
        std::string nextSceneID;
        std::string text;
        std::vector<VarVal> variables;
        std::string special;
        Condition condition;
    };

    struct MagiumScene{
        std::string id;
        std::vector<VariableSet> varSets;
        std::vector<ConditionalText> texts;
        std::vector<Choice> choices;
    };

    enum class BoolOP{
        EQUAL,
        NOT_EQUAL,
        GREATER_EQUAL,
        LESSER_EQUAL,
        GREATER,
        LESSER,
    };
}