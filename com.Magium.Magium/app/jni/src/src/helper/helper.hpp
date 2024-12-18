#pragma once

#include <SDL3/SDL.h>
#include <vector>

#include "stopwatch.hpp"

extern float g_width;
extern float g_height;
// TODO(Ala): Cleanup, and make a better system for delayed functions.
extern std::vector<MagiumSDL::Helper::DelayedFunction> g_delayedFunctions;

namespace MagiumSDL {
namespace Helper {
inline void delayFunction(float s, std::function<void(void)> func) { g_delayedFunctions.push_back(DelayedFunction(s, func)); }

inline SDL_FRect centerRect(SDL_FRect r) { return {r.x - r.w / 2.f, r.y - r.h / 2.f, r.w, r.h}; }

inline SDL_FRect centerRectX(SDL_FRect r) { return {r.x - r.w / 2.f, r.y, r.w, r.h}; }

inline SDL_FRect alignRight(SDL_FRect r) { return {r.x - r.w, r.y, r.w, r.h}; }

// scale with screen
inline SDL_FRect swiths(SDL_FRect r) { return {r.x * g_width, r.y * g_height, r.w * g_width, r.h * g_height}; }

// scale with screen
inline SDL_FRect swiths(float x, float y, float w, float h) { return {x * g_width, y * g_height, w * g_width, h * g_height}; }

} // namespace Helper
inline bool operator==(const SDL_Color &l, const SDL_Color &r) { return l.r == r.r && l.g == r.g && l.b == r.b && l.a == r.a; }
inline bool operator!=(const SDL_Color &l, const SDL_Color &r) { return !(l == r); }
} // namespace MagiumSDL
