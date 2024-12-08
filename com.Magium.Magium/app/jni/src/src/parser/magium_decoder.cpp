#include "magium_decoder.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_iostream.h>

#include <functional>
#include <memory>

#include "../helper/helper.hpp"
#include "../ui/image.hpp"
#include "../values.hpp"
#include "persister.hpp"

extern TTF_Font *g_fontMainText;
extern MagiumSDL::UIScene g_game;

namespace MagiumSDL {
MagiumDecoder::MagiumDecoder(std::shared_ptr<UIScene> scene, std::shared_ptr<RawText> text, std::shared_ptr<RawText> chapterCounterText)
    : m_uiscene{scene}, m_text{text}, m_chapterCounterText{chapterCounterText} {
    if (m_persister.startupData()) {
        m_view.setupData(m_persister.achievements(), m_persister.getSlot("active"));
    }

    m_parser.parse(c_fileNames[m_view.fileIndex()]);
    updateScene();

    m_persister.addSlot(m_view.slot());
    m_persister.updateAchievements(m_view.achievements());
    m_persister.save();
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

void MagiumDecoder::applyChoice(Choice choice) {
    SDL_Log("Next id: %s", choice.nextSceneID.c_str());
    m_uiscene->clear();

    int maxStatLevel = std::stoi(m_view.getVal(c_maxStatVariable));
    for (std::shared_ptr<MagiumStat> stat : m_uiStats) {
        stat->confirmStatLevel();
        stat->setMaxLevel(maxStatLevel);
        m_view.addVarVal({
            .var = c_statVariableNames[c_statNameToStatVariableNameIndex.at(stat->name())],
            .val = std::to_string(stat->level()),
        });
    }

    int availablePointsBefore = std::stoi(m_view.getVal(c_availablePointsVariable));
    m_view.setSceneId(choice.nextSceneID);
    for (VarVal &v : choice.variables)
        m_view.addVarVal(v);

    int availablePointsAfter = std::stoi(m_view.getVal(c_availablePointsVariable));
    if (availablePointsBefore != availablePointsAfter)
        *m_pAvailablePoints += availablePointsAfter - availablePointsBefore;

    if (choice.special.find(c_specialCheckpointSave) != choice.special.npos) {
        m_view.nextChapter();
        m_chapterCounterText->changeText(m_view.getChapterName());
    }

    if (choice.special.find(c_specialStats) != choice.special.npos) {
    }

    if (choice.special.find(c_specialAchievement) != choice.special.npos) {
        std::string achievementText;
        if (!m_view.addAchievementExists(parseAchievementSpecial(choice.special, achievementText))) {
            showAchievement(achievementText);
        }
    }

    if (choice.special.find(c_specialRestart) != choice.special.npos) {
        m_view = {};
        m_parser.parse(c_fileNames[m_view.fileIndex()]);
        m_chapterCounterText->changeText(m_view.getChapterName());
    }

    if (choice.special.find(c_specialSaves) != choice.special.npos) {
    }

    // berserk
    if (m_view.sceneId() == "Ch6-Eiden-vs-dragon" && m_view.getVal("v_maximized_stats_used") == "1") {
        m_berserkPointsProxy = *m_pAvailablePoints;
        const int berserkPointIncrease = 10;
        for (std::shared_ptr<MagiumStat> stat : m_uiStats) {
            stat->addLevel(berserkPointIncrease);
        }
        berserkMode(berserkPointIncrease);
    }

    updateScene();

    m_persister.addSlot(m_view.slot());
    m_persister.updateAchievements(m_view.achievements());
    m_persister.save();
}

void MagiumDecoder::updateScene() {
    auto [scene, found] = m_parser.getScene(m_view.sceneId());
    if (!found) {
        m_view.incrementFileIndex();
        m_parser.parse(c_fileNames[m_view.fileIndex()]);
        std::tie(scene, found) = m_parser.getScene({});
        m_view.setSceneId(scene.id);
    }

    auto statChecks = m_view.evaluateSetCommands(scene.varSets);
    int totalHeights = 0;
    for (std::pair<std::string, bool> &p : statChecks) {
        std::shared_ptr<RawText> statText =
            std::make_shared<RawText>(m_text->rect().x, totalHeights + 0.005f * g_height + m_text->rect().y, p.first, .74f,
                                      p.second ? COLOR_STAT_CHECK_PASSED : COLOR_STAT_CHECK_FAILED, g_fontMainText);
        m_uiscene->add(statText);
        totalHeights += statText->rect().h + 0.005f * g_height;
    }
	totalHeights += 0.02f * g_height;

    m_text->changeText(m_view.evaluateText(scene.texts));
    if (statChecks.size() > 0) {
        m_text->rect().y += totalHeights;
    }
    m_uiscene->add(m_text);

    std::vector<Choice> choices = m_view.evaluateChoices(scene.choices);
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

void MagiumDecoder::restart() {
    m_view = {};
    m_parser.parse(c_fileNames[m_view.fileIndex()]);
    m_chapterCounterText->changeText(m_view.getChapterName());
}

void MagiumDecoder::update() {
    if (m_clicked) {
        applyChoice(m_choiceTaken);
        m_clicked = false;
    }
}

void MagiumDecoder::berserkMode(int maxStat) {
    if (maxStat < 1) {
        return;
    }
    Helper::delayFunction(1.f, [this, maxStat]() {
        for (std::shared_ptr<MagiumStat> stat : m_uiStats) {
            stat->decrementLevel();
        }
        berserkMode(maxStat - 1);
    });
}
} // namespace MagiumSDL
