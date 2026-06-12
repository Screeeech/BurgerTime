#include "Components/Pepper.hpp"

#include "Commands/CallbackCommand.hpp"
#include "Components/Animation.hpp"
#include "Components/Collider.hpp"
#include "Components/CollisionRect.hpp"
#include "Components/Entity.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/Sprite.hpp"
#include "Components/Timer.hpp"
#include "Constants.hpp"
#include "GameEvents.hpp"
#include "Services/EventManager.hpp"
#include "Services/InputManager.hpp"
#include "Services/Renderer.hpp"
#include "Services/ResourceManager.hpp"
#include "Services/Sound.hpp"
#include "Utils.hpp"

namespace bt
{

Pepper::Pepper(gla::GameObject* pOwner, Entity& player)
    : Component(pOwner)
    , m_pDurationTimer(pOwner->AddComponent<gla::Timer>())
    , m_pCoolDownTimer(pOwner->AddComponent<gla::Timer>())
    , m_pAnimation(pOwner->AddComponent<gla::Animation>(layers::pepper))
    , m_pHitbox(pOwner->AddComponent<gla::CollisionRect>(
          0, gla::Collider::Bits::Layer1, gla::CollisionCallback{}, glm::vec2{}, glm::vec2{ 16.f, 16.f }, false))
    , m_pPlayer(&player)
{
    auto const spriteSheetTexture{ gla::Locator::Get<gla::ResourceManager>().LoadTexture("Textures/spritesheet.png") };

    auto const size{ spriteSheetTexture->GetSize() };
    auto const cols{ static_cast<int>(size.x / 16.f) };
    auto const rows{ static_cast<int>(size.y / 16.f) };
    auto& spriteSheet{ m_pAnimation->AddSpriteSheet(spriteSheetTexture, cols, rows) };

    m_pAnimation->AddAnimation(
        "pepperDustNone"_h,
        spriteSheet,
        {
            { .colIdx = 10, .rowIdx = 0 },
        });
    m_pAnimation->AddAnimation(
        "pepperDustUp"_h,
        spriteSheet,
        {
            { .colIdx = 12, .rowIdx = 2, .flipY = true },
        });
    m_pAnimation->AddAnimation(
        "pepperDustDown"_h,
        spriteSheet,
        {
            { .colIdx = 12, .rowIdx = 2 },
        });
    m_pAnimation->AddAnimation(
        "pepperDustLeft"_h,
        spriteSheet,
        {
            { .colIdx = 12, .rowIdx = 1, .flipY = true },
        });
    m_pAnimation->AddAnimation(
        "pepperDustRight"_h,
        spriteSheet,
        {
            { .colIdx = 12, .rowIdx = 2, .flipX = true },
        });

    m_pAnimation->SetAnimation("pepperDustNone"_h);
}

void Pepper::SpawnPepper(glm::vec2 position, glm::vec2 direction) const
{
    gla::Locator::Get<gla::Sound>().PlayAudio("pepper_shake"_h);
    gla::Locator::Get<gla::EventManager>().InvokeEvent(gla::Event{"PepperAttack"_h});

    m_pDurationTimer->Start(pepperDuration);
    m_pHitbox->Enable();

    if (direction == glm::vec2{ 0, -1 })  // Up
    {
        m_pOwner->GetTransform().SetLocalPosition(position + glm::vec2(-8, -32));
        m_pAnimation->SetAnimation("pepperDustUp"_h, true);
    }
    else if (direction == glm::vec2{ 0, 1 })  // Down
    {
        m_pOwner->GetTransform().SetLocalPosition(position + glm::vec2(-8, 0));
        m_pAnimation->SetAnimation("pepperDustDown"_h, true);
    }
    else if (direction == glm::vec2{ -1, 0 })  // Left
    {
        m_pOwner->GetTransform().SetLocalPosition(position + glm::vec2(-26, -18));
        m_pAnimation->SetAnimation("pepperDustLeft"_h, true);
    }
    else  // Right
    {
        m_pOwner->GetTransform().SetLocalPosition(position + glm::vec2(12, -18));
        m_pAnimation->SetAnimation("pepperDustRight"_h, true);
    }
}

void Pepper::Update()
{
    if (m_pDurationTimer->IsFinished())
    {
        m_pAnimation->SetAnimation("pepperDustNone"_h);
        m_pHitbox->Disable();
    }
}

void Pepper::OnActivate()
{
    auto& inputManager = gla::Locator::Get<gla::InputManager>();
    inputManager.BindAction<gla::CallbackCommand>(
        "attack"_h,
        m_pPlayer->entityIndex,
        [this] -> void
        {
            auto const* pMoveComponent = m_pPlayer->m_pOwner->GetComponent<MoveComponent>();

            // TODO: Move the cooldown timer to a different place
            if (m_pCoolDownTimer->IsRunning())
                return;
            m_pCoolDownTimer->Start(pepperCooldown);

            gla::Locator::Get<gla::EventManager>().InvokeEvent(
                PepperEvent("TryPepper"_h, m_pPlayer->entityIndex, pMoveComponent->GetDirection(), pMoveComponent->GetSpritePosition(), this));
        });
}

void Pepper::OnDeactivate()
{
    auto& inputManager = gla::Locator::Get<gla::InputManager>();
    inputManager.UnbindAction("attack"_h, m_pPlayer->entityIndex);
}

}  // namespace bt