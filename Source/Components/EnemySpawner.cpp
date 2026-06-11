#include "Components/EnemySpawner.hpp"

#include <random>

#include "Components/EnemyAI.hpp"
#include "Components/Stage.hpp"
#include "Components/Timer.hpp"
#include "GameEvents.hpp"
#include "GameObject.hpp"
#include "Locator.hpp"
#include "Services/EventManager.hpp"
#include "Services/InputManager.hpp"
#include "Utils.hpp"

namespace bt
{

EnemySpawner::EnemySpawner(
    gla::GameObject* pOwner, gla::GameObject* player1, gla::GameObject* player2, std::unordered_map<Entity::Type, int> enemyCounts)
    : Component(pOwner)
    , m_pPlayer1(player1)
    , m_pPlayer2(player2)
    , m_totalEnemies(std::move(enemyCounts))
    , m_pSpawnDelayTimer(pOwner->AddComponent<gla::Timer>())
{
}

void EnemySpawner::Update()
{
    if (m_pSpawnDelayTimer->IsFinished())
    {
        m_pSpawnDelayTimer->Start(spawnDelay);
        AttemptSpawn();
    }
}

void EnemySpawner::OnActivate()
{
    m_aliveEnemies.clear();
    m_pSpawnDelayTimer->Start(spawnDelay);
    gla::Locator::Get<gla::EventManager>().BindEvent("EnemyDeath"_h, this, &EnemySpawner::OnEnemyDeath);

}

void EnemySpawner::OnDeactivate()
{
    gla::Locator::Get<gla::EventManager>().UnbindEvents(this);
}

void EnemySpawner::AttemptSpawn()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution enemyRandom(0, 2);

    int const randomStartingType = enemyRandom(gen);

    auto const type = [&] -> std::optional<Entity::Type>
    {
        for (int i{}; i <= 2; i++)
        {
            auto const t = static_cast<Entity::Type>((i + randomStartingType) % 3);
            if (m_aliveEnemies[t] < m_totalEnemies.at(t))
                return t;
        }
        return std::nullopt;
    }();
    if (not type)
        return;

    static std::uniform_int_distribution sideRandom(0, 1);
    bool const left = sideRandom(gen);
    glm::vec2 const spawnPosition = { left ? -15 : (Stage::stageWidth * Stage::tileWidth) + 15, -2 };

    auto const entityIndex = GetFreeEntityIndex();

    auto* enemyObject = m_pOwner->CreateChild(spawnPosition, std::format("Enemy {}", entityIndex));
    Entity::CreateEnemy(enemyObject, entityIndex, *type, { left ? 1 : -1, 0 });
    enemyObject->AddComponent<EnemyAI>(entityIndex, m_pPlayer1, m_pPlayer2);
    enemyObject->Activate();
}

auto EnemySpawner::GetFreeEntityIndex() -> int
{
    int entityIndex{ gla::InputManager::maxPlayers };
    while (m_usedEntityIndices.contains(entityIndex))
        entityIndex++;

    m_usedEntityIndices.insert(entityIndex);
    return entityIndex;
}

void EnemySpawner::OnEnemyDeath(std::any const& enemyDeathEvent)
{
    auto const& args = std::any_cast<EnemyDeathEvent const&>(enemyDeathEvent);
    if (not m_usedEntityIndices.contains(args.entityIndex))
        return;

    m_usedEntityIndices.erase(args.entityIndex);
    m_totalEnemies[args.enemyType]--;
}


}  // namespace bt