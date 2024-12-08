#include "data_view.hpp"

#include "utility.hpp"

namespace MagiumSDL {
DataView::DataView() {
    m_sceneId = "Ch1-Intro1";
    addVarVal({
        .var = c_availablePointsVariable,
        .val = "0",
    });
    addVarVal({
        .var = c_availablePointsAuxVariable,
        .val = "0",
    });
    addVarVal({
        .var = c_maxStatVariable,
        .val = "3",
    });
    for (std::string s : c_statVariableNames) {
        addVarVal({
            .var = s,
            .val = "0",
        });
    }
}

void DataView::setupData(std::vector<std::string> achievements, DataSlot s) {
    m_variables = s.variables;
    m_bookIndex = s.bookIndex;
    m_fileIndex = s.fileIndex;
    m_chapterIndex = s.chapterIndex;
    m_sceneId = s.scene;
}

void DataView::nextChapter() {
    m_chapterIndex++;
    if (m_chapterIndex >= c_bookChapterCount[m_bookIndex]) {
        m_chapterIndex = 0;
        m_bookIndex++;
    }
}

std::string DataView::getChapterName() const {
    std::string name;
    if (m_bookIndex > 0) {
        name += "Book" + std::to_string(m_bookIndex + 1) + " -";
    }
    name += "Chapter " + std::to_string(m_chapterIndex + 1);
    return name;
}

DataSlot DataView::slot() {
    return {
        .name = "active",
        .variables = m_variables,
        .bookIndex = m_bookIndex,
        .fileIndex = m_fileIndex,
        .chapterIndex = m_chapterIndex,
        .scene = m_sceneId,
    };
}

bool DataView::exists(std::string var) {
    for (VarVal &v : m_variables) {
        if (v.var == var)
            return true;
    }
    return false;
}

std::string &DataView::getVal(std::string var) {
    for (VarVal &v : m_variables) {
        if (v.var == var)
            return v.val;
    }
    return nullstring;
}

void DataView::addVarVal(VarVal varVal) {
    if (exists(varVal.var)) {
        if (varVal.val.find("+") != varVal.val.npos) {
            std::string &val = getVal(varVal.var);
            val = std::to_string(std::stoi(val) + std::stoi(varVal.val));
        } else {
            getVal(varVal.var) = varVal.val;
        }
    } else {
        if (int pos = varVal.val.find("+") != varVal.val.npos)
            varVal.val.erase(pos, 1);
        m_variables.push_back(varVal);
    }
}

bool DataView::addAchievementExists(std::string achievement) {
    std::string all;
    for (std::string a : m_achievements) {
        all += a;
        if (a == achievement) {
            return true;
        }
    }
    m_achievements.push_back(achievement);
    return false;
}

bool DataView::evaluateVarVal(VarVal varVal, BoolOP op) {
    if (exists(varVal.var)) {
        std::string &val = getVal(varVal.var);
        switch (op) {
            case BoolOP::EQUAL: return val == varVal.val;
            case BoolOP::NOT_EQUAL: return val != varVal.val;
            case BoolOP::GREATER_EQUAL: return std::stoi(val) >= std::stoi(varVal.val);
            case BoolOP::LESSER_EQUAL: return std::stoi(val) <= std::stoi(varVal.val);
            case BoolOP::GREATER: return std::stoi(val) > std::stoi(varVal.val);
            case BoolOP::LESSER: return std::stoi(val) < std::stoi(varVal.val);
        }
    } else {
        return varVal.val == "0" && op == BoolOP::EQUAL;
    }
}

std::string DataView::evaluateText(std::vector<ConditionalText> &texts) {
    std::string grouped;
    for (ConditionalText t : texts) {
        if (evaluateCondition(t.condition)) {
            grouped += t.text;
        }
    }

    grouped.erase(0, grouped.find_first_not_of('\n'));
    grouped.erase(0, grouped.find_first_not_of(' '));
    grouped += "\n\n\n";

    return grouped;
}

std::vector<std::pair<std::string, bool>> DataView::evaluateSetCommands(std::vector<VariableSet> &set) {
    std::vector<std::pair<std::string, bool>> formatted;
    for (VariableSet &s : set) {
        std::string statCheck;
        if (evaluateCondition(s.condition, &statCheck)) {
            addVarVal(s.variable);
        }

		bool skip = false;
        while (!skip) {
            std::vector<size_t> possibleSigns = {
                statCheck.find("=="), statCheck.find("!="), statCheck.find(">="),
                statCheck.find("<="), statCheck.find(">"),  statCheck.find("<"),
            };
            size_t signPos = statCheck.length() + 1;
            BoolOP operation;
            for (int i = 0; i < possibleSigns.size(); i++) {
                if (possibleSigns[i] != statCheck.npos && possibleSigns[i] < signPos) {
                    signPos = possibleSigns[i];
                    operation = (BoolOP)i;
                }
            }
            if (signPos == statCheck.length() + 1)
                break;
            int signLength = operation == BoolOP::GREATER || operation == BoolOP::LESSER ? 1 : 2;

            VarVal varVal;

            std::string until = statCheck.substr(0, signPos);
            std::vector<size_t> possibleStarts = {
                0,
                findLastOf(until, "&&") == until.npos ? 0 : findLastOf(until, "&&") + 2,
            };
            int varStart = *std::max_element(possibleStarts.begin(), possibleStarts.end());
            varVal.var = statCheck.substr(varStart, signPos - varStart);

            std::vector<size_t> possibleEnds = {
                statCheck.length(),
                statCheck.find("&&", signPos),
            };
            int valEnd = *std::min_element(possibleEnds.begin(), possibleEnds.end());
            varVal.val = statCheck.substr(signPos + signLength, valEnd - signPos - signLength);

            bool evaluation = evaluateVarVal(varVal, operation);
            statCheck.replace(varStart, valEnd - varStart, std::to_string(evaluation));

			int value = std::stoi(varVal.val);

            if (operation == BoolOP::LESSER || (varVal.val == "0" && operation == BoolOP::EQUAL)) {
				skip = true;
				continue;
            }
			if (operation == BoolOP::GREATER) {
				value++;
			}

			if (!c_statVariableNameToStatName.contains(varVal.var)) {
				continue;
			}
            formatted.push_back({"[ " + c_statVariableNameToStatName.at(varVal.var) +
                                     (evaluation ? " check successful - level " : " check failed - level ") + std::to_string(value) + " ]\n",
                                 evaluation});
        }
    }
    return formatted;
}

std::vector<Choice> DataView::evaluateChoices(std::vector<Choice> &choices) {
    std::vector<Choice> applyableChoices;
    for (Choice c : choices) {
        if (evaluateCondition(c.condition)) {
            applyableChoices.push_back(c);
        }
    }
    return applyableChoices;
}

bool DataView::evaluateCondition(Condition condition, std::string *conditionPassed) {
    std::string cond = condition.condition;
    if (cond == "")
        return true;

    std::vector<std::string> expressions;
    size_t pos = 0;
    while (conditionPassed) {
        std::string expr = cond.substr(pos, cond.size() - pos);

        int end = expr.find("||");
        end = end == expr.npos ? expr.length() : end;

        expressions.push_back(expr.substr(0, end));

        pos += end + 2;
        if (pos >= expr.length()) {
            break;
        }
    }

    while (true) {
        std::vector<size_t> possibleSigns = {
            cond.find("=="), cond.find("!="), cond.find(">="), cond.find("<="), cond.find(">"), cond.find("<"),
        };
        size_t signPos = cond.length() + 1;
        BoolOP operation;
        for (int i = 0; i < possibleSigns.size(); i++) {
            if (possibleSigns[i] != cond.npos && possibleSigns[i] < signPos) {
                signPos = possibleSigns[i];
                operation = (BoolOP)i;
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

        bool evaluation = evaluateVarVal(varVal, operation);
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

    bool passed = cond.find("1") != cond.npos;
    if (conditionPassed) {
        int index = count(cond.substr(0, findLastOf(cond, "1")), '0') + count(cond.substr(0, findLastOf(cond, "1")), '1');
        index = passed ? index : expressions.size() - 1;
        *conditionPassed = expressions[index];
    }
    return passed;
}
} // namespace MagiumSDL
