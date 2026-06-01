#ifndef BURGERTIME_PLAYERCONTROLLER_H
#define BURGERTIME_PLAYERCONTROLLER_H

#include <glm/vec3.hpp>

#include "Component.hpp"
#include "StateMachine.hpp"
#include "States/PlayerStates.hpp"

namespace gla
{
class Collider;
class CollisionRect;
struct Event;
}  // namespace gla

namespace bt
{
class Pepper;

class PlayerController final : public gla::Renderable
{
public:
    explicit PlayerController(gla::GameObject* pPlayer, Stage* stage, Pepper* pepper, int playerIndex);

    void SetDirection(glm::vec2 direction);
    [[nodiscard]] auto GetDirection() const -> glm::vec2;

    void Move(glm::vec2 displacement) const;
    void OnHit(gla::Collider const& collider) const;
    // void OnPepper(std::any const& eventArgs) const;

    int const m_playerIndex;

protected:
    void Render() override;
    void FixedUpdate(float deltaTime) override;

    void OnActivate() override;
    void OnDeactivate() override;

private:
    static constexpr glm::vec2 spriteFeetOffset{ 8.f, 15.f };


    Stage* m_pStage;
    Pepper* m_pPepper;
    gla::Animation* m_pAnimation;
    gla::CollisionRect* m_pHitBox{};
    gla::CollisionRect* m_pHurtBox{};

    glm::vec2 m_direction{};
    // static float constexpr m_speed{ 1.f };

    playerstates::PlayerStateMachine m_finiteStateMachine;
};

}  // namespace bt

#endif  // BURGERTIME_PLAYERCONTROLLER_H
