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

Enemy::Enemy(gla::GameObject* pOwner, Stage* pStage, int entityIndex)
    : Component(pOwner)
    , m_entityIndex(entityIndex)
    , m_pMoveComponent(pOwner->AddComponent<MoveComponent>(pStage, 0.6f, 0.5f))
    , m_pTimer(pOwner->AddComponent<gla::Timer>())
    , m_pAnimation(pOwner->GetComponent<gla::Animation>())
    , m_pPlayerHitBox(pOwner->AddComponent<gla::CollisionRect>(
          gla::Collider::Bits::Layer1,
          gla::Collider::Bits::Layer2,
          gla::PlayerEvent("OnPepper"_h, entityIndex),
          glm::vec2{},
          glm::vec2{ 16.f, 16.f }))
    , m_pHeadHurtBox(pOwner->AddComponent<gla::CollisionRect>(
          gla::Collider::Bits::Layer5,
          0,
          [this](auto&, auto&) -> void { m_pStateMachine->TransitionTo<enemystates::Dying>(); },
          glm::vec2{},
          glm::vec2{ 16.f, 4.f }))
    , m_pFeetHurtBox(pOwner->AddComponent<gla::CollisionRect>(
          gla::Collider::Bits::Layer6,
          0,
          [this](auto&, gla::Collider const& otherCollider) -> void
          {
              auto* collidingBurger = otherCollider.m_pOwner->GetComponent<BurgerPart>();
              assert(collidingBurger != nullptr and "Collider's parent needs to have a BurgerComponent");

              collidingBurger->AcquireEnemy(*m_pOwner, *this);

              // When falling we disable the feet hurt box so no 2nd burger parts can be triggered
              m_pFeetHurtBox->DisableCollisionLayers(gla::Collider::Bits::Layer6);

              m_pStateMachine->TransitionTo<enemystates::Falling>();
          },
          glm::vec2{ 0.f, 12.f },
          glm::vec2{ 16.f, 4.f }))
    , m_pStateMachine(pOwner->AddComponent<enemystates::EnemyStateMachine>(enemystates::Context{
          .animation = *m_pAnimation,
          .stunTimer = *m_pTimer,
          .moveComponent = *m_pMoveComponent,
          .playerHitbox = *m_pPlayerHitBox,
          .headBurtBox = *m_pHeadHurtBox,
          .feetHurtBox = *m_pFeetHurtBox,
          .entityIndex = m_entityIndex,
      }))
{
    assert(m_pAnimation and "GameObject with Enemy component must have a valid Animation component first");
}

void Enemy::OnActivate()
{
    // auto& eventManager = gla::Locator::Get<gla::EventManager>();
    // eventManager.BindEvent("OnPepper"_h, this, &Enemy::OnPepper);

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

void Enemy::DefineAnimationsHotDog(gla::Animation& animation, std::shared_ptr<gla::Texture2D> const& spriteSheetTexture)
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
    animation.AddAnimation(
        "dying"_h,
        spriteSheet,
        {
            {
                .colIdx = 0,
                .rowIdx = 3,
                .duration = 15.f / 60.f,
            },
            {
                .colIdx = 1,
                .rowIdx = 3,
                .duration = 15.f / 60.f,
            },
            {
                .colIdx = 2,
                .rowIdx = 3,
                .duration = 15.f / 60.f,
            },
            {
                .colIdx = 3,
                .rowIdx = 3,
                .duration = 15.f / 60.f,
            },
            {
                .colIdx = 6,
                .rowIdx = 3,
                .duration = 15.f / 60.f,
            },
        });

    animation.SetAnimation("idle"_h, true);
}

void Enemy::DefineAnimationsEgg(gla::Animation& animation, std::shared_ptr<gla::Texture2D> const& spriteSheetTexture)
{
    auto const size{ spriteSheetTexture->GetSize() };
    auto const cols{ static_cast<int>(size.x / 16.f) };
    auto const rows{ static_cast<int>(size.y / 16.f) };
    auto& spriteSheet = animation.AddSpriteSheet(spriteSheetTexture, cols, rows);

    animation.AddAnimation(
        "idle"_h,
        spriteSheet,
        {
            { .colIdx = 0, .rowIdx = 6 },
        });
    animation.AddAnimation(
        "walkUp"_h,
        spriteSheet,
        {
            { .colIdx = 4, .rowIdx = 6, .duration = 6.f / 60.f },
            { .colIdx = 5, .rowIdx = 6, .duration = 6.f / 60.f },
        });
    animation.AddAnimation(
        "walkDown"_h,
        spriteSheet,
        {
            { .colIdx = 0, .rowIdx = 6, .duration = 6.f / 60.f },
            { .colIdx = 1, .rowIdx = 6, .duration = 6.f / 60.f },
        });
    animation.AddAnimation(
        "walkLeft"_h,
        spriteSheet,
        {
            { .colIdx = 2, .rowIdx = 6, .duration = 6.f / 60.f },
            { .colIdx = 3, .rowIdx = 6, .duration = 6.f / 60.f },
        });
    animation.AddAnimation(
        "walkRight"_h,
        spriteSheet,
        {
            { .colIdx = 2, .rowIdx = 6, .duration = 6.f / 60.f, .flipX = true },
            { .colIdx = 3, .rowIdx = 6, .duration = 6.f / 60.f, .flipX = true },
        });
    animation.AddAnimation(
        "stunned"_h,
        spriteSheet,
        {
            { .colIdx = 4, .rowIdx = 7, .duration = 12.f / 60.f, .flipX = true },
            { .colIdx = 5, .rowIdx = 7, .duration = 12.f / 60.f, .flipX = true },
        });
    animation.AddAnimation(
        "dying"_h,
        spriteSheet,
        {
            {
                .colIdx = 0,
                .rowIdx = 7,
                .duration = 15.f / 60.f,
            },
            {
                .colIdx = 1,
                .rowIdx = 7,
                .duration = 15.f / 60.f,
            },
            {
                .colIdx = 2,
                .rowIdx = 7,
                .duration = 15.f / 60.f,
            },
            {
                .colIdx = 3,
                .rowIdx = 7,
                .duration = 15.f / 60.f,
            },
            {
                .colIdx = 6,
                .rowIdx = 7,
                .duration = 15.f / 60.f,
            },
        });

    animation.SetAnimation("idle"_h, true);
}


}  // namespace bt