#include "persister.hpp"
#include "utility.hpp"

namespace MagiumSDL {
Persister::Persister() { m_startupData = load(); }

Persister::~Persister() {}

void Persister::loadAchievements(std::string &file) {
    int start = file.find(":") + 1;
    std::string achStr = file.substr(start, file.find("\n") - start);

    m_achievements.clear();
    int pos = 0;
    while (pos < achStr.length()) {
        int nextPos = achStr.find(",", pos);
        nextPos = nextPos == achStr.npos ? achStr.length() : nextPos;
        m_achievements.push_back(achStr.substr(pos, nextPos - pos));
        pos = nextPos + 1;
    }
}

DataSlot Persister::loadSlot(std::string key, std::string &file) {
    int start = file.find(key + ":") + key.length();
    std::string slotStr = file.substr(start, file.find("\n", start) - start);

	SDL_Log("Slot data: %s", slotStr.c_str());

    std::vector<VarVal> variables;
    int pos = slotStr.find("v#") + 2;
	int varCount = count(slotStr.substr(pos), '=');
    for(int i = 0; i < varCount; i++) {
        int nextPos = slotStr.find(",", pos);
        int eqPos = slotStr.find("=", pos);
        std::string var = slotStr.substr(pos, eqPos - pos);
        std::string val = slotStr.substr(eqPos + 1, nextPos - eqPos - 1);
        variables.push_back({var, val});
        pos = nextPos + 1;
    }

    int biStart = slotStr.find("b#") + 2;
    int bookIndex = std::stoi(slotStr.substr(biStart, slotStr.find(";", biStart) - biStart));

    int fiStart = slotStr.find("f#") + 2;
    int fileIndex = std::stoi(slotStr.substr(fiStart, slotStr.find(";", fiStart) - fiStart));

    int ciStart = slotStr.find("c#") + 2;
    int chapterIndex = std::stoi(slotStr.substr(ciStart, slotStr.find(";", ciStart) - ciStart));

    int siStart = slotStr.find("s#") + 2;
    std::string scene = slotStr.substr(siStart, slotStr.find(";", siStart) - siStart);

	std::string variableStr = "";
	for (VarVal &s : variables) {
		variableStr += s.var + '=' + s.val + ';';
	}
    SDL_Log("Slot info-> n:%s, b:%i, f:%i, c:%i, s:%s.\nv:%s", key.c_str(), bookIndex, fileIndex, chapterIndex, scene.c_str(), variableStr.c_str());

    return {
        .name = key,
        .variables = variables,
        .bookIndex = bookIndex,
        .fileIndex = fileIndex,
        .chapterIndex = chapterIndex,
        .scene = scene,
    };
}

bool Persister::load() {
    std::string file = readfile("persistent_data.txt");
    if (count(file, '\n') <= 1) {
        return false;
    }

    loadAchievements(file);

    int saves = count(file, '\n') - 1;
    SDL_Log("%i", saves);
    int pos = file.find('\n') + 1;
    for (int i = 0; i < saves; i++) {
        int keyEnd = file.find(':', pos);
        m_slots.push_back(loadSlot(file.substr(pos, keyEnd - pos), file));
        pos = file.find('\n') + 1;
    }

    return true;
}

std::string Persister::saveAchievements() {
    std::string achStr;
    for (std::string &s : m_achievements) {
        achStr += s + ",";
    }
    return achStr;
}

std::string Persister::saveSlot(DataSlot slot) {
    std::string slotStr = slot.name + ":";
    slotStr += "b#" + std::to_string(slot.bookIndex) + ';';
    slotStr += "f#" + std::to_string(slot.fileIndex) + ';';
    slotStr += "c#" + std::to_string(slot.chapterIndex) + ';';
    slotStr += "s#" + slot.scene + ';';
    slotStr += "v#";
    for (VarVal &v : slot.variables) {
        slotStr += v.var + '=' + v.val + ',';
    }
    return slotStr;
}

void Persister::save() {
    std::string newData;
    newData += saveAchievements() + '\n';
    for (DataSlot s : m_slots) {
        newData += saveSlot(s) + '\n';
    }
    writefile("persistent_data.txt", newData);
}

DataSlot Persister::getSlot(std::string key) {
    for (DataSlot &s : m_slots) {
        if (s.name == key)
            return s;
    }
    return {};
}

void Persister::addSlot(DataSlot slot) {
    for (DataSlot &s : m_slots) {
        if (s.name == slot.name) {
            s = slot;
            return;
        }
    }
    m_slots.push_back(slot);
}

void Persister::wipeData() {
	writefile("persistent_data.txt", "");
    SDL_Log("Wiped data!");
}
} // namespace MagiumSDL
