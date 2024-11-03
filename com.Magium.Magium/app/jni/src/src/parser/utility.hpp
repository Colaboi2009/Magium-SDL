#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_iostream.h>
#include <algorithm>
#include <string>

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
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
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

inline std::string readfile(std::string filepath) {
    SDL_IOStream *stream = SDL_IOFromFile(filepath.c_str(), "r");

    size_t fileSize = SDL_GetIOSize(stream);
    char *buffer = (char *)SDL_malloc(fileSize + 1);
    SDL_ReadIO(stream, buffer, fileSize);
    buffer[fileSize] = '\0';

    SDL_free(buffer);
    SDL_CloseIO(stream);

    return buffer;
}

inline int count(std::string str, char k) {
    int count = 0;
    for (char &c : str) {
        if (c == k)
            count++;
    }
    return count;
}

inline void writefile(std::string filepath, std::string content) {
    SDL_IOStream *stream = SDL_IOFromFile(filepath.c_str(), "w");

    // char *buffer = (char *)SDL_malloc(content.size());
    SDL_WriteIO(stream, content.c_str(), content.size());

    // SDL_free(buffer);
    SDL_CloseIO(stream);
}
} // namespace MagiumSDL
