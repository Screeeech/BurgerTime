#include "Components/PlayerController.hpp"

#include <algorithm>
#include <format>

#include "Commands/CallbackCommand.hpp"
#include "Commands/MoveCommand.hpp"
#include "EventManager.hpp"
#include "Events.hpp"
#include "GameObject.hpp"
#include "InputManager.hpp"
#include "SceneManager.hpp"
#include "Utils.hpp"

namespace bt
{
class Font;

PlayerController::PlayerController(gla::GameObject* pPlayer, int playerIndex)
    : Component(pPlayer)
    , m_playerIndex(playerIndex)

{
    auto& input = gla::InputManager::Get();
    input.BindAction<MoveCommand>("moveUp"_h, playerIndex, m_pOwner, glm::vec3{0, -1, 0});
    input.BindAction<MoveCommand>("moveLeft"_h, playerIndex, m_pOwner, glm::vec3{-1, 0, 0});
    input.BindAction<MoveCommand>("moveDown"_h, playerIndex, m_pOwner, glm::vec3{0, 1, 0});
    input.BindAction<MoveCommand>("moveRight"_h, playerIndex, m_pOwner, glm::vec3{1, 0, 0});

    input.BindAction<gla::CallbackCommand>(
        "damage"_h,
        playerIndex,
        [playerIndex]() -> void { gla::EventManager::Get().InvokeEvent(gla::HealthEvent{"healthChange"_h, playerIndex, -1}); });
    input.BindAction<gla::CallbackCommand>(
        "attack"_h,
        playerIndex,
        [playerIndex]() -> void { gla::EventManager::Get().InvokeEvent(gla::ScoreEvent{"scoreChange"_h, playerIndex, 10}); });

    gla::EventManager::Get().BindEvent("die"_h, this, &PlayerController::OnDeath);
}

PlayerController::~PlayerController() noexcept
{
    auto& input = gla::InputManager::Get();
    input.UnbindAction("moveUp"_h, m_playerIndex);
    input.UnbindAction("moveLeft"_h, m_playerIndex);
    input.UnbindAction("moveDown"_h, m_playerIndex);
    input.UnbindAction("moveRight"_h, m_playerIndex);
    input.UnbindAction("damage"_h, m_playerIndex);
    input.UnbindAction("attack"_h, m_playerIndex);

    gla::EventManager::Get().UnbindEvents(this);
}

void PlayerController::Update(float deltaTime)
{
    // Check if the direction vector has significant length
    if (glm::length(m_direction) > 0)
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

void PlayerController::OnDeath(const gla::Event& event)
{
    const auto& playerEvent{dynamic_cast<const gla::PlayerEvent&>(event)};

    if (playerEvent.playerIndex != m_playerIndex)
        return;

    auto* scene = gla::SceneManager::Get().GetActiveScene();
    scene->RemoveGameObject(m_pOwner);
}


}  // namespace bt