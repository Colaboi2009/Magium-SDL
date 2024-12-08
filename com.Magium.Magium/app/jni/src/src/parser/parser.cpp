#include "parser.hpp"

#include "utility.hpp"

namespace MagiumSDL {

Parser::Parser() {}

Parser::~Parser() {}

std::vector<std::string> Parser::separateFiles(std::string &chapter) {
    std::vector<std::string> separated;
    int currentIdPos = chapter.find("ID:");
    while (currentIdPos != chapter.npos) {
        int nextIdPos = chapter.find("ID:", currentIdPos + 3);
        if (nextIdPos == chapter.npos)
            nextIdPos = chapter.length() - 1;
        separated.push_back(chapter.substr(currentIdPos, nextIdPos - currentIdPos));
        currentIdPos = chapter.find("ID:", currentIdPos + 3);
    }
    return separated;
}

std::string Parser::findID(std::string &scene) {
    int idPos = scene.find("ID:");
    int endPos = scene.find("\n");
    std::string id = scene.substr(idPos + 3, endPos - idPos - 3);
    return removeSpaces(id);
}

std::string Parser::findText(std::string &scene) {
    int textPos = scene.find("TEXT:") + 5;
    int endPos = scene.find("choice(");
    std::string text = scene.substr(textPos, endPos - textPos);
    return text;
}

std::string Parser::findChoices(std::string &scene) {
    int choiceStart = scene.find("choice(");
    return scene.substr(choiceStart);
}

std::vector<ConditionalText> Parser::parseText(std::string text, std::vector<VariableSet> &sets) {
    std::vector<ConditionalText> texts;
    std::string textNoSpace = removeSpaces(text);

    int currentPos = 0;
    while (true) {
		bool output = false;
        if (textNoSpace.find("set(") != textNoSpace.npos) {
			output = true;
            int setStart = textNoSpace.find("(") + 1;
            int valStart = textNoSpace.find(",");
            int setEnd = textNoSpace.find(")");
            VarVal var{
                .var = textNoSpace.substr(setStart, valStart - setStart),
                .val = textNoSpace.substr(valStart + 1, setEnd - valStart - 1),
            };

            Condition cond{.condition = ""};
			int hashIfStart = textNoSpace.find("#if", setEnd);
            int ifStart = textNoSpace.find("if(", setEnd);
            int ifEnd = setEnd;
            if (ifStart != textNoSpace.npos && ifStart < hashIfStart) {
                ifStart += 3;
                ifEnd = textNoSpace.find(")", ifStart);
                cond.condition = textNoSpace.substr(ifStart, ifEnd - ifStart);
            }


            VariableSet set;
            set.variable = var;
            set.condition = cond;
            sets.push_back(set);

            textNoSpace.erase(0, ifEnd + 1);
            continue;
        }

        bool conditioned = textNoSpace[0] == '#';
        if (conditioned) {
            ConditionalText conditionText;

            int conditionStart = textNoSpace.find("#if(") + 4;
            int conditionEnd = textNoSpace.find(")");
            conditionText.condition.condition = textNoSpace.substr(conditionStart, conditionEnd - conditionStart); // CONDITION TODO

            int startPos = text.find("{", currentPos) + 1;
            int endPos = text.find("}", currentPos);
            conditionText.text = text.substr(startPos, endPos - startPos);

            texts.push_back(conditionText);

            textNoSpace.erase(0, textNoSpace.find("}") + 1);
            currentPos = endPos + 1;
        } else {
            int endPos = text.find("#", currentPos);
            if (endPos == text.npos) {
                endPos = text.length() - 1;
            }

            ConditionalText conditionText;
            conditionText.condition.condition = "";
            conditionText.text = text.substr(currentPos, endPos - currentPos);

            texts.push_back(conditionText);

            if (endPos == text.length() - 1 || textNoSpace.length() <= 1)
                break;

            textNoSpace.erase(0, textNoSpace.find("#"));
            currentPos = endPos;
        }
    }
    return texts;
}

std::vector<Choice> Parser::parseChoices(std::string text) {
    std::vector<Choice> choices;
    std::vector<std::string> choicesStrings;
    int curPos = 0;
    while (true) {
        int start = text.find("choice(", curPos);
        if (start == text.npos)
            break;

        int endPos = text.find(")", text.find(",", start)) + 1;

        int possibleIf = text.find("if", endPos);
        if (possibleIf != text.npos && possibleIf - endPos < 8) { // less than 'choice(' length for safety
            endPos = text.find(")", possibleIf + 4) + 1;
        }
        choicesStrings.push_back(text.substr(start, endPos - start));
        curPos = endPos;
    }

    for (std::string s : choicesStrings) {
        Choice choice;

        int textStart = s.find("\"\"");
        int textEnd = s.find("\"", textStart + 2);
        if (textStart == s.npos) {
            textStart = s.find("\"");
            textEnd = s.find("\"", textStart + 1);
            choice.text = s.substr(textStart + 1, textEnd - textStart - 1);
        } else {
            choice.text = s.substr(textStart + 2, textEnd - textStart - 2);
        }

        std::string spaceless = removeSpaces(s);

        int conditionStart = spaceless.find("if(");
        if (conditionStart != spaceless.npos) {
            conditionStart += 3;
            int conditionEnd = spaceless.find(")", conditionStart);
            choice.condition.condition = spaceless.substr(conditionStart, conditionEnd - conditionStart);
        }

        int choiceEndPos = spaceless.length() - 1;
        if (conditionStart != spaceless.npos)
            choiceEndPos = conditionStart - 4;

        int idStart = spaceless.find(c_nextSceneIdVariable) + c_nextSceneIdVariable.length() + 1;
        int idEnd = spaceless.find(",", idStart);
        if (idEnd == spaceless.npos)
            idEnd = choiceEndPos;
        choice.nextSceneID = spaceless.substr(idStart, idEnd - idStart);

        int specialStart = s.find("special:");
        if (specialStart != s.npos) {
            specialStart += 8;
            int specialEnd = s.find(")", s.find(","));
            choice.special = s.substr(specialStart, specialEnd - specialStart);
        }

        int curPos = 0;
        while (true) {
            VarVal var;

            curPos = spaceless.find("v_", curPos + 2);
            if (curPos == spaceless.npos                            // no more
                || curPos > choiceEndPos                            // too far
                || (choice.special != "" && curPos > specialStart)) // part of achievement
                break;

            int varEnd = spaceless.find("=", curPos);
            var.var = spaceless.substr(curPos, varEnd - curPos);

            int valEnd = spaceless.find(",", varEnd);
            if (valEnd == spaceless.npos)
                valEnd = choiceEndPos;
            var.val = spaceless.substr(varEnd + 1, valEnd - varEnd - 1);

            choice.variables.push_back(var);
        }

        choices.push_back(choice);
    }

    return choices;
}

void Parser::parse(std::string filename) {
    std::string filepath = c_magiumFormatFilepath + filename + c_fileExtension;
    std::string fileContents = readfile(filepath);

    std::vector<std::string> scenes = separateFiles(fileContents);

    m_currentChapter.clear();
    for (int i = 0; i < scenes.size(); i++) {
        std::string sceneStr = scenes[i];

        MagiumScene scene;
        scene.id = findID(sceneStr);

        std::string text = findText(sceneStr);
        scene.texts = parseText(text, scene.varSets);

        std::string choices = findChoices(sceneStr);
        scene.choices = parseChoices(choices);

        m_currentChapter.push_back(scene);
    }
}

std::tuple<MagiumScene, bool> Parser::getScene(std::string name) {
    for (MagiumScene &s : m_currentChapter) {
        if (s.id == name) {
            return std::make_tuple(s, true);
        }
    }
    return std::make_tuple(m_currentChapter[0], false);
}
} // namespace MagiumSDL
