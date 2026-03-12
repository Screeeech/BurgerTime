//
// Created by lily-laptop on 11/03/2026.
//

#include "PlayerController.h"

#include <algorithm>
#include <print>

#include "commands/MoveCommand.h"
#include "GameObject.h"
#include "InputManager.h"

namespace dae
{

PlayerController::PlayerController(GameObject* pPlayer, int playerIndex)
    : Component(pPlayer)
{
    auto& input = InputManager::GetInstance();
    constexpr float speed{ 100.f };
    input.BindAction<MoveCommand>("moveUp", playerIndex, m_pOwner, glm::vec3{ 0, -1, 0 } * speed);
    input.BindAction<MoveCommand>("moveLeft", playerIndex, m_pOwner, glm::vec3{ -1, 0, 0 } * speed);
    input.BindAction<MoveCommand>("moveDown", playerIndex, m_pOwner, glm::vec3{ 0, 1, 0 } * speed);
    input.BindAction<MoveCommand>("moveRight", playerIndex, m_pOwner, glm::vec3{ 1, 0, 0 } * speed);
}

PlayerController::~PlayerController() {}

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


}  // namespace dae