#pragma once

#include <string>
#include <vector>
#include <tuple>

#include "magium_structs.hpp"

namespace MagiumSDL {
class Parser {
  private:
    std::vector<MagiumScene> m_currentChapter;

    std::vector<std::string> separateFiles(std::string &chapter);

    std::string findID(std::string &scene);
    std::string findText(std::string &scene);
    std::string findChoices(std::string &scene);

    std::vector<ConditionalText> parseText(std::string text, std::vector<VariableSet> &sets);
    std::vector<Choice> parseChoices(std::string text);

  public:
	Parser();
	~Parser();

	void parse(std::string filepath);
	std::tuple<MagiumScene, bool> getScene(std::string name);
};
} // namespace MagiumSDL
