#include "Components/PlayerController.hpp"

#include <algorithm>
#include <format>

#include "Colors.hpp"
#include "Commands/CallbackCommand.hpp"
#include "Commands/MoveCommand.hpp"
#include "Components/Collider.hpp"
#include "Components/CollisionRect.hpp"
#include "GameEvents.hpp"
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

PlayerController::PlayerController(gla::GameObject* pPlayer, Stage* stage, Pepper* pepper, int playerIndex)
    : Renderable(pPlayer, 10)
    , m_playerIndex(playerIndex)
    , m_pStage(stage)
    , m_pPepper(pepper)
    , m_pAnimation(pPlayer->GetComponent<gla::Animation>())
    , m_pHitBox(pPlayer->AddComponent<gla::CollisionRect>(
          static_cast<uint32_t>(gla::Collider::Bits::Layer1),
          0,
          std::vector<gla::CollisionCallback>{ [](auto&) { std::println("Hit!"); } },
          glm::vec2{},
          glm::vec2{ 16.f, 32.f }))
    , m_pHurtBox(pPlayer->AddComponent<gla::CollisionRect>(
          static_cast<uint32_t>(gla::Collider::Bits::Layer2),
          0,
          std::vector<gla::CollisionCallback>{ std::bind_front(&PlayerController::OnHit, this) },
          glm::vec2{},
          glm::vec2{ 16.f, 32.f }))
    , m_finiteStateMachine({ .animation = m_pAnimation })
{
}

void PlayerController::SetDirection(glm::vec2 direction)
{
    m_direction.x = std::clamp(m_direction.x + direction.x, -1.f, 1.f);
    m_direction.y = std::clamp(m_direction.y + direction.y, -1.f, 1.f);
}

auto PlayerController::GetDirection() const -> glm::vec2
{
    return m_direction;
}

void PlayerController::Move(glm::vec2 displacement) const
{
    m_pOwner->GetTransform().ChangeLocalPosition(displacement);
}

void PlayerController::OnHit(gla::Collider const& /*collider*/) const
{
    std::println("Player hit!!");
    gla::Locator::Get<gla::ISound>().PlayAudio("death"_h);
}

void PlayerController::Render()
{
    glm::vec2 const worldPos = m_pOwner->GetTransform().GetWorldPosition();
    glm::vec2 const pos = worldPos + spriteFeetOffset;

    auto const& renderer = gla::Locator::Get<gla::Renderer>();

    renderer.SetColor(colors::Red);
    renderer.DrawRect({ pos.x, pos.y, 1.f, 1.f });

    renderer.SetColor(colors::Blue);
    renderer.DrawRect({ pos.x + (m_direction.x * 4), pos.y, 1.f, 1.f });
}

void PlayerController::FixedUpdate(float deltaTime)
{
    glm::vec2 const worldPos = m_pOwner->GetTransform().GetWorldPosition();
    glm::vec2 const pos = worldPos + spriteFeetOffset;

    playerstates::Context context{
        .direction = m_direction,
        .position = pos,
        .animation = m_pAnimation,
        .stage = m_pStage,
        .playerController = this,
        .deltaTime = deltaTime,
    };
    m_finiteStateMachine.Update(context);

    // Reset direction for the next frame
    m_direction = glm::vec2(0.0f);
}

void PlayerController::OnActivate()
{
    Renderable::OnActivate();

    auto& inputManager = gla::Locator::Get<gla::InputManager>();
    inputManager.BindAction<MoveCommand>("moveUp"_h, m_playerIndex, m_pOwner, glm::vec2{ 0, -1 });
    inputManager.BindAction<MoveCommand>("moveLeft"_h, m_playerIndex, m_pOwner, glm::vec2{ -1, 0 });
    inputManager.BindAction<MoveCommand>("moveDown"_h, m_playerIndex, m_pOwner, glm::vec2{ 0, 1 });
    inputManager.BindAction<MoveCommand>("moveRight"_h, m_playerIndex, m_pOwner, glm::vec2{ 1, 0 });
    inputManager.BindAction<gla::CallbackCommand>(
        "attack"_h,
        m_playerIndex,
        [this]
        {
            std::println("Attacking!");
            gla::Locator::Get<gla::EventManager>().InvokeEvent(
                PepperEvent("pepper"_h, m_playerIndex, GetDirection(), m_pOwner->GetWorldPosition() + spriteFeetOffset, m_pPepper));
        });
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


}  // namespace bt