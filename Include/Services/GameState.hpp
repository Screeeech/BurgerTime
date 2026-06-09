#ifndef BURGERTIME_GAMESTATE_HPP
#define BURGERTIME_GAMESTATE_HPP

#include <any>
#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>


namespace bt
{
struct Initials
{
    char first;
    char last;

    bool operator==(const Initials& other) const { return first == other.first && last == other.last; }
};
}  // namespace bt

template<>
struct std::hash<bt::Initials>
{
    std::size_t operator()(bt::Initials const& init) const noexcept
    {
        return (static_cast<std::size_t>(init.first) << 8) | static_cast<std::size_t>(init.last);
    }
};

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
    static constexpr int maxLives{ 5 };
    static constexpr int initialPepper{ 5 };
    static constexpr std::string highScoreFile{ "highscores.json" };

public:
    explicit GameState();

    void StartGame();
    void BeginRound() const;
    void EndGame();
    void SetGameMode(GameMode mode);
    auto GetGameMode() const -> GameMode;

    auto GetHealth() const -> int;

    std::optional<int> peterPepperPlayerIndex;
    std::optional<int> sallySaltPlayerIndex;
    std::optional<int> enemyPlayerIndex;
    int pepper{ initialPepper };
    int health{ maxLives };
    int score{};
    int highScore{};
    Initials currentInitials{ 'P', '1' };
private:
    void OnRespawn(std::any const& respawnEvent);

    void OnPlayerConnect(std::any const& connectEvent);
    void OnPlayerDisconnect(std::any const& connectEvent);

    void LoadHighScoreData();
    void SaveHighScoreData();
    std::unordered_map<Initials, int> m_highScores;

    bool m_gameStarted{ false };
    GameMode m_gameMode{};

};

}  // namespace bt

#endif  // BURGERTIME_GAMESTATE_HPP
