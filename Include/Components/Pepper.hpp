#ifndef BURGERTIME_PEPPER_HPP
#define BURGERTIME_PEPPER_HPP
#include <glm/vec2.hpp>

#include "Component.hpp"

namespace gla
{
class CollisionRect;
class Timer;
class Animation;
}
namespace bt
{
class Entity;

class Pepper : public gla::Component
{
public:
    explicit Pepper(gla::GameObject* pOwner, Entity& player);
    ~Pepper() override;

    void SpawnPepper(glm::vec2 position, glm::vec2 direction) const;

protected:
    void Update() override;
    void OnActivate() override;

private:
    static constexpr float pepperDuration{ .75f };
    static constexpr float pepperCooldown{ 1.f };

    gla::Timer* m_pDurationTimer;
    gla::Timer* m_pCoolDownTimer;
    gla::Animation* m_pAnimation{};
    gla::CollisionRect* m_pHitbox{};
    Entity* m_pPlayer;
};

}  // namespace bt

#endif  // BURGERTIME_PEPPER_HPP
