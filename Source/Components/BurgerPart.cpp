#include "Components/BurgerPart.hpp"

#include <ranges>

#include "Components/Collider.hpp"
#include "Components/CollisionRect.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/Sprite.hpp"
#include "Components/Stage.hpp"
#include "Constants.hpp"

namespace vw = std::ranges::views;

namespace bt
{

BurgerPart::BurgerPart(gla::GameObject* pOwner, Stage* pStage, Piece pieceType, std::shared_ptr<gla::Texture2D> const& spriteSheetTexture)
    //: Component(pOwner)
    : Renderable(pOwner, 20)
    , m_pStage(pStage)
//, m_pMoveComponent(pOwner->AddComponent<MoveComponent>(pStage))
{
    for (auto const& [i, pair] : m_pieces | vw::enumerate)
    {
        auto& [hitbox, sprite] = pair;

        float const xOffset{ static_cast<float>(i) * pieceSize };

        hitbox = pOwner->AddComponent<gla::CollisionRect>(
            gla::Collider::Bits::Layer3 | gla::Collider::Bits::Layer4,
            gla::Collider::Bits::Layer4,
            std::vector<gla::CollisionCallback>{ [this, i](auto& collider) -> void { OnPieceStep(collider, i); } },
            glm::vec2{ xOffset, 0.f },
            glm::vec2(pieceSize));
        sprite = pOwner->AddComponent<gla::Sprite>(spriteSheetTexture, layers::burgerParts);
        auto srcRect = GetBurgerPieceSourceRect(pieceType, i);
        sprite->SetSourceRect(srcRect);
        sprite->m_offset.x = xOffset;
    }
}

void BurgerPart::FixedUpdate(float fixedDeltaTime)
{
    if (m_steppedPieces >= pieceCount)
    {
        m_pOwner->GetTransform().ChangeLocalPosition({ 0.f, fallingSpeed * fixedDeltaTime });
        if (IsOnPlatform())
        {
            LockOntoGround();

            m_steppedPieces = 0;
            for (auto const& [collider, sprite] : m_pieces)
            {
                collider->m_collisionLayers |= gla::Collider::Bits::Layer3;
                sprite->m_offset.y = 0;
            }
            std::println("Burger landed!");
        }
    }
}

void BurgerPart::Render()
{
    auto const& renderer = gla::Locator::Get<gla::Renderer>();
    auto const worldPos = m_pOwner->GetWorldPosition();

    renderer.SetColor(colors::Blue);
    renderer.DrawRect({ worldPos.x, worldPos.y, 1.f, 1.f });
}

auto BurgerPart::GetBurgerPieceSourceRect(Piece type, long index) -> SDL_FRect
{
    float const xIndex{ 14.f + static_cast<float>(index) };
    float const yIndex{ 6.f + static_cast<float>(type) };

    return {
        .x = pieceSize * xIndex,
        .y = pieceSize * yIndex,
        .w = pieceSize,
        .h = pieceSize,
    };
}

void BurgerPart::OnPieceStep(gla::Collider const& collider, long index)
{
    // If it's a burger hitting another burger
    if (collider.m_collisionMasks == gla::Collider::Bits::Layer4)
    {
        std::println("Burger Collision!");
    }
    else  // If it's the player stepping on a burger
    {
        auto& [hitbox, sprite] = m_pieces.at(index);

        // Turn off player stepping collisions
        hitbox->m_collisionLayers &= not gla::Collider::Bits::Layer3;

        sprite->m_offset.y = pieceStepOffset;

        ++m_steppedPieces;
        if (m_steppedPieces >= pieceCount)
            std::println("Burger falling!", index);
    }
}

auto BurgerPart::IsOnPlatform() const -> bool
{
    auto const worldPos = m_pOwner->GetLocalPosition();
    auto const bottomYPos = std::ceil(worldPos.y) + pieceSize;
    auto const mod = static_cast<int>(bottomYPos + 1) % static_cast<int>(Stage::tileHeight);
    if (mod == Stage::tileHeight - 2.f)
    {
        auto const tileType = m_pStage->GetTileAtPosition(glm::vec2{ worldPos.x, bottomYPos } + glm::vec2{ 0.f, 1.f });
        if (tileType == Stage::TileType::Platform or tileType == Stage::TileType::LadderPlatform)
            return true;
    }

    return false;
}

void BurgerPart::LockOntoGround() const
{
    auto const bottomBurgerPos = m_pOwner->GetLocalPosition() + glm::vec2{ 0.f, pieceSize };
    auto const yOffsetIntoTile = static_cast<int>(bottomBurgerPos.y) % static_cast<int>(Stage::tileHeight);
    auto const bump = Stage::tileHeight - static_cast<float>(yOffsetIntoTile) - 1;
    m_pOwner->GetTransform().SetLocalPosition({ bottomBurgerPos.x, bottomBurgerPos.y + bump - pieceSize });
}


}  // namespace bt