#include "magium_decoder.hpp"

#include <fstream>

namespace MagiumSDL::Parser{
    std::vector<std::string> MagiumDecoder::separateFiles(std::string &chapter)
    {
        std::vector<std::string> separated;
        int currentIdPos = 0;
        while ((currentIdPos = chapter.find("ID:", currentIdPos)) != chapter.npos){
            int nextIdPos = chapter.find("ID:", currentIdPos);
            separated.push_back(chapter.substr(currentIdPos - 3, nextIdPos - currentIdPos));
        }
        return separated;
    }

    std::string MagiumDecoder::findID(std::string &scene)
    {
        int idPos = scene.find("ID:");
        int endPos = scene.find("\n");
        return scene.substr(idPos, endPos - idPos);
    }

    std::string MagiumDecoder::findText(std::string &scene)
    {
        return std::string();
    }

    std::string MagiumDecoder::findChoices(std::string &scene)
    {
        return std::string();
    }

    std::vector<Choice> MagiumDecoder::parseChoices(std::string choices)
    {
        return std::vector<Choice>();
    }

    void MagiumDecoder::processAndStoreFile(std::string filename) // incomplete
    {
        std::ifstream file(c_magiumFormatFilepath + filename);
        size_t size;
        std::string grouped;
        while (!file.eof()){
            std::string line;
            std::getline(file, line);
            grouped += line;            
        }
        std::vector<std::string> scenes = separateFiles(grouped);

        m_currentChapter.clear();
        for (int i = 0; i < scenes.size(); i++){
            std::string sceneStr = scenes[i];
            
            MagiumScene scene;
            scene.id = findID(sceneStr);
            
            std::string text = findText(sceneStr);
            scene.texts = parseText(text);

            std::string choices = findChoices(sceneStr);
            scene.choices = parseChoices(choices);

            m_currentChapter.push_back(scene);

            SDL_Log("%s", text.c_str());
        }
    }
}