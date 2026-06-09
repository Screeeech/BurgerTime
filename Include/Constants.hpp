#ifndef BURGERTIME_COLORS_HPP
#define BURGERTIME_COLORS_HPP
#include <SDL3/SDL_pixels.h>

namespace bt
{

namespace colors
{

inline constexpr SDL_Color PlatformColor{ .r = 255, .g = 154, .b = 57, .a = 255 };
inline constexpr SDL_Color BlueLadderColor{ .r = 52, .g = 173, .b = 232, .a = 255 };
inline constexpr SDL_Color GreenLadderColor{ .r = 67, .g = 202, .b = 67, .a = 255 };

inline constexpr SDL_Color Red{ .r = 255, .g = 0, .b = 0, .a = 255 };
inline constexpr SDL_Color Green{ .r = 0, .g = 255, .b = 0, .a = 255 };
inline constexpr SDL_Color Blue{ .r = 0, .g = 0, .b = 255, .a = 255 };
inline constexpr SDL_Color Black{ .r = 0, .g = 0, .b = 0, .a = 255 };
inline constexpr SDL_Color Grey{ .r = 50, .g = 50, .b = 50, .a = 255 };

}  // namespace colors

namespace layers
{

inline constexpr int stage{ 0 };
inline constexpr int burgerParts{ 1 };
inline constexpr int enemies{ 2 };
inline constexpr int player{ 3 };
inline constexpr int pepper{ 4 };
inline constexpr int text{ 10 };

;
}  // namespace layers

namespace score
{
inline constexpr int partDrop{ 50 };
inline constexpr int hotdog{ 100 };
inline constexpr int pickle{ 200 };
inline constexpr int egg{ 300 };
inline constexpr int enemyDropScoreMultiplier{ 500 };
inline constexpr int enemyDropScoreCap{ 6 };

}

}  // namespace bt

#endif  // BURGERTIME_COLORS_HPP
