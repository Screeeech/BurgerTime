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

class Pepper : public gla::Component
{
public:
    explicit Pepper(gla::GameObject* pOwner, int zIndex = 0);
    ~Pepper() noexcept override = default;

    void SpawnPepper(glm::vec2 position, glm::vec2 direction) const;

protected:
    void Update(float deltaTime) override;

private:
    static constexpr float pepperDuration{ .75f };
    gla::Timer* m_pTimer;
    gla::Animation* m_pAnimation{};
    gla::CollisionRect* m_pHitbox{};
};

}  // namespace bt

#endif  // BURGERTIME_PEPPER_HPP
