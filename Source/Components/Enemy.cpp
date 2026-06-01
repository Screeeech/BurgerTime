#include "Components/Enemy.hpp"

#include <print>

#include "GameObject.hpp"
#include "Components/CollisionRect.hpp"

namespace bt
{

Enemy::Enemy(gla::GameObject* pOwner)
    : Component(pOwner)
    , m_pHitBox(pOwner->AddComponent<gla::CollisionRect>(
          static_cast<uint32_t>(gla::Collider::Bits::Layer1),
          static_cast<uint32_t>(gla::Collider::Bits::Layer2),
          std::vector<gla::CollisionCallback>{ [](auto&) { std::println("Enemy hit!"); } },
          glm::vec2{},
          glm::vec2{ 16.f, 16.f }))


{
}

}  // namespace bt