#ifndef BURGERTIME_ENEMYSPAWNER_HPP
#define BURGERTIME_ENEMYSPAWNER_HPP

#include <any>
#include <optional>
#include <set>
#include <unordered_map>

#include "Component.hpp"
#include "Entity.hpp"

namespace gla
{
class Timer;
}
namespace bt
{

class EnemySpawner final : public gla::Component
{
    static constexpr float spawnDelay{ 4.f };

public:
    // Single player
    explicit EnemySpawner(
        gla::GameObject* pOwner, gla::GameObject* player1, std::unordered_map<Entity::Type, int> enemyCounts);

    // Co-op
    explicit EnemySpawner(
        gla::GameObject* pOwner, gla::GameObject* player1, gla::GameObject* player2, std::unordered_map<Entity::Type, int> enemyCounts);

    // Versus
    explicit EnemySpawner(
        gla::GameObject* pOwner, gla::GameObject* player1, int enemyPlayerIndex, std::unordered_map<Entity::Type, int> enemyCounts);

protected:
    void Update() override;
    void OnActivate() override;
    void OnDeactivate() override;

private:
    void AttemptSpawn();
    auto GetFreeEntityIndex() -> int;

    void OnEnemyDeath(std::any const& enemyDeathEvent);
    void OnEnemyDisable(std::any const& eventArgs);

    bool m_spawnerActive{ true };
    gla::GameObject* m_pPlayer1;
    gla::GameObject* m_pPlayer2{};
    std::unordered_map<Entity::Type, int> m_totalEnemies;
    std::unordered_map<Entity::Type, int> m_aliveEnemies;
    std::set<int> m_usedEntityIndices;

    std::optional<int> m_enemyPlayerIndex;
    bool m_shouldRespawnEnemyPlayer{};

    gla::Timer* m_pSpawnDelayTimer;
};

}  // namespace bt

#endif  // BURGERTIME_ENEMYSPAWNER_HPP
