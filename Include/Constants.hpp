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
inline constexpr SDL_Color LoadingTextColor{ .r = 100, .g = 176, .b = 255, .a = 255 };

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
inline constexpr int bonus{ 3 };
inline constexpr int player{ 4 };
inline constexpr int pepper{ 5 };
inline constexpr int text{ 10 };
}  // namespace layers


namespace score
{
inline constexpr int partDrop{ 50 };
inline constexpr int hotdog{ 100 };
inline constexpr int pickle{ 200 };
inline constexpr int egg{ 300 };
inline constexpr int enemyDropScoreMultiplier{ 500 };
inline constexpr int enemyDropScoreCap{ 6 };
}  // namespace score

namespace game
{
inline constexpr int startingLives{ 5 };
inline constexpr int startingPepper{ 4 };
inline constexpr int stageCount{ 3 };
inline constexpr auto highScoreFile{ "highscores.json" };

inline constexpr float loadingTime{ 1.f };
inline constexpr float stageEndDelay{ 5.f };
inline constexpr float stageBeginDelay{ 3.f };
inline constexpr float enemySpawnDelay{ 4.f };

inline constexpr float pepperAttackDuration{ 1.f };
inline constexpr float pepperAttackCooldown{ 1.25f };
inline constexpr float stunDuration{ 3.f };
inline constexpr float bonusAppearTime{ 5.f };

inline constexpr float burgerPartFallingSpeed{ 60.f };
inline constexpr glm::vec2 playerVelocity{ 0.65f, 0.45f };
inline constexpr glm::vec2 enemyVelocity{ 0.4f, 0.3f };



}  // namespace game

}  // namespace bt

#endif  // BURGERTIME_COLORS_HPP
