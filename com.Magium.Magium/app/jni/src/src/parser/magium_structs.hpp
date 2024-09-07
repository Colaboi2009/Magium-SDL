#include <string>
#include <vector>

namespace MagiumSDL::Parser{
    struct Condition{
        std::string condition;
    };

    struct ConditionalText{
        std::string text;
        Condition condition;
    };

    struct Choice{
        std::string nextSceneID;
        std::string text;
        std::vector<std::string> variablesToSet;
        std::vector<std::string> valueOfVariablesToSet;
        Condition condition;
    };

    struct MagiumScene{
        std::string id;
        std::vector<ConditionalText> texts;
        std::vector<Choice> choices;
    };
}