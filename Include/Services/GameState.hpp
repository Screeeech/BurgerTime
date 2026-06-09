#ifndef BURGERTIME_GAMESTATE_HPP
#define BURGERTIME_GAMESTATE_HPP

#include <any>
#include <cstdint>
#include <optional>

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
    explicit GameState();

    void StartGame();
    void SetGameMode(GameMode mode);
    auto GetGameMode() const -> GameMode;

    std::optional<int> peterPepperPlayerIndex;
    std::optional<int> sallySaltPlayerIndex;
    std::optional<int> enemyPlayerIndex;
private:
    void OnPlayerConnect(std::any const& connectEvent);
    void OnPlayerDisconnect(std::any const& connectEvent);

    static constexpr int maxLives{ 5 };

    bool m_gameStarted{ false };
    GameMode m_gameMode{};

};

}  // namespace bt

#endif  // BURGERTIME_GAMESTATE_HPP
