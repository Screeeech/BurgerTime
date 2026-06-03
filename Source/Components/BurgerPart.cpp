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

BurgerPart::BurgerPart(gla::GameObject* pOwner, Stage* pStage, Type pieceType, std::shared_ptr<gla::Texture2D> const& spriteSheetTexture)
    : Component(pOwner)
    //: Renderable(pOwner, 20)
    , m_stateMachine(
          burgerpartstates::Context{
              .transform = m_pOwner->GetTransform(),
              .steppedPieces = m_steppedPieces,
              .stage = *pStage,
          })
    , m_pStage(pStage)
{
    for (auto const& [i, pair] : m_pieces | vw::enumerate)
    {
        auto& [hitbox, sprite] = pair;

        float const xOffset{ static_cast<float>(i) * pieceSize };

        hitbox = pOwner->AddComponent<gla::CollisionRect>(
            gla::Collider::Bits::Layer3 | gla::Collider::Bits::Layer4,
            gla::Collider::Bits::Layer4,
            std::vector<gla::CollisionCallback>{ [this, i](auto&) -> void { OnPieceStep(i); } },
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
    burgerpartstates::Context context{
        .transform = m_pOwner->GetTransform(),
        .steppedPieces = m_steppedPieces,
        .deltaTime = fixedDeltaTime,
        .stage = *m_pStage,
        .pieces = &m_pieces,
    };

    m_stateMachine.Update(context);
}

//void BurgerPart::Render()
//{
//    auto const& renderer = gla::Locator::Get<gla::Renderer>();
//    auto const worldPos = m_pOwner->GetWorldPosition();
//
//    renderer.SetColor(colors::Blue);
//    renderer.DrawRect({ worldPos.x, worldPos.y, 1.f, 1.f });
//}

auto BurgerPart::GetBurgerPieceSourceRect(Type type, long index) -> SDL_FRect
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

    // Turn off player stepping collisions
    hitbox->m_collisionLayers &= not gla::Collider::Bits::Layer3;

    sprite->m_offset.y = pieceStepOffset;
    ++m_steppedPieces;
}


}  // namespace bt