#ifndef BURGERTIME_MOVECOMPONENT_HPP
#define BURGERTIME_MOVECOMPONENT_HPP
#include <glm/vec2.hpp>

#include "Component.hpp"

namespace bt
{
class Stage;

class MoveComponent final : public gla::Component
{
public:
    explicit MoveComponent(gla::GameObject* pOwner, float walkModifier = 1.f, float climbModifier = 1.f);

    void Climb() const;
    void Walk() const;
    void Move(glm::vec2 displacement) const;

    void SetDirection(glm::vec2 direction);
    auto GetDirection() const -> glm::vec2;
    auto GetSpritePosition() const -> glm::vec2;

    // Stage related helper functions
    void LockOntoGround() const;
    void LockOntoLadder() const;

    auto CanWalk() const -> bool;
    auto CanWalkLeft() const -> bool;
    auto CanWalkRight() const -> bool;
    auto CanClimbDown() const -> bool;
    auto CanClimbUp() const -> bool;
    auto IsOnGround() const -> bool;


protected:
    void LateUpdate() override;

    void OnActivate() override;

private:

    glm::vec2 m_direction{};
    Stage* m_pStage;
    float m_walkModifier{};
    float m_climbModifier{};
};

}  // namespace bt

#endif  // BURGERTIME_MOVECOMPONENT_HPP
