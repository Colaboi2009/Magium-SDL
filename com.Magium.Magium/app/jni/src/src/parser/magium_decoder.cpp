#include "magium_decoder.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_iostream.h>

#include <algorithm>
#include <functional>
#include <memory>

#include "../helper/helper.hpp"
#include "../ui/image.hpp"
#include "../values.hpp"
#include "persister.hpp"
#include "utility.hpp"

extern TTF_Font *g_fontMainText;
extern MagiumSDL::UIScene g_game;

namespace MagiumSDL {
MagiumDecoder::MagiumDecoder(std::shared_ptr<UIScene> scene, std::shared_ptr<RawText> text, std::shared_ptr<RawText> chapterCounterText)
    : m_uiscene{scene}, m_text{text}, m_chapterCounterText{chapterCounterText}, m_persister{}, m_data{} {
    if (m_persister.startupData()) {
        m_data.setupData(m_persister.achievements(), m_persister.getSlot("active"));
    }

    processAndStoreFile(c_fileNames[m_data.fileIndex()] + c_fileExtension);
    updateScene();

    m_persister.addSlot(m_data.slot());
    m_persister.updateAchievements(m_data.achievements());
    m_persister.save();
}

std::vector<std::string> MagiumDecoder::separateFiles(std::string &chapter) {
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

std::string MagiumDecoder::findID(std::string &scene) {
    int idPos = scene.find("ID:");
    int endPos = scene.find("\n");
    std::string id = scene.substr(idPos + 3, endPos - idPos - 3);
    return removeSpaces(id);
}

std::string MagiumDecoder::findText(std::string &scene) {
    int textPos = scene.find("TEXT:") + 5;
    int endPos = scene.find("choice(");
    std::string text = scene.substr(textPos, endPos - textPos);
    replaceString(text, "\n", ""); // note: im relying on '\r' to make new lines
    return text;
}

std::string MagiumDecoder::findChoices(std::string &scene) {
    int choiceStart = scene.find("choice(");
    return scene.substr(choiceStart);
}

std::vector<ConditionalText> MagiumDecoder::parseText(std::string text, std::vector<VariableSet> &sets) {
    std::vector<ConditionalText> texts;
    std::string textNoSpace = removeSpaces(text);

    int currentPos = 0;
    while (true) {
        if (textNoSpace.find("set(") != textNoSpace.npos) {
            int setStart = textNoSpace.find("(") + 1;
            int valStart = textNoSpace.find(",");
            int setEnd = textNoSpace.find(")");
            VarVal var{
                .var = textNoSpace.substr(setStart, valStart - setStart),
                .val = textNoSpace.substr(valStart + 1, setEnd - valStart - 1),
            };

            Condition cond{.condition = ""};
            int ifStart = textNoSpace.find("if(", setEnd);
            int ifEnd = setEnd;
            if (ifStart != textNoSpace.npos) {
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

std::vector<Choice> MagiumDecoder::parseChoices(std::string text) {
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

std::string MagiumDecoder::evaluateText(std::vector<ConditionalText> &texts) {
    std::string grouped;
    for (ConditionalText t : texts) {
        if (evaluateCondition(t.condition)) {
            grouped += t.text;
        }
    }

    grouped.erase(0, grouped.find_first_not_of('\r'));
    grouped.erase(0, grouped.find_first_not_of(' '));
    grouped += "\n\n\n";

    return grouped;
}

void MagiumDecoder::evaluateSetCommands(std::vector<VariableSet> &set) {
    for (VariableSet &s : set) {
        if (evaluateCondition(s.condition)) {
            m_data.addVarVal(s.variable);
        }
    }
}

std::vector<Choice> MagiumDecoder::evaluateChoices(std::vector<Choice> &choices) {
    std::vector<Choice> applyableChoices;
    for (Choice c : choices) {
        if (evaluateCondition(c.condition)) {
            applyableChoices.push_back(c);
        }
    }
    return applyableChoices;
}

bool MagiumDecoder::evaluateCondition(Condition condition) {
    std::string cond = condition.condition;
    if (cond == "")
        return true;

    while (true) {
        std::vector<size_t> possibleSigns = {
            cond.find("=="), cond.find("!="), cond.find(">="), cond.find("<="), cond.find(">"), cond.find("<"),
        };
        size_t signPos = cond.length() + 1;
        BoolOP operation;
        for (int i = 0; i < possibleSigns.size(); i++) {
            if (possibleSigns[i] != cond.npos) {
                if (possibleSigns[i] < signPos) {
                    signPos = possibleSigns[i];
                    operation =
                        (BoolOP)i; // this feels dodgy, its dependant on the order of the enums and order of the possibleSigns elements.
                }
            }
        }
        if (signPos == cond.length() + 1)
            break;
        int signLength = operation == BoolOP::GREATER || operation == BoolOP::LESSER ? 1 : 2;

        VarVal varVal;

        std::string until = cond.substr(0, signPos);
        std::vector<size_t> possibleStarts = {
            0,
            findLastOf(until, "&&") == until.npos ? 0 : findLastOf(until, "&&") + 2,
            findLastOf(until, "||") == until.npos ? 0 : findLastOf(until, "||") + 2,
        };
        int varStart = *std::max_element(possibleStarts.begin(), possibleStarts.end());
        varVal.var = cond.substr(varStart, signPos - varStart);

        std::vector<size_t> possibleEnds = {
            cond.length(),
            cond.find("&&", signPos),
            cond.find("||", signPos),
        };
        int valEnd = *std::min_element(possibleEnds.begin(), possibleEnds.end());
        varVal.val = cond.substr(signPos + signLength, valEnd - signPos - signLength);

        bool evaluation = m_data.evaluateVarVal(varVal, operation);
        cond.replace(varStart, valEnd - varStart, std::to_string(evaluation));
    }

    while (true) {
        int andPos = cond.find("&&");
        if (andPos == cond.npos)
            break;

        std::string until = cond.substr(0, andPos);
        int start = until.find("||") == until.npos ? 0 : findLastOf(until, "||") + 2;

        std::vector<size_t> possibleEnds = {
            cond.find("&&", andPos + 2),
            cond.find("||", andPos + 2),
            cond.length(),
        };
        int end = *std::min_element(possibleEnds.begin(), possibleEnds.end());

        std::string left = cond.substr(start, andPos - start);
        std::string right = cond.substr(andPos + 2, end - andPos - 2);
        bool evaluation = left == "1" && right == "1";

        cond.replace(start, end - start, std::to_string(evaluation));
    }

    return cond.find("1") != cond.npos;
}

std::string MagiumDecoder::parseAchievementSpecial(std::string special, std::string &text) {
    int firstDash = special.find("-") + 1;
    int secondDash = special.find("-", firstDash);
    text = special.substr(firstDash, secondDash - firstDash);
    return special.substr(secondDash + 1, special.length() - secondDash - 1);
}

void MagiumDecoder::showAchievement(std::string text) {
    std::shared_ptr<UIScene> achievementScene = std::make_shared<UIScene>();
    g_game.add(achievementScene);

    achievementScene->add(std::make_shared<Image>("achievement_box.png", Helper::swiths(RECT_ACHIEVEMENT_BOX), RECT_ACHIEVEMENT_BOX.w));
    achievementScene->add(
        std::make_shared<Image>("achievement_side_counter.png", Helper::swiths(RECT_ACHIEVEMENT_BOX), RECT_ACHIEVEMENT_BOX.w));
    achievementScene->add(
        std::make_shared<Image>("achievement_trophy.png", Helper::swiths(RECT_ACHIEVEMENT_TROPHY), RECT_ACHIEVEMENT_TROPHY.w));

    achievementScene->add(std::make_shared<RawText>(Helper::swiths(RECT_ACHIEVEMENT_TEXT).x, Helper::swiths(RECT_ACHIEVEMENT_TEXT).y, text,
                                                    RECT_ACHIEVEMENT_TEXT.h, COLOR_GAME_TEXT_STORY_TEXT, g_fontMainText,
                                                    RECT_ACHIEVEMENT_TEXT.w));

    Helper::delayFunction(5.f, [achievementScene]() { g_game.remove(achievementScene); });
}

void MagiumDecoder::processAndStoreFile(std::string filename) {
    std::string filepath = c_magiumFormatFilepath + filename;
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

void MagiumDecoder::applyChoice(Choice choice) {
    SDL_Log("Next id: %s", choice.nextSceneID.c_str());
    m_uiscene->clear();

    //{ // LOGGING
    //    std::string total;
    //    for (VarVal v : m_data.getAll()) {
    //        total += "{\"" + v.var + "\",\"" + v.val + "\"},";
    //    }
    //	SDL_Log("%s", total.c_str());
    //}

    int maxStatLevel = std::stoi(m_data.getVal(c_maxStatVariable));
    for (std::shared_ptr<MagiumStat> stat : m_uiStats) {
        stat->confirmStatLevel();
        stat->setMaxLevel(maxStatLevel);
        m_data.addVarVal({
            .var = c_statVariableNames[c_statNameToStatVariableNameIndex.at(stat->name())],
            .val = std::to_string(stat->level()),
        });
    }

    int availablePointsBefore = std::stoi(m_data.getVal(c_availablePointsVariable));
    m_data.setSceneId(choice.nextSceneID);
    for (VarVal &v : choice.variables)
        m_data.addVarVal(v);

    int availablePointsAfter = std::stoi(m_data.getVal(c_availablePointsVariable));
    if (availablePointsBefore != availablePointsAfter)
        *m_pAvailablePoints += availablePointsAfter - availablePointsBefore;

    if (choice.special.find(c_specialCheckpointSave) != choice.special.npos) {
        m_data.incrementChapterNumber();
        if (m_data.chapterNumber() > c_bookChapterCount[m_data.bookNumber() - 1]) {
            m_data.resetChapterNumber();
            m_data.incrementBookNumber();
        }
        if (m_data.bookNumber() == 1) {
            m_chapterCounterText->changeText("Chapter " + std::to_string(m_data.chapterNumber()));
        } else {
            m_chapterCounterText->changeText("Book " + std::to_string(m_data.bookNumber()) + " -Chapter " +
                                             std::to_string(m_data.chapterNumber()));
        }
    }

    if (choice.special.find(c_specialStats) != choice.special.npos) {
    }

    if (choice.special.find(c_specialAchievement) != choice.special.npos) {
        std::string achievementText;
        if (!m_data.addAchievementExists(parseAchievementSpecial(choice.special, achievementText))) {
            showAchievement(achievementText);
        }
    }

    if (choice.special.find(c_specialRestart) != choice.special.npos) {
        m_data = {};
		processAndStoreFile(c_fileNames[m_data.fileIndex()] + c_fileExtension);
        if (m_data.bookNumber() == 1) {
            m_chapterCounterText->changeText("Chapter " + std::to_string(m_data.chapterNumber()));
        } else {
            m_chapterCounterText->changeText("Book " + std::to_string(m_data.bookNumber()) + " -Chapter " +
                                             std::to_string(m_data.chapterNumber()));
        }
    }

    if (choice.special.find(c_specialSaves) != choice.special.npos) {
    }

    updateScene();

    m_persister.addSlot(m_data.slot());
    m_persister.updateAchievements(m_data.achievements());
    m_persister.save();
}

void MagiumDecoder::updateScene() {
    MagiumScene scene = {};
    for (MagiumScene &ms : m_currentChapter) {
        if (ms.id == m_data.sceneId()) {
            scene = ms;
        }
    }
    if (scene.texts.size() == 0) {
        m_data.incrementFileIndex();
        processAndStoreFile(c_fileNames[m_data.fileIndex()] + c_fileExtension);
        scene = m_currentChapter[0];
        m_data.setSceneId(scene.id);
    }

    evaluateSetCommands(scene.varSets);

    m_text->changeText(evaluateText(scene.texts));
    m_uiscene->add(m_text);

    std::vector<Choice> choices = evaluateChoices(scene.choices);
    for (int i = 0; i < choices.size(); i++) {
        SDL_FRect r = {g_width / 2.f - g_width * STORY_BUTTON_WIDTH / 2.f,
                       m_text->rect().h + m_text->rect().y + g_height * STORY_BUTTON_SPACING * i, g_width * STORY_BUTTON_WIDTH,
                       g_height * STORY_BUTTON_HEIGHT};
        Choice *choiceTaken = &m_choiceTaken;
        bool *clicked = &m_clicked;
        Choice associatedChoice = choices[i];
        std::shared_ptr<Button> b = std::make_shared<Button>(r, COLOR_GAME_TEXT_OPTION_BUTTON, COLOR_GAME_TEXT_STORY_TEXT, choices[i].text,
                                                             STORY_TEXT_SIZE, [associatedChoice, choiceTaken, clicked]() {
                                                                 *clicked = true;
                                                                 *choiceTaken = associatedChoice;
                                                             });
        m_uiscene->add(b);
    }
    SDL_FRect finalRect = m_uiscene->get(m_uiscene->getAll().size() - 1).rect();
    std::shared_ptr<RawImage> padding =
        std::make_shared<RawImage>(SDL_FRect{finalRect.x, finalRect.y + g_width * STORY_BUTTON_BOTTOM_PADDING}, SDL_Color{0, 0, 0, 0});
    m_uiscene->add(padding);
}

void MagiumDecoder::update() {
    if (m_clicked) {
        applyChoice(m_choiceTaken);
        m_clicked = false;
    }
}

} // namespace MagiumSDL
