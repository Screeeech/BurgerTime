#include "Components/BurgerPart.hpp"

#include <ranges>

#include "Constants.hpp"
#include "Components/Collider.hpp"
#include "Components/CollisionRect.hpp"
#include "Components/Sprite.hpp"

namespace vw = std::ranges::views;

namespace bt
{

BurgerPart::BurgerPart(gla::GameObject* pOwner, Piece pieceType, std::shared_ptr<gla::Texture2D> const& spriteSheetTexture)
    : Component(pOwner)
{
    for (auto const& [i, pair] : m_pieces | vw::enumerate)
    {
        auto& [hitbox, sprite] = pair;

        float const xOffset{ static_cast<float>(i) * pieceSize };

        hitbox = pOwner->AddComponent<gla::CollisionRect>(
            static_cast<uint32_t>(gla::Collider::Bits::Layer3),
            0,
            std::vector<gla::CollisionCallback>{ [this, i](auto&) -> void { OnPieceStep(i); } },
            glm::vec2{ xOffset, 0.f },
            glm::vec2(pieceSize));
        sprite = pOwner->AddComponent<gla::Sprite>(spriteSheetTexture, layers::burgerParts);
        auto srcRect = GetBurgerPieceSourceRect(pieceType, i);
        sprite->SetSourceRect(srcRect);
        sprite->m_offset.x = xOffset;
    }
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

void BurgerPart::OnPieceStep(long index)
{
    auto& [hitbox, sprite] = m_pieces.at(index);

    hitbox->Disable();
    sprite->m_offset.y = pieceStepOffset;

    ++m_steppedPieces;
    if (m_steppedPieces >= pieceCount)
        std::println("Stepped on piece {}", index);
}


}  // namespace bt