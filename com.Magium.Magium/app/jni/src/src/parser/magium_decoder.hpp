#pragma once

#include <SDL3/SDL.h>

#include "magium_structs.hpp"
#include "magium_data.hpp"
#include "../ui/ui_scene.hpp"
#include "../ui/raw_text.hpp"
#include "../ui/button.hpp"


namespace MagiumSDL::Parser{
    class MagiumDecoder{
    private:
        const std::string c_magiumFormatFilepath = "magium_format/"; // in the assets folder directory

        const std::string c_specialAchievement = "achievement";
        const std::string c_specialCheckpointSave = "checkpoint_save";
        const std::string c_specialRestart = "restart";
        const std::string c_specialSaves = "saves";
        const std::string c_specialStats = "stats";

        const std::string c_nextSceneIdVariable = "v_current_scene";
        const std::string c_availablePointsVariable = "v_available_points";
        const std::string c_maxStatVariable = "v_max_stat";
        const std::vector<std::string> c_statNames = { 
            "v_strength", "v_toughness",
            "v_agility", // Speed
            "v_reflexes", "v_hearing",
            "v_perception", // Observation
            "v_ancient_languages", "v_combat_technique", "v_premonition", "v_bluff", "v_magical_sense", "v_aura_hardening",
            "v_magical_power", // Currently, not utilized
            "v_magical_knowledge", // Currently, not utilized
        };
        const std::vector<std::string> c_chapterNames = {"ch1", "ch2", "ch3", "ch4", "ch5", "ch6", "ch7", "ch8", "ch9", "ch10", "ch11a", "ch11b",
                                                         "b2ch1","b2ch2", "b2ch3", "b2ch4a", "b2ch4b", "b2ch5a", "b2ch5b", "b2ch6", "b2ch7", "b2ch8", "b2ch9a", "b2ch9b", "b2ch10a", "b2ch10b", "b2ch11a",  "b2ch11b", "b2ch11c",
                                                         "b3ch1","b3ch2a", "b3ch2b", "b3ch2c", "b3ch3a", "b3ch3b", "b3ch4a", "b3ch4b", "b3ch5a", "b3ch5b", "b3ch6a", "b3ch6b", "b3ch6c", "b3ch7a", "b3ch8a", "b3ch8b", "b3ch9a", "b3ch9b", "b3ch9c", "b3ch10a", "b3ch10b", "b3ch10c", "b3ch11a", "b3ch12a", "b3ch12b"};
        
        std::shared_ptr<Button> m_buttonStrength, m_buttonSpeed, m_buttonToughness, m_buttonReflexes, 
                     m_buttonHearing, m_buttonObservation, m_buttonAncientLanguages, m_buttonCombatTechnique, 
                     m_buttonPremonition, m_buttonBluff, m_buttonMagicalSense, m_buttonAuraHardening, 
                     m_buttonMagicalPower, m_buttonMagicalKnowledge;

        std::shared_ptr<UIScene> m_uiscene;
        std::shared_ptr<RawText> m_text;
        std::shared_ptr<RawText> m_chapterCounterText;

        bool m_clicked;
        // when m_clicked is true this has a useable value
        Choice m_choiceTaken;

        MagiumData m_data;
        std::vector<MagiumScene> m_currentChapter;
        std::string m_currentId;
        int m_currentChapterNumber = 1;
        int m_currentBookNumber = 1;
    
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

        void applyChoice(Choice choice);
        void updateScene(std::string id);

    public:
        MagiumDecoder(std::shared_ptr<UIScene> scene, std::shared_ptr<RawText> text, std::shared_ptr<RawText> chapterCounterText);
        void setStatVariables(std::shared_ptr<Button> strength, std::shared_ptr<Button> speed, std::shared_ptr<Button> toughness, std::shared_ptr<Button> reflexes, 
                      std::shared_ptr<Button> hearing, std::shared_ptr<Button> observation, std::shared_ptr<Button> ancientLanguages, std::shared_ptr<Button> combatTechnique, 
                      std::shared_ptr<Button> premonition, std::shared_ptr<Button> bluff, std::shared_ptr<Button> magicalSense, std::shared_ptr<Button> auraHardening, 
                      std::shared_ptr<Button> magicalPower, std::shared_ptr<Button> magicalKnowledge);

        void processAndStoreFile(std::string filename);
        void update();
    };
}