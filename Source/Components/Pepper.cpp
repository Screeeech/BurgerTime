#include "Components/Pepper.hpp"

#include "Components/Animation.hpp"
#include "Components/Sprite.hpp"
#include "Services/ResourceManager.hpp"
#include "Utils.hpp"

namespace bt
{

Pepper::Pepper(gla::GameObject* pOwner, int zIndex)
    : Renderable(pOwner, zIndex)
    , m_pAnimation(pOwner->AddComponent<gla::Animation>(zIndex))
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
            { .colIdx = 12, .rowIdx = 1 },
        });
    m_pAnimation->AddAnimation(
        "pepperDustRight"_h,
        spriteSheet,
        {
            { .colIdx = 12, .rowIdx = 2, .flipX = true },
        });

    m_pAnimation->SetAnimation("pepperDustNone"_h);
}

void Pepper::SpawnPepper(glm::vec2 position, glm::vec2 direction)
{
    m_duration = 1.f;
    m_pOwner->GetTransform().SetLocalPosition(position);
    if (direction == glm::vec2{ 0, -1 })  // Up
        m_pAnimation->SetAnimation("pepperDustUp"_h, true);
    if (direction == glm::vec2{ 0, 1 })  // Down
        m_pAnimation->SetAnimation("pepperDustDown"_h, true);
    if (direction == glm::vec2{ -1, 0 })  // Left
        m_pAnimation->SetAnimation("pepperDustLeft"_h, true);
    if (direction == glm::vec2{ 1, 0 })  // Right
        m_pAnimation->SetAnimation("pepperDustRight"_h, true);
}

void Pepper::Render() {}

void Pepper::FixedUpdate(float deltaTime)
{
    if (m_duration > 0.f)
    {
        m_duration -= deltaTime;
        if (m_duration <= 0.f)
            m_pAnimation->SetAnimation("pepperDustNone"_h);
    }
}

void Pepper::OnActivate()
{
    Renderable::OnActivate();
}

void Pepper::OnDeactivate()
{
    Renderable::OnDeactivate();
}

}  // namespace bt