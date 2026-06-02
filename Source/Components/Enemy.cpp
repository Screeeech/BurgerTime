#include "Components/Enemy.hpp"

#include <print>

#include "Commands/MoveCommand.hpp"
#include "Components/Animation.hpp"
#include "Components/CollisionRect.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/Timer.hpp"
#include "GameObject.hpp"
#include "Services/InputManager.hpp"
#include "Utils.hpp"

namespace bt
{

Enemy::Enemy(gla::GameObject* pOwner, Stage* pStage, int playerIndex)
    : Component(pOwner)
    , m_playerIndex(playerIndex)
    , m_pMoveComponent(pOwner->AddComponent<MoveComponent>(pStage, 0.6f, 0.5f))
    , m_pTimer(pOwner->AddComponent<gla::Timer>())
    , m_pAnimation(pOwner->GetComponent<gla::Animation>())
    , m_pHitBox(pOwner->AddComponent<gla::CollisionRect>(
          static_cast<uint32_t>(gla::Collider::Bits::Layer1),
          static_cast<uint32_t>(gla::Collider::Bits::Layer2),
          std::vector<gla::CollisionCallback>{ std::bind_front(&Enemy::OnPeppered, this) },
          glm::vec2{},
          glm::vec2{ 16.f, 16.f }))
    , m_stateMachine({ .animation = m_pAnimation })
{
    assert(m_pAnimation and "GameObject with Enemy component must have a valid Animation component first");
}
void Enemy::FixedUpdate(float /*fixedDeltaTime*/)
{
    enemystates::Context context{
        .animation = m_pAnimation,
        .timer = m_pTimer,
        .moveComponent = m_pMoveComponent,
    };
    m_stateMachine.Update(context);
}

void Enemy::OnActivate()
{
    auto& inputManager = gla::Locator::Get<gla::InputManager>();
    inputManager.BindAction<MoveCommand>("moveUp"_h, m_playerIndex, m_pOwner, glm::vec2{ 0, -1 });
    inputManager.BindAction<MoveCommand>("moveLeft"_h, m_playerIndex, m_pOwner, glm::vec2{ -1, 0 });
    inputManager.BindAction<MoveCommand>("moveDown"_h, m_playerIndex, m_pOwner, glm::vec2{ 0, 1 });
    inputManager.BindAction<MoveCommand>("moveRight"_h, m_playerIndex, m_pOwner, glm::vec2{ 1, 0 });
}
void Enemy::OnDeactivate()
{
    auto& inputManager = gla::Locator::Get<gla::InputManager>();
    inputManager.UnbindAction("moveUp"_h, m_playerIndex);
    inputManager.UnbindAction("moveLeft"_h, m_playerIndex);
    inputManager.UnbindAction("moveDown"_h, m_playerIndex);
    inputManager.UnbindAction("moveRight"_h, m_playerIndex);
}

void Enemy::OnDeath()
{
    m_stateMachine.TransitionTo<enemystates::Dying>({ .animation = m_pAnimation, .timer = m_pTimer });
}

void Enemy::OnPeppered(gla::Collider const& /*collider*/)
{
    using namespace enemystates;

    if (m_stateMachine.IsActive<Walking>())
        m_stateMachine.TransitionTo<StunnedWalking>({ .animation = m_pAnimation, .timer = m_pTimer });
    else if (m_stateMachine.IsActive<Climbing>())
        m_stateMachine.TransitionTo<StunnedClimbing>({ .animation = m_pAnimation, .timer = m_pTimer });
}

void Enemy::DefineAnimations(gla::Animation& animation, std::shared_ptr<gla::Texture2D> const& spriteSheetTexture)
{
    auto const size{ spriteSheetTexture->GetSize() };
    auto const cols{ static_cast<int>(size.x / 16.f) };
    auto const rows{ static_cast<int>(size.y / 16.f) };
    auto& spriteSheet = animation.AddSpriteSheet(spriteSheetTexture, cols, rows);

    animation.AddAnimation(
        "idle"_h,
        spriteSheet,
        {
            { .colIdx = 0, .rowIdx = 2 },
        });
    animation.AddAnimation(
        "walkUp"_h,
        spriteSheet,
        {
            { .colIdx = 4, .rowIdx = 2, .duration = 6.f / 60.f },
            { .colIdx = 5, .rowIdx = 2, .duration = 6.f / 60.f },
        });
    animation.AddAnimation(
        "walkDown"_h,
        spriteSheet,
        {
            { .colIdx = 0, .rowIdx = 2, .duration = 6.f / 60.f },
            { .colIdx = 1, .rowIdx = 2, .duration = 6.f / 60.f },
        });
    animation.AddAnimation(
        "walkLeft"_h,
        spriteSheet,
        {
            { .colIdx = 2, .rowIdx = 2, .duration = 6.f / 60.f },
            { .colIdx = 3, .rowIdx = 2, .duration = 6.f / 60.f },
        });
    animation.AddAnimation(
        "walkRight"_h,
        spriteSheet,
        {
            { .colIdx = 2, .rowIdx = 2, .duration = 6.f / 60.f, .flipX = true },
            { .colIdx = 3, .rowIdx = 2, .duration = 6.f / 60.f, .flipX = true },
        });
    animation.AddAnimation(
        "stunned"_h,
        spriteSheet,
        {
            { .colIdx = 4, .rowIdx = 3, .duration = 12.f / 60.f, .flipX = true },
            { .colIdx = 5, .rowIdx = 3, .duration = 12.f / 60.f, .flipX = true },
        });

    animation.SetAnimation("idle"_h, true);
}


}  // namespace bt