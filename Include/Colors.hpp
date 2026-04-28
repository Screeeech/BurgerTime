#ifndef BURGERTIME_COLORS_HPP
#define BURGERTIME_COLORS_HPP
#include <SDL3/SDL_pixels.h>

namespace bt::colors
{

inline constexpr SDL_Color PlatformColor{ .r = 255, .g = 154, .b = 57, .a = 255 };
inline constexpr SDL_Color BlueLadderColor{ .r = 52, .g = 173, .b = 232, .a = 255 };
inline constexpr SDL_Color GreenLadderColor{ .r = 67, .g = 202, .b = 67, .a = 255 };

inline constexpr SDL_Color Red{ .r=255, .g=0, .b=0, .a=255 };

}  // namespace bt::colors

#endif  // BURGERTIME_COLORS_HPP
