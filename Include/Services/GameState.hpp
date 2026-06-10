#ifndef BURGERTIME_GAMESTATE_HPP
#define BURGERTIME_GAMESTATE_HPP

#include <any>
#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>

#include "Component.hpp"


namespace gla
{
class Timer;
}
namespace bt
{
struct Initials
{
    char first;
    char last;

    auto operator==(const Initials& other) const -> bool { return first == other.first && last == other.last; }
};
}  // namespace bt

template<>
struct std::hash<bt::Initials>
{
    auto operator()(bt::Initials const& init) const noexcept -> std::size_t
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

class GameState final : public gla::Component
{
    static constexpr int maxStageCount{ 3 };
    static constexpr int maxLives{ 5 };
    static constexpr int initialPepper{ 5 };
    static constexpr std::string highScoreFile{ "highscores.json" };
    static constexpr float stageChangeDelay{ 5.f };
    static constexpr float loadingTime{ 3.f };

public:
    explicit GameState(gla::GameObject* pOwner);

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
    Initials currentInitials{ .first = 'P', .last = '1' };

protected:
    void OnActivate() override;
    void OnDeactivate() override;

private:
    void OnStageComplete(std::any const& eventArgs);
    void OnDeath(std::any const& eventArgs);
    void Respawn();
    void NextStage();

    void OnPlayerConnect(std::any const& connectEvent);
    void OnPlayerDisconnect(std::any const& connectEvent);

    void LoadHighScoreData();
    void SaveHighScoreData();
    std::unordered_map<Initials, int> m_highScores;

    bool m_gameStarted{ false };
    GameMode m_gameMode{};
    gla::GameObject* m_pStageObject;
    int m_stageIndex{};
    gla::Timer* m_pTimer;
};

}  // namespace bt

#endif  // BURGERTIME_GAMESTATE_HPP
