#include "Components/PlayerController.hpp"

#include <algorithm>
#include <format>

#include "Commands/CallbackCommand.hpp"
#include "Commands/MoveCommand.hpp"
#include "Events.hpp"
#include "GameObject.hpp"
#include "SceneManager.hpp"
#include "ServiceLocator.hpp"
#include "Services/EventManager.hpp"
#include "Services/InputManager.hpp"
#include "Services/SoundService.hpp"
#include "Utils.hpp"

namespace bt
{
class Font;

PlayerController::PlayerController(gla::GameObject* pPlayer, int playerIndex)
    : Component(pPlayer)
    , m_playerIndex(playerIndex)

{
    if (auto* inputManager = gla::ServiceLocator::Request<gla::InputManager>().value_or(nullptr))
    {
        inputManager->BindAction<MoveCommand>("moveUp"_h, playerIndex, m_pOwner, glm::vec3{ 0, -1, 0 });
        inputManager->BindAction<MoveCommand>("moveLeft"_h, playerIndex, m_pOwner, glm::vec3{ -1, 0, 0 });
        inputManager->BindAction<MoveCommand>("moveDown"_h, playerIndex, m_pOwner, glm::vec3{ 0, 1, 0 });
        inputManager->BindAction<MoveCommand>("moveRight"_h, playerIndex, m_pOwner, glm::vec3{ 1, 0, 0 });

        inputManager->BindAction<gla::CallbackCommand>(
            "damage"_h,
            playerIndex,
            [playerIndex]() -> void
            {
                if (auto* eventManager = gla::ServiceLocator::Request<gla::EventManager>().value_or(nullptr))
                    eventManager->InvokeEvent(gla::PlayerEvent{ "die"_h, playerIndex });
            });
        inputManager->BindAction<gla::CallbackCommand>(
            "attack"_h,
            playerIndex,
            [playerIndex]() -> void
            {
                if (auto* eventManager = gla::ServiceLocator::Request<gla::EventManager>().value_or(nullptr))
                    eventManager->InvokeEvent(gla::ScoreEvent{ "scoreChange"_h, playerIndex, 10 });
            });
    }

    if (auto* eventManager = gla::ServiceLocator::Request<gla::EventManager>().value_or(nullptr))
        eventManager->BindEvent("die"_h, this, &PlayerController::OnDeath);
}

PlayerController::~PlayerController() noexcept
{
    if (auto* inputManager = gla::ServiceLocator::Request<gla::InputManager>().value_or(nullptr))
    {
        inputManager->UnbindAction("moveUp"_h, m_playerIndex);
        inputManager->UnbindAction("moveLeft"_h, m_playerIndex);
        inputManager->UnbindAction("moveDown"_h, m_playerIndex);
        inputManager->UnbindAction("moveRight"_h, m_playerIndex);
        inputManager->UnbindAction("damage"_h, m_playerIndex);
        inputManager->UnbindAction("attack"_h, m_playerIndex);
    }

    if (auto* eventManager = gla::ServiceLocator::Request<gla::EventManager>().value_or(nullptr))
        eventManager->UnbindEvents(this);
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
    const auto& playerEvent{ dynamic_cast<const gla::PlayerEvent&>(event) };

    if (playerEvent.playerIndex != m_playerIndex)
        return;

    if (auto* sound{ gla::ServiceLocator::Request<gla::SoundService>().value_or(nullptr) })
        sound->PlayAudio("death"_h);

    //auto* scene = gla::SceneManager::Get().GetActiveScene();
    //scene->RemoveGameObject(m_pOwner);
}


}  // namespace bt