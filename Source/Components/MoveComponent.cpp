#include "Components/MoveComponent.hpp"

#include <algorithm>

#include "Components/Stage.hpp"
#include "GameObject.hpp"

namespace bt
{

MoveComponent::MoveComponent(gla::GameObject* pOwner, Stage* pStage, float walkModifier, float climbModifier)
    : Component(pOwner)
    , m_pStage(pStage)
    , m_walkModifier(walkModifier)
    , m_climbModifier(climbModifier)
{
}

void MoveComponent::Climb() const
{
    m_pOwner->GetTransform().ChangeLocalPosition(0.f, m_direction.y * m_climbModifier);
}

void MoveComponent::Walk() const
{
    m_pOwner->GetTransform().ChangeLocalPosition(m_direction.x * m_walkModifier, 0.f);
}

void MoveComponent::Move(glm::vec2 displacement) const
{
    m_pOwner->GetTransform().ChangeLocalPosition(displacement);
}

void MoveComponent::SetDirection(glm::vec2 direction)
{
    m_direction.x = std::clamp(m_direction.x + direction.x, -1.f, 1.f);
    m_direction.y = std::clamp(m_direction.y + direction.y, -1.f, 1.f);
}

auto MoveComponent::GetDirection() const -> glm::vec2
{
    return m_direction;
}

auto MoveComponent::GetSpritePosition() const -> glm::vec2
{
    return m_pOwner->GetLocalPosition() + spriteFeetOffset;
}

void MoveComponent::LockOntoGround() const
{
    static constexpr int groundOffset{ 13 };

    auto const yOffsetIntoTile = static_cast<int>(GetSpritePosition().y) % static_cast<int>(Stage::tileHeight);
    auto const bump = static_cast<float>(groundOffset - yOffsetIntoTile);
    Move({ 0.f, bump });
}

void MoveComponent::LockOntoLadder() const
{
    static constexpr int ladderOffset{ 7 };

    auto const xOffsetIntoTile = static_cast<int>(GetSpritePosition().x) % static_cast<int>(Stage::tileWidth);
    auto const bump = static_cast<float>(ladderOffset - xOffsetIntoTile);
    Move({ bump, 0.f });
}

auto MoveComponent::CanWalk() const -> bool
{
    float constexpr halfPlayerSprite{ 8.f };

    glm::vec2 xSideOffset{};
    if (m_direction.x > 0)
        xSideOffset.x = halfPlayerSprite * 2;
    else if (m_direction.x < 0)
        xSideOffset.x = -halfPlayerSprite;

    Stage::TileType const tile = m_pStage->GetTileAtPosition(GetSpritePosition() + xSideOffset);
    return tile == Stage::TileType::Platform or tile == Stage::TileType::LadderPlatform;
}

auto MoveComponent::CanClimbDown() const -> bool
{
    int const xOffsetIntoTile = static_cast<int>(GetSpritePosition().x) % static_cast<int>(Stage::tileWidth);

    // Within this margin we are over a ladder
    if (xOffsetIntoTile < 6 or xOffsetIntoTile > 9)
        return false;

    switch (m_pStage->GetTileAtPosition(GetSpritePosition() - glm::vec2(0.f, -4.f)))
    {
        case Stage::TileType::Ladder:
            return true;
        case Stage::TileType::LadderPlatform:
            return true;
        default:
            return false;
    }
}

auto MoveComponent::CanClimbUp() const -> bool
{
    int const xOffsetIntoTile = static_cast<int>(GetSpritePosition().x) % static_cast<int>(Stage::tileWidth);

    // Within this margin we are over a ladder
    if (xOffsetIntoTile < 6 or xOffsetIntoTile > 9)
        return false;

    switch (m_pStage->GetTileAtPosition(GetSpritePosition()))
    {
        case Stage::TileType::Ladder:
            return true;
        case Stage::TileType::LadderPlatform:
            return true;
        default:
            return false;
    }
}

auto MoveComponent::IsOnGround() const -> bool
{
    int const yOffsetIntoTile = static_cast<int>(GetSpritePosition().y) % static_cast<int>(Stage::tileHeight);
    if (yOffsetIntoTile < 13 or yOffsetIntoTile > 17)
        return false;

    // Check for tile below player
    switch (m_pStage->GetTileAtPosition(GetSpritePosition() - glm::vec2(0.f, 1.f)))
    {
        case Stage::TileType::Platform:
            return true;
        case Stage::TileType::LadderPlatform:
            return true;
        default:
            return false;
    }
}


void MoveComponent::LateUpdate()
{
    m_direction = {};
}

}  // namespace bt