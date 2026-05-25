#ifndef BURGERTIME_ENEMY_HPP
#define BURGERTIME_ENEMY_HPP
#include "Component.hpp"

namespace bt
{

class Enemy final : public gla::Component
{
public:
    explicit Enemy(gla::GameObject* pOwner);

protected:
    void Update(float deltaTime) override;
};

}  // namespace bt

#endif  // BURGERTIME_ENEMY_HPP
