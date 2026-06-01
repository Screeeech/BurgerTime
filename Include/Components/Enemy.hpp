#ifndef BURGERTIME_ENEMY_HPP
#define BURGERTIME_ENEMY_HPP
#include "Component.hpp"

namespace gla
{
class CollisionRect;
}
namespace bt
{

class Enemy final : public gla::Component
{
public:
    explicit Enemy(gla::GameObject* pOwner);

protected:

private:
    gla::CollisionRect* m_pHitBox;
};

}  // namespace bt

#endif  // BURGERTIME_ENEMY_HPP
