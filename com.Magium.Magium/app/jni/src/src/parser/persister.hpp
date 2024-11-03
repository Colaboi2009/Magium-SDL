#pragma once

#include "magium_structs.hpp"
#include <SDL3/SDL.h>

namespace MagiumSDL {
class Persister {
  private:
    std::vector<std::string> m_achievements;
    std::vector<DataSlot> m_slots;

	bool m_startupData;

    void loadAchievements(std::string &file);
    DataSlot loadSlot(std::string key, std::string &file);

    std::string saveAchievements();
    std::string saveSlot(DataSlot slot);

  public:
    Persister();
    ~Persister();

    bool load();
    void save();
	std::vector<std::string> achievements() const { return m_achievements; }
	void updateAchievements(std::vector<std::string> a) { m_achievements = a; }
    DataSlot getSlot(std::string key);
	void addSlot(DataSlot slot);
	bool startupData() const { return m_startupData; }

	void wipeData();
};
} // namespace MagiumSDL
