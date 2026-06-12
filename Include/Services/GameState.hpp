#ifndef BURGERTIME_GAMESTATE_HPP
#define BURGERTIME_GAMESTATE_HPP

#include <any>
#include <cstdint>
#include <glm/vec2.hpp>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>

#include "Component.hpp"
#include "Components/Entity.hpp"
#include "Constants.hpp"


namespace gla
{
class Timer;
}
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

public:
    explicit GameState(gla::GameObject* pOwner);

    void StartGame();
    void BeginRound() const;
    void EndGame();

    void SetGameMode(GameMode mode);
    [[nodiscard]] auto GetGameMode() const -> GameMode;
    [[nodiscard]] auto GetHealth() const -> int;
    [[nodiscard]] auto GetPepper() const -> int;
    [[nodiscard]] auto GetScore() const -> int;
    [[nodiscard]] auto GetSpawnPositions() const -> std::pair<glm::vec2, glm::vec2>;
    [[nodiscard]] auto GetEnemyCounts() const -> std::unordered_map<Entity::Type, int>;

    std::optional<int> peterPepperPlayerIndex;
    std::optional<int> sallySaltPlayerIndex;
    std::optional<int> enemyPlayerIndex;

    int highScore{};
protected:
    void OnActivate() override;
    void OnDeactivate() override;

private:
    void CreateStage() const;
    void OnDeath(std::any const& eventArgs);
    void Respawn();
    void NextStage();

    void OnStageComplete(std::any const& eventArgs);
    void OnPlayerConnect(std::any const& connectEvent);
    void OnPlayerDisconnect(std::any const& connectEvent);
    void OnPepperAttack(std::any const& eventArgs);
    void OnBonusPickup(std::any const& eventArgs);
    void OnScoreChange(std::any const& scoreEvent);
    void OnRestart(std::any const& eventArgs);

    int m_health{ game::startingLives };
    int m_pepper{ game::startingPepper };
    int m_score{};

    bool m_gameStarted{ false };
    GameMode m_gameMode{};
    gla::GameObject* m_pStageObject;
    int m_stageIndex{};
    gla::Timer* m_pStartTimer;
    gla::Timer* m_pEndTimer;
};

}  // namespace bt

#endif  // BURGERTIME_GAMESTATE_HPP
