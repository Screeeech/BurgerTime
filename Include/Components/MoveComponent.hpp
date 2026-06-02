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
    explicit MoveComponent(gla::GameObject* pOwner, Stage* pStage);

    void Climb() const;
    void Walk() const;

    void SetDirection(glm::vec2 direction);
    auto GetDirection() const -> glm::vec2;
    auto GetSpritePosition() const -> glm::vec2;

    // Stage related helper functions

    void LockOntoGround() const;
    void LockOntoLadder() const;

    auto CanWalk() const -> bool;
    auto CanClimbDown() const -> bool;
    auto CanClimbUp() const -> bool;
    auto IsOnGround() const -> bool;


protected:
    void LateUpdate(float deltaTime) override;

private:
    void Move(glm::vec2 displacement) const;
    static constexpr glm::vec2 spriteFeetOffset{ 8.f, 15.f };

    glm::vec2 m_direction{};
    Stage* m_pStage;
};

}  // namespace bt

#endif  // BURGERTIME_MOVECOMPONENT_HPP
