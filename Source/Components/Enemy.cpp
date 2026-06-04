#include "Components/Enemy.hpp"

#include <print>

#include "Commands/MoveCommand.hpp"
#include "Components/Animation.hpp"
#include "Components/BurgerPart.hpp"
#include "Components/CollisionRect.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/Timer.hpp"
#include "Events.hpp"
#include "GameObject.hpp"
#include "Services/EventManager.hpp"
#include "Services/InputManager.hpp"
#include "Utils.hpp"

namespace bt
{

Enemy::Enemy(gla::GameObject* pOwner, Stage* pStage, int playerIndex)
    : Component(pOwner)
    , m_entityIndex(playerIndex)
    , m_pMoveComponent(pOwner->AddComponent<MoveComponent>(pStage, 0.6f, 0.5f))
    , m_pTimer(pOwner->AddComponent<gla::Timer>())
    , m_pAnimation(pOwner->GetComponent<gla::Animation>())
    , m_pPlayerHitBox(pOwner->AddComponent<gla::CollisionRect>(
          gla::Collider::Bits::Layer1,
          0,
          "OnPepper"_h,
          glm::vec2{},
          glm::vec2{ 16.f, 16.f }))
    , m_pHeadHurtBox(pOwner->AddComponent<gla::CollisionRect>(
          gla::Collider::Bits::Layer5,
          0,
          "OnSquish"_h,
          glm::vec2{},
          glm::vec2{ 16.f, 4.f }))
    , m_pFeetHurtBox(pOwner->AddComponent<gla::CollisionRect>(
          gla::Collider::Bits::Layer6,
          0,
          "OnDrop"_h,
          glm::vec2{ 0.f, 12.f },
          glm::vec2{ 16.f, 4.f }))
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
        .entityIndex = m_entityIndex,
    };
    m_stateMachine.Update(context);
}

void Enemy::OnActivate()
{
    auto& inputManager = gla::Locator::Get<gla::InputManager>();
    inputManager.BindAction<MoveCommand>("moveUp"_h, m_entityIndex, m_pOwner, glm::vec2{ 0, -1 });
    inputManager.BindAction<MoveCommand>("moveLeft"_h, m_entityIndex, m_pOwner, glm::vec2{ -1, 0 });
    inputManager.BindAction<MoveCommand>("moveDown"_h, m_entityIndex, m_pOwner, glm::vec2{ 0, 1 });
    inputManager.BindAction<MoveCommand>("moveRight"_h, m_entityIndex, m_pOwner, glm::vec2{ 1, 0 });
}

void Enemy::OnDeactivate()
{
    auto& inputManager = gla::Locator::Get<gla::InputManager>();
    inputManager.UnbindAction("moveUp"_h, m_entityIndex);
    inputManager.UnbindAction("moveLeft"_h, m_entityIndex);
    inputManager.UnbindAction("moveDown"_h, m_entityIndex);
    inputManager.UnbindAction("moveRight"_h, m_entityIndex);
}

void Enemy::OnDeath()
{
    m_stateMachine.TransitionTo<enemystates::Dying>({ .animation = m_pAnimation, .timer = m_pTimer });
}

void Enemy::OnDrop(gla::Collider const& collider)
{
    m_stateMachine.TransitionTo<enemystates::Falling>({ .animation = m_pAnimation, .timer = m_pTimer });
    collider.m_pOwner->GetComponent<BurgerPart>()->AcquireEnemy(*m_pOwner, *this);
}

void Enemy::LandOnPlatform()
{
    m_stateMachine.TransitionTo<enemystates::StandingIdle>({ .animation = m_pAnimation });
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