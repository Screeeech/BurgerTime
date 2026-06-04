#include "Components/BurgerPart.hpp"

#include <ranges>

#include "Components/Collider.hpp"
#include "Components/CollisionRect.hpp"
#include "Components/Enemy.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/Sprite.hpp"
#include "Components/Stage.hpp"
#include "Components/Timer.hpp"
#include "Constants.hpp"
#include "Services/EventManager.hpp"
#include "Utils.hpp"

namespace vw = std::ranges::views;

namespace bt
{

BurgerPart::BurgerPart(gla::GameObject* pOwner, Stage* pStage, Type pieceType, std::shared_ptr<gla::Texture2D> const& spriteSheetTexture)
    : Component(pOwner)
    //: Renderable(pOwner, 20)
    , m_pResetTimer(pOwner->AddComponent<gla::Timer>())
    , m_pieces(
          [&] -> Pieces
          {
              Pieces pieces{};
              for (auto const& [i, pair] : pieces | vw::enumerate)
              {
                  auto& [hitbox, sprite] = pair;

                  float const xOffset{ static_cast<float>(i) * pieceSize };

                  hitbox = pOwner->AddComponent<gla::CollisionRect>(
                      gla::Collider::Bits::Layer3 | gla::Collider::Bits::Layer4,
                      gla::Collider::Bits::Layer4 | gla::Collider::Bits::Layer5,
                      [this, i](gla::Collider&, gla::Collider&) -> void { OnPieceStep(i); },
                      glm::vec2{ xOffset, 0.f },
                      glm::vec2(pieceSize));
                  sprite = pOwner->AddComponent<gla::Sprite>(spriteSheetTexture, layers::burgerParts);
                  auto const srcRect = GetBurgerPieceSourceRect(pieceType, i);
                  sprite->SetSourceRect(srcRect);
                  sprite->m_offset.x = xOffset;
              }
              return pieces;
          }())
    , m_pStage(pStage)
    , m_stateMachine({ .part = *this, .transform = m_pOwner->GetTransform(), .timer = *m_pResetTimer, .stage = *m_pStage, .deltaTime = {} })
{
}

void BurgerPart::AcquireEnemy(gla::GameObject& enemyObject, Enemy& enemy)
{
    // Reparent the enemy to the burger so it falls along with the part
    enemyObject.QueueReparent(*m_pOwner);
    m_fallingEnemies.push_back(&enemy);
}

void BurgerPart::ReleaseEnemies()
{
    // Reparent to stage
    for (auto* enemy : m_fallingEnemies)
    {
        enemy->m_pOwner->QueueReparent(*m_pStage->m_pOwner);
        enemy->LandOnPlatform();
    }

    m_fallingEnemies.clear();
}

auto BurgerPart::GetSteppedPieces() const -> int
{
    return m_steppedPieces;
}

void BurgerPart::SetSteppedPieces(int steppedPieces)
{
    m_steppedPieces = std::min(steppedPieces, pieceCount);
}

auto BurgerPart::GetPieces() -> Pieces&
{
    return m_pieces;
}

void BurgerPart::FixedUpdate(float fixedDeltaTime)
{
    burgerpartstates::Context context{
        .part = *this,
        .transform = m_pOwner->GetTransform(),
        .timer = *m_pResetTimer,
        .stage = *m_pStage,
        .deltaTime = fixedDeltaTime,
    };

    m_stateMachine->Update(context);
}
// void BurgerPart::Render()
//{
//     auto const& renderer = gla::Locator::Get<gla::Renderer>();
//     auto const worldPos = m_pOwner->GetWorldPosition();
//
//     renderer.SetColor(colors::Blue);
//     renderer.DrawRect({ worldPos.x, worldPos.y, 1.f, 1.f });
// }

void BurgerPart::OnActivate() {}

void BurgerPart::OnDeactivate()
{
    Component::OnDeactivate();
}
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
    hitbox->m_collisionLayers &= ~gla::Collider::Bits::Layer3;

    sprite->m_offset.y = pieceStepOffset;
    ++m_steppedPieces;

    //// If burger collision
    // if (collider.m_collisionMasks & gla::Collider::Bits::Layer4)
    //{
    //     // Turn off burger collisions temporarily
    //     hitbox->m_collisionLayers &= ~gla::Collider::Bits::Layer4;
    // }
}


}  // namespace bt