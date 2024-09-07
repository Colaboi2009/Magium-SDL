#include <SDL3/SDL.h>

#include "magium_structs.hpp"
#include "../ui/ui_scene.hpp"

namespace MagiumSDL::Parser{
    class MagiumDecoder{
    private:        
        std::vector<std::string> separateFiles(std::string &chapter);
        
        std::string findID(std::string &scene);
        std::string findText(std::string &scene);
        std::string findChoices(std::string &scene);
        
        std::vector<ConditionalText> parseText(std::string text) {};
        
        std::vector<Choice> parseChoices(std::string choices);
        void parseConditions(std::vector<Condition> conditions) {};
        void setVariables() {};

        const std::string c_magiumFormatFilepath = "magium_format/";
        std::vector<MagiumScene> m_currentChapter;

    public:
        /// @brief loads a magium file which contains a chapter (or part of a chapter) and parses its contents.
        /// @param filename only the file name, do not specify path.
        void processAndStoreFile(std::string filename);
        /// @brief creates a scene from an id.
        /// @param id when empty, it returns the first scene.
        /// @return the scene with the given id, or empty scene if it was not found.
        UIScene createSceneFromID(std::string id); // 
    };
}