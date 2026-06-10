#include "Components/Plate.hpp"

#include <cassert>

#include "Components/BurgerPart.hpp"
#include "Components/CollisionRect.hpp"
#include "Constants.hpp"
#include "GameEvents.hpp"
#include "Services/EventManager.hpp"
#include "Services/Renderer.hpp"
#include "Utils.hpp"

namespace bt
{
class BurgerPart;

Plate::Plate(gla::GameObject* pOwner, int maxStackSize, int plateIndex)
    : Renderable(pOwner, layers::stage)
    , m_maxStackSize(maxStackSize * BurgerPart::pieceCount)
    , m_plateIndex(plateIndex)
    , m_pHitBox(pOwner->AddComponent<gla::CollisionRect>(
          gla::Collider::Bits::Layer7,
          0,
          [this](gla::Collider& collider, gla::Collider& otherCollider) -> void
          {
              m_pieceCount++;
              if (m_pieceCount % BurgerPart::pieceCount == 0)
              {
                  auto* burgerPart = otherCollider.m_pOwner->GetComponent<BurgerPart>();
                  assert(burgerPart and "otherCollider must have a BurgerPart component");

                  burgerPart->SettleOntoPlate(m_pieceCount / BurgerPart::pieceCount, m_pOwner->GetLocalPosition() + glm::vec2(0.f, -1.f));

                  // Move plate collider up for the next burger part
                  auto& collisionRect = static_cast<gla::CollisionRect&>(collider);
                  collisionRect.m_position.y -= BurgerPart::pieceSize + 1.f;
              }

              // Disable burger part collider so it doesn't trigger multiple times
              otherCollider.DisableCollisionMasks(gla::Collider::Bits::Layer7);

              if (m_pieceCount >= m_maxStackSize)
              {
                  gla::Locator::Get<gla::EventManager>().InvokeEvent(PlateFinishedEvent("PlateFinished"_h, m_plateIndex));
                  collider.Disable();
              }
          },
          glm::vec2{ -BurgerPart::pieceSize / 2.f, BurgerPart::pieceSize },
          glm::vec2{ BurgerPart::pieceSize * 5.f, 4.f }))
{
}

void Plate::Render()
{
    auto const& renderer = gla::Locator::Get<gla::Renderer>();

    auto const pos = m_pOwner->GetWorldPosition();
    auto constexpr offset = BurgerPart::pieceSize;

    renderer.SetColor(colors::PlatformColor);
    renderer.DrawLines(
        {
            {
                pos + glm::vec2(-(offset / 2.f) - 1.f, -offset / 4.f),
                pos + glm::vec2(-1.f, offset),
            },
            {
                pos + glm::vec2(-1.f, offset),
                pos + glm::vec2((offset * 4.f) + 1.f, offset),
            },
            {
                pos + glm::vec2((offset * 4.f) + 1.f, offset),
                pos + glm::vec2((offset * 4.f) + (offset / 2.f), -offset / 4.f),
            },
        });
}

}  // namespace bt