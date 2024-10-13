#pragma once

#include <algorithm>
#include <string>
#include "magium_structs.hpp"

namespace MagiumSDL {
inline std::string removeSpaces(std::string s) {
    s.erase(std::remove_if(s.begin(), s.end(), isspace), s.end());
    return s;
}

inline void replaceString(std::string &str, const std::string &from, const std::string &to) {
    size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::string::npos) {
        str.replace(pos, from.length(), to);
        pos += to.length();
    }
}

inline std::string toLower(std::string s) {
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
	return s;
}

inline size_t findLastOf(std::string &s, std::string toFind) {
    size_t pos = 0;
    while (true) {
        size_t nextPos = s.find(toFind, pos);
        pos = s.find(toFind, nextPos + 1);
        if (pos == s.npos)
            return nextPos;
    }
    return s.npos;
}

} // namespace MagiumSDL
