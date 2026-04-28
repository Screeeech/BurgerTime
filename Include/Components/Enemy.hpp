#ifndef BURGERTIME_ENEMY_HPP
#define BURGERTIME_ENEMY_HPP
#include "Component.hpp"

namespace bt
{

class Enemy final : public gla::Component
{
public:
    explicit Enemy(gla::GameObject* pOwner);

    Enemy(Enemy const&) = delete;
    auto operator=(Enemy const&) -> Enemy& = delete;
    Enemy(Enemy&&) = delete;
    auto operator=(Enemy&&) -> Enemy& = delete;

    void Update(float deltaTime) override;
};

}  // namespace bt

#endif  // BURGERTIME_ENEMY_HPP
