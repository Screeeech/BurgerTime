#include "PlayerController.h"

#include <algorithm>
#include <print>

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

PlayerController::PlayerController(GameObject* pPlayer, int playerIndex, TextComponent* healthDisplay, TextComponent* scoreDisplay)
    : Component(pPlayer)
    , m_playerIndex(playerIndex)
    , m_health(pPlayer->AddComponent<HealthComponent>())
    , m_healthDisplay(healthDisplay)
    , m_score(pPlayer->AddComponent<ScoreComponent>())
    , m_scoreDisplay(scoreDisplay)

{
    auto& input = InputManager::GetInstance();
    input.BindAction<MoveCommand>("moveUp", playerIndex, m_pOwner, glm::vec3{ 0, -1, 0 });
    input.BindAction<MoveCommand>("moveLeft", playerIndex, m_pOwner, glm::vec3{ -1, 0, 0 });
    input.BindAction<MoveCommand>("moveDown", playerIndex, m_pOwner, glm::vec3{ 0, 1, 0 });
    input.BindAction<MoveCommand>("moveRight", playerIndex, m_pOwner, glm::vec3{ 1, 0, 0 });

    input.BindAction<CallbackCommand>(
        "damage", playerIndex, [playerIndex]() { EventManager::GetInstance().InvokeEvent(HealthEvent{ "healthChange", playerIndex, -1 }); });
    input.BindAction<CallbackCommand>(
        "attack", playerIndex, [playerIndex]() { EventManager::GetInstance().InvokeEvent(ScoreEvent{ "enemyKill", playerIndex, 10 }); });

    auto& event = EventManager::GetInstance();
    event.BindEvent("healthChange", this, &PlayerController::OnHealthChange);
    event.BindEvent("enemyKill", this, &PlayerController::OnEnemyKill);
    event.BindEvent("die", this , &PlayerController::OnDeath);

    m_healthDisplay->SetText(std::format("Lives: {}", m_health->GetHealth()));
    m_scoreDisplay->SetText(std::format("Score: {}", m_score->GetScore()));
}

PlayerController::~PlayerController() noexcept
{
    auto& input = InputManager::GetInstance();
    input.UnbindAction("moveUp", m_playerIndex);
    input.UnbindAction("moveLeft", m_playerIndex);
    input.UnbindAction("moveDown", m_playerIndex);
    input.UnbindAction("moveRight", m_playerIndex);
    input.UnbindAction("damage", m_playerIndex);
    input.UnbindAction("attack", m_playerIndex);

    auto& event = EventManager::GetInstance();
    event.UnbindEvent("healthChange", this);
    event.UnbindEvent("enemyKill", this);
    event.UnbindEvent("die", this);
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
        EventManager::GetInstance().QueueEvent(PlayerEvent{"die", m_playerIndex });
    }
    if(m_healthDisplay)
        m_healthDisplay->SetText(std::format("Lives: {}", m_health->GetHealth()));
}

void PlayerController::OnEnemyKill(const Event& event)
{
    const auto& scoreEvent{ dynamic_cast<const ScoreEvent&>(event) };

    if(scoreEvent.playerIndex != m_playerIndex)
        return;

    m_score->ChangeScore(scoreEvent.score);

    if(m_scoreDisplay)
        m_scoreDisplay->SetText(std::format("Score: {}", m_score->GetScore()));
}

void PlayerController::OnDeath(const Event& event)
{
    const auto& playerEvent{ dynamic_cast<const PlayerEvent&>(event) };

    if(playerEvent.playerIndex != m_playerIndex)
        return;

    auto* scene = SceneManager::GetInstance().GetActiveScene();
    scene->Remove(m_pOwner);
}


}  // namespace dae