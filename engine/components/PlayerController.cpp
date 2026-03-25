#include "PlayerController.h"

#include <algorithm>
#include <format>
#include <sdbm.hpp>
using namespace sdbm;

#include "commands/CallbackCommand.h"
#include "commands/MoveCommand.h"
#include "components/TextComponent.h"
#include "EventManager.h"
#include "Events.h"
#include "GameObject.h"
#include "InputManager.h"
#include "ScoreComponent.h"

namespace dae
{
class Font;

PlayerController::PlayerController(GameObject* pPlayer, int playerIndex, TextComponent* healthDisplay,
                                   TextComponent* scoreDisplay)
    : Component(pPlayer)
    , m_playerIndex(playerIndex)
    , m_health(pPlayer->AddComponent<HealthComponent>())
    , m_healthDisplay(healthDisplay)
    , m_score(pPlayer->AddComponent<ScoreComponent>())
    , m_scoreDisplay(scoreDisplay)

{
    auto& input = InputManager::Get();
    input.BindAction<MoveCommand>("moveUp"_h, playerIndex, m_pOwner, glm::vec3{ 0, -1, 0 });
    input.BindAction<MoveCommand>("moveLeft"_h, playerIndex, m_pOwner, glm::vec3{ -1, 0, 0 });
    input.BindAction<MoveCommand>("moveDown"_h, playerIndex, m_pOwner, glm::vec3{ 0, 1, 0 });
    input.BindAction<MoveCommand>("moveRight"_h, playerIndex, m_pOwner, glm::vec3{ 1, 0, 0 });

    input.BindAction<CallbackCommand>("damage"_h, playerIndex, [playerIndex]()
                                      { EventManager::Get().InvokeEvent(HealthEvent{ "healthChange"_h, playerIndex, -1 }); });
    input.BindAction<CallbackCommand>("attack"_h, playerIndex, [playerIndex]()
                                      { EventManager::Get().InvokeEvent(ScoreEvent{ "enemyKill"_h, playerIndex, 10 }); });

    auto& event = EventManager::Get();
    event.BindEvent("healthChange"_h, this, &PlayerController::OnHealthChange);
    event.BindEvent("enemyKill"_h, this, &PlayerController::OnEnemyKill);
    event.BindEvent("die"_h, this, &PlayerController::OnDeath);

    m_healthDisplay->SetText(std::format("Lives: {}", m_health->GetHealth()));
    m_scoreDisplay->SetText(std::format("Score: {}", m_score->GetScore()));
}

PlayerController::~PlayerController() noexcept
{
    auto& input = InputManager::Get();
    input.UnbindAction("moveUp"_h, m_playerIndex);
    input.UnbindAction("moveLeft"_h, m_playerIndex);
    input.UnbindAction("moveDown"_h, m_playerIndex);
    input.UnbindAction("moveRight"_h, m_playerIndex);
    input.UnbindAction("damage"_h, m_playerIndex);
    input.UnbindAction("attack"_h, m_playerIndex);

    auto& event = EventManager::Get();
    event.UnbindEvent("healthChange"_h, this);
    event.UnbindEvent("enemyKill"_h, this);
    event.UnbindEvent("die"_h, this);
}

void PlayerController::Update(float deltaTime)
{
    // Check if the direction vector has significant length
    if(glm::length(m_direction) > 0)
    {
        const glm::vec3 direction = glm::normalize(m_direction);
        const glm::vec3 velocity = direction * deltaTime * m_speed;
        m_pOwner->GetTransform().ChangeLocalPosition(velocity);
    }
    // Reset direction for the next frame
    m_direction = glm::vec3(0.0f);
}

void PlayerController::SetDirection(glm::vec3 direction)
{
    m_direction.x = std::clamp(m_direction.x + direction.x, -1.f, 1.f);
    m_direction.y = std::clamp(m_direction.y + direction.y, -1.f, 1.f);
}

void PlayerController::OnHealthChange(const Event& event)
{
    const auto& hpEvent = dynamic_cast<const HealthEvent&>(event);

    if(hpEvent.playerIndex != m_playerIndex)
        return;

    if(m_health->ChangeHealth(hpEvent.health))
    {
        EventManager::Get().QueueEvent(PlayerEvent{ "die"_h, m_playerIndex });
    }
    if(m_healthDisplay)
        m_healthDisplay->SetText(std::format("Lives: {}", m_health->GetHealth()));
}

void PlayerController::OnEnemyKill(const Event& event)
{
    const auto& scoreEvent{ dynamic_cast<const ScoreEvent&>(event) };

    if(scoreEvent.playerIndex != m_playerIndex)
        return;

    m_score->ChangeScore(scoreEvent.scoreChange);

    if(m_scoreDisplay)
        m_scoreDisplay->SetText(std::format("Score: {}", m_score->GetScore()));
}

void PlayerController::OnDeath(const Event& event)
{
    const auto& playerEvent{ dynamic_cast<const PlayerEvent&>(event) };

    if(playerEvent.playerIndex != m_playerIndex)
        return;

    auto* scene = SceneManager::Get().GetActiveScene();
    scene->RemoveGameObject(m_pOwner);
}


}  // namespace dae