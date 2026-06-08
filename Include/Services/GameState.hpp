#ifndef BURGERTIME_GAMESTATE_HPP
#define BURGERTIME_GAMESTATE_HPP

#include <cstdint>

namespace bt
{

enum class GameMode : std::uint8_t
{
    Singleplayer,
    Coop,
    Versus
};

class GameState final
{
public:
    void StartGame();
    void SetGameMode(GameMode mode);
    auto GetGameMode() const -> GameMode;

private:
    static constexpr int maxLives{ 5 };

    bool m_gameStarted{ false };
    GameMode m_gameMode{};

};

}  // namespace bt

#endif  // BURGERTIME_GAMESTATE_HPP
