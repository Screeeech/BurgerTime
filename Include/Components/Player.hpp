#ifndef BURGERTIME_PLAYERCONTROLLER_H
#define BURGERTIME_PLAYERCONTROLLER_H

#include <glm/vec3.hpp>

#include "Components/Animation.hpp"
#include "StateMachine.hpp"
#include "States/PlayerStates.hpp"

namespace gla
{
class Timer;
class Collider;
class CollisionRect;
struct Event;
}  // namespace gla

namespace bt
{
class MoveComponent;
class Pepper;

class Player final : public gla::Component
{
public:
    explicit Player(gla::GameObject* pPlayer, Stage* pStage, Pepper* pPepper, int playerIndex);

    void OnDamage(gla::Collider const& collider, gla::Collider const& other);

    int const m_playerIndex;

protected:
    void FixedUpdate(float deltaTime) override;

    void OnActivate() override;
    void OnDeactivate() override;

private:
    static constexpr float hitDelay{ 3.f };

    Pepper* m_pPepper;
    MoveComponent* m_pMoveComponent;
    gla::Animation* m_pAnimation;
    gla::CollisionRect* m_pHitBox{};
    //gla::Timer* m_pTime{};

    playerstates::PlayerStateMachine m_finiteStateMachine;

public:
    static void DefineAnimations(gla::Animation& animation, std::shared_ptr<gla::Texture2D> const& spriteSheetTexture);
};

}  // namespace bt

#endif  // BURGERTIME_PLAYERCONTROLLER_H
