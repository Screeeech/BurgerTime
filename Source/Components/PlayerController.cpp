#include "Components/PlayerController.hpp"

#include <algorithm>
#include <format>

#include "Colors.hpp"
#include "Commands/CallbackCommand.hpp"
#include "Commands/MoveCommand.hpp"
#include "Events.hpp"
#include "GameObject.hpp"
#include "SceneManager.hpp"
#include "ServiceLocator.hpp"
#include "Services/EventManager.hpp"
#include "Services/InputManager.hpp"
#include "Services/ISound.hpp"
#include "Services/Renderer.hpp"
#include "Utils.hpp"

namespace bt
{
class Font;

PlayerController::PlayerController(gla::GameObject* pPlayer, Stage* stage, int playerIndex)
    : Renderable(pPlayer, 10)
    , m_finiteStateMachine({ .animation = pPlayer->GetComponent<gla::Animation>() })
    , m_playerIndex(playerIndex)
    , m_stage(stage)
{
    if (auto* inputManager = gla::ServiceLocator::Request<gla::InputManager>().value_or(nullptr))
    {
        inputManager->BindAction<MoveCommand>("moveUp"_h, playerIndex, m_pOwner, glm::vec3{ 0, -1, 0 });
        inputManager->BindAction<MoveCommand>("moveLeft"_h, playerIndex, m_pOwner, glm::vec3{ -1, 0, 0 });
        inputManager->BindAction<MoveCommand>("moveDown"_h, playerIndex, m_pOwner, glm::vec3{ 0, 1, 0 });
        inputManager->BindAction<MoveCommand>("moveRight"_h, playerIndex, m_pOwner, glm::vec3{ 1, 0, 0 });
    }
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
    if (glm::length(m_direction) > 0)
    {
        m_direction = glm::normalize(m_direction);
        m_direction *= deltaTime * m_speed;
    }

    glm::vec3 constexpr spriteFeetOffset{ 0.f, 16.f, 0.f };
    glm::vec3 const worldPos = m_pOwner->GetTransform().GetWorldPosition();
    glm::vec3 const pos = worldPos + spriteFeetOffset;

    playerstates::Context context{
        .direction = m_direction,
        .position = pos,
        .animation = m_pOwner->GetComponent<gla::Animation>(),
        .stage = m_stage,
        .playerController = this,
    };
    m_finiteStateMachine.Update(context);

    // Reset direction for the next frame
    m_direction = glm::vec3(0.0f);
}

void PlayerController::SetDirection(glm::vec3 direction)
{
    m_direction.x = std::clamp(m_direction.x + direction.x, -1.f, 1.f);
    m_direction.y = std::clamp(m_direction.y + direction.y, -1.f, 1.f);
}

void PlayerController::Move(glm::vec3 displacement) const
{
    m_pOwner->GetTransform().ChangeLocalPosition(displacement);
}

void PlayerController::OnDeath(const gla::Event& event) const
{
    const auto& playerEvent{ dynamic_cast<const gla::PlayerEvent&>(event) };

    if (playerEvent.playerIndex != m_playerIndex)
        return;

    if (auto* sound{ gla::ServiceLocator::Request<gla::ISound>().value_or(nullptr) })
        sound->PlayAudio("death"_h);

    // auto* scene = gla::SceneManager::Get().GetActiveScene();
    // scene->RemoveGameObject(m_pOwner);
}

void PlayerController::Render()
{
    glm::vec3 constexpr spriteFeetOffset{ 0.f, 16.f, 0.f };
    glm::vec3 const worldPos = m_pOwner->GetTransform().GetWorldPosition();
    glm::vec3 const pos = worldPos + spriteFeetOffset;

    auto const* renderer = gla::ServiceLocator::Request<gla::Renderer>().value();
    renderer->SetColor(colors::Red);
    renderer->DrawRect({ pos.x, pos.y, 1.f, 1.f });
}


}  // namespace bt