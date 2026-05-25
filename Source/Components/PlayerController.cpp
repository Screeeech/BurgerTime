#include "Components/PlayerController.hpp"

#include <algorithm>
#include <format>

#include "Colors.hpp"
#include "Commands/CallbackCommand.hpp"
#include "Commands/MoveCommand.hpp"
#include "Events.hpp"
#include "GameObject.hpp"
#include "Locator.hpp"
#include "SceneManager.hpp"
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
}

void PlayerController::FixedUpdate(float /*deltaTime*/)
{
    glm::vec3 constexpr spriteFeetOffset{ 8.f, 15.f, 0.f };
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

void PlayerController::OnActivate()
{
    Renderable::OnActivate();

    auto& inputManager = gla::Locator::Get<gla::InputManager>();
    inputManager.BindAction<MoveCommand>("moveUp"_h, m_playerIndex, m_pOwner, glm::vec3{ 0, -1, 0 });
    inputManager.BindAction<MoveCommand>("moveLeft"_h, m_playerIndex, m_pOwner, glm::vec3{ -1, 0, 0 });
    inputManager.BindAction<MoveCommand>("moveDown"_h, m_playerIndex, m_pOwner, glm::vec3{ 0, 1, 0 });
    inputManager.BindAction<MoveCommand>("moveRight"_h, m_playerIndex, m_pOwner, glm::vec3{ 1, 0, 0 });
}

void PlayerController::OnDeactivate()
{
    Renderable::OnDeactivate();

    auto& inputManager = gla::Locator::Get<gla::InputManager>();
    inputManager.UnbindAction("moveUp"_h, m_playerIndex);
    inputManager.UnbindAction("moveLeft"_h, m_playerIndex);
    inputManager.UnbindAction("moveDown"_h, m_playerIndex);
    inputManager.UnbindAction("moveRight"_h, m_playerIndex);
    inputManager.UnbindAction("damage"_h, m_playerIndex);
    inputManager.UnbindAction("attack"_h, m_playerIndex);

    gla::Locator::Get<gla::EventManager>().UnbindEvents(this);
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

    gla::Locator::Get<gla::ISound>().PlayAudio("death"_h);
}

void PlayerController::Render()
{
    glm::vec3 constexpr spriteFeetOffset{ 8.f, 15.f, 0.f };
    glm::vec3 const worldPos = m_pOwner->GetTransform().GetWorldPosition();
    glm::vec3 const pos = worldPos + spriteFeetOffset;

    auto const& renderer = gla::Locator::Get<gla::Renderer>();

    renderer.SetColor(colors::Red);
    renderer.DrawRect({ pos.x, pos.y, 1.f, 1.f });

    renderer.SetColor(colors::Blue);
    renderer.DrawRect({ pos.x + (m_direction.x * 4), pos.y, 1.f, 1.f });
}


}  // namespace bt