#include "Components/Player.hpp"

#include <algorithm>
#include <format>

#include "Commands/CallbackCommand.hpp"
#include "Commands/MoveCommand.hpp"
#include "Components/Collider.hpp"
#include "Components/CollisionRect.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/Timer.hpp"
#include "Constants.hpp"
#include "GameEvents.hpp"
#include "GameObject.hpp"
#include "Locator.hpp"
#include "SceneManager.hpp"
#include "Services/EventManager.hpp"
#include "Services/InputManager.hpp"
#include "Services/ISound.hpp"
#include "Utils.hpp"

namespace bt
{
class Font;

Player::Player(gla::GameObject* pPlayer, Stage* pStage, Pepper* pPepper, int playerIndex)
    : Component(pPlayer)
    , m_playerIndex(playerIndex)
    , m_pPepper(pPepper)
    , m_pMoveComponent(pPlayer->AddComponent<MoveComponent>(pStage, 0.85f, 0.65f))
    , m_pAnimation(pPlayer->GetComponent<gla::Animation>())
    , m_pHitBox(pPlayer->AddComponent<gla::CollisionRect>(
          gla::Collider::Bits::Layer2,
          gla::Collider::Bits::Layer3,
          std::vector<gla::CollisionCallback>{ std::bind_front(&Player::OnDamage, this) },
          glm::vec2{},
          glm::vec2{ 16.f, 16.f }))
    //, m_pTime(pPlayer->AddComponent<gla::Timer>())
    , m_finiteStateMachine({ .animation = m_pAnimation })
{
}

void Player::OnDamage(gla::Collider const& /*collider*/)
{
    m_finiteStateMachine.TransitionTo<playerstates::Dying>({ .animation = m_pAnimation });
    m_pHitBox->Disable();

    std::println("Player hit!");

    gla::Locator::Get<gla::ISound>().PlayAudio("death"_h);
}

void Player::FixedUpdate(float deltaTime)
{
    playerstates::Context context{
        .direction = m_pMoveComponent->GetDirection(),
        .animation = m_pAnimation,
        .moveComponent = m_pMoveComponent,
        .deltaTime = deltaTime,
    };
    m_finiteStateMachine.Update(context);
}

void Player::OnActivate()
{
    auto& inputManager = gla::Locator::Get<gla::InputManager>();
    inputManager.BindAction<MoveCommand>("moveUp"_h, m_playerIndex, m_pOwner, glm::vec2{ 0, -1 });
    inputManager.BindAction<MoveCommand>("moveLeft"_h, m_playerIndex, m_pOwner, glm::vec2{ -1, 0 });
    inputManager.BindAction<MoveCommand>("moveDown"_h, m_playerIndex, m_pOwner, glm::vec2{ 0, 1 });
    inputManager.BindAction<MoveCommand>("moveRight"_h, m_playerIndex, m_pOwner, glm::vec2{ 1, 0 });
    inputManager.BindAction<gla::CallbackCommand>(
        "attack"_h,
        m_playerIndex,
        [this] -> void
        {
            std::println("Attacking!");
            gla::Locator::Get<gla::EventManager>().InvokeEvent(
                PepperEvent("pepper"_h, m_playerIndex, m_pMoveComponent->GetDirection(), m_pMoveComponent->GetSpritePosition(), m_pPepper));
        });
}

void Player::OnDeactivate()
{
    auto& inputManager = gla::Locator::Get<gla::InputManager>();
    inputManager.UnbindAction("moveUp"_h, m_playerIndex);
    inputManager.UnbindAction("moveLeft"_h, m_playerIndex);
    inputManager.UnbindAction("moveDown"_h, m_playerIndex);
    inputManager.UnbindAction("moveRight"_h, m_playerIndex);
    inputManager.UnbindAction("damage"_h, m_playerIndex);

    gla::Locator::Get<gla::EventManager>().UnbindEvents(this);
}
void Player::DefineAnimations(gla::Animation& animation, std::shared_ptr<gla::Texture2D> const& spriteSheetTexture)
{
    auto const size{ spriteSheetTexture->GetSize() };
    auto const cols{ static_cast<int>(size.x / 16.f) };
    auto const rows{ static_cast<int>(size.y / 16.f) };
    auto& spriteSheet{ animation.AddSpriteSheet(spriteSheetTexture, cols, rows) };

    animation.AddAnimation(
        "idle"_h,
        spriteSheet,
        {
            { .colIdx = 1, .rowIdx = 0 },
        });
    animation.AddAnimation(
        "walkDown"_h,
        spriteSheet,
        {
            { .colIdx = 0, .rowIdx = 0, .duration = 4.f / 60.f },
            { .colIdx = 1, .rowIdx = 0, .duration = 4.f / 60.f },
            { .colIdx = 2, .rowIdx = 0, .duration = 4.f / 60.f },
        });
    animation.AddAnimation(
        "walkUp"_h,
        spriteSheet,
        {
            { .colIdx = 6, .rowIdx = 0, .duration = 4.f / 60.f },
            { .colIdx = 7, .rowIdx = 0, .duration = 4.f / 60.f },
            { .colIdx = 8, .rowIdx = 0, .duration = 4.f / 60.f },
        });
    animation.AddAnimation(
        "walkLeft"_h,
        spriteSheet,
        {
            { .colIdx = 3, .rowIdx = 0, .duration = 4.f / 60.f },
            { .colIdx = 4, .rowIdx = 0, .duration = 4.f / 60.f },
            { .colIdx = 5, .rowIdx = 0, .duration = 4.f / 60.f },
        });
    animation.AddAnimation(
        "walkRight"_h,
        spriteSheet,
        {
            { .colIdx = 3, .rowIdx = 0, .duration = 4.f / 60.f, .flipX = true },
            { .colIdx = 4, .rowIdx = 0, .duration = 4.f / 60.f, .flipX = true },
            { .colIdx = 5, .rowIdx = 0, .duration = 4.f / 60.f, .flipX = true },
        });
    animation.AddAnimation(
        "pepperDown"_h,
        spriteSheet,
        {
            { .colIdx = 0, .rowIdx = 1, .duration = 0.f },
        });
    animation.AddAnimation(
        "pepperLeft"_h,
        spriteSheet,
        {
            { .colIdx = 1, .rowIdx = 1, .duration = 0.f },
        });
    animation.AddAnimation(
        "pepperRight"_h,
        spriteSheet,
        {
            { .colIdx = 1, .rowIdx = 1, .duration = 0.f, .flipX = true },
        });
    animation.AddAnimation(
        "pepperUp"_h,
        spriteSheet,
        {
            { .colIdx = 2, .rowIdx = 1, .duration = 0.f },
        });
    animation.AddAnimation(
        "death"_h,
        spriteSheet,
        {
            { .colIdx = 3, .rowIdx = 1, .duration = 0.5f },
            { .colIdx = 4, .rowIdx = 1, .duration = 1.0f },
            { .colIdx = 5, .rowIdx = 1, .duration = 0.15f },
            { .colIdx = 6, .rowIdx = 1, .duration = 0.15f },
        });
    animation.AddAnimation(
        "dying"_h,
        spriteSheet,
        {
            { .colIdx = 7, .rowIdx = 1, .duration = 0.15f },
            { .colIdx = 8, .rowIdx = 1, .duration = 0.15f },
        });

    animation.SetAnimation("idle"_h, true);
}


}  // namespace bt