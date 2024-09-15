#pragma once

#include <string>
#include <vector>

namespace MagiumSDL::Parser{
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
        GREATER,
        LESSER,
        GREATER_EQUAL,
        LESSER_EQUAL,
    };
}