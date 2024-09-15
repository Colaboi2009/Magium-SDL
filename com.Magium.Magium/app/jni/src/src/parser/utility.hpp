#pragma once

#include <string>
#include <algorithm>

namespace MagiumSDL {
    std::string removeSpaces(std::string s) 
    {
        s.erase(std::remove_if(s.begin(), s.end(), isspace), s.end());
        return s;
    }

    void replaceString(std::string &str, const std::string &from, const std::string &to) 
    {
        size_t pos = 0;
        while((pos = str.find(from, pos)) != std::string::npos) {
            str.replace(pos, from.length(), to);
            pos += to.length();
        }
    }

    size_t findLastOf(std::string &s, std::string toFind) 
    {
        size_t pos = 0;
        while (true) {
            size_t nextPos = s.find(toFind, pos);
            pos = s.find(toFind, nextPos + 1);
            if (pos == s.npos)
                return nextPos;
        }
        return s.npos;
    }
}