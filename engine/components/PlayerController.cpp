#include "PlayerController.h"

#include <algorithm>
#include <print>

#include "commands/CallbackCommand.h"
#include "commands/MoveCommand.h"
#include "EventManager.h"
#include "Events.h"
#include "GameObject.h"
#include "InputManager.h"

namespace dae
{

PlayerController::PlayerController(GameObject* pPlayer, int playerIndex)
    : Component(pPlayer)
{
    auto& input = InputManager::GetInstance();
    input.BindAction<MoveCommand>("moveUp", playerIndex, m_pOwner, glm::vec3{ 0, -1, 0 });
    input.BindAction<MoveCommand>("moveLeft", playerIndex, m_pOwner, glm::vec3{ -1, 0, 0 });
    input.BindAction<MoveCommand>("moveDown", playerIndex, m_pOwner, glm::vec3{ 0, 1, 0 });
    input.BindAction<MoveCommand>("moveRight", playerIndex, m_pOwner, glm::vec3{ 1, 0, 0 });

    HealthEvent hpEvent{ 100, 3 };
    input.BindAction<CallbackCommand>("test", playerIndex, [hpEvent](){ EventManager::GetInstance().InvokeEvent(100, hpEvent); });

    EventManager::GetInstance().AddListener(100, this, std::bind(&PlayerController::Test, this , std::placeholders::_1));
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

void PlayerController::Test(const Event& event)
{
    std::println("HP Event id: {}", event.eventID);
}

}  // namespace dae