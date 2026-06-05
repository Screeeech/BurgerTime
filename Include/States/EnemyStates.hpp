#ifndef BURGERTIME_ENEMYSTATES_HPP
#define BURGERTIME_ENEMYSTATES_HPP

#include <any>
#include <glm/vec2.hpp>

#include "StateMachine.hpp"

namespace gla
{
class Collider;
class Timer;
class Animation;
}  // namespace gla

namespace bt
{
class MoveComponent;
class Enemy;
class Stage;

namespace enemystates
{

struct IdleStanding;
struct Walking;
struct StunnedStanding;
struct IdleClimbing;
struct Climbing;
struct StunnedClimbing;
struct Falling;
struct Dying;

struct Context final
{
    gla::Animation& animation;
    gla::Timer& stunTimer;
    // Enemy* enemy{};
    MoveComponent& moveComponent;
    gla::Collider& hitBox;
    int entityIndex;
};

using EnemyStateMachine = StateMachine<Context, IdleStanding, Walking, Climbing, IdleClimbing, StunnedStanding, StunnedClimbing, Falling, Dying>;
using EnemyState = HelperState<Context, EnemyStateMachine>;

struct PepperEventState : EnemyState
{
    PepperEventState() = default;
    PepperEventState(PepperEventState const&) = default;
    PepperEventState(PepperEventState&&) = default;
    auto operator=(PepperEventState const&) -> PepperEventState& = default;
    auto operator=(PepperEventState&&) -> PepperEventState& = default;
    ~PepperEventState() override = default;

    int entityIndex{};
    gla::Animation* animation{};
    gla::Timer* stunTimer{};
    gla::Collider* hitBox{};

    virtual void OnEnter();
    virtual void OnExit();

private:
    virtual void OnPepper(std::any const& collisionEvent) = 0;
    // void OnSquish(std::any const& collisionEvent);
    // void OnDrop(std::any const& collisionEvent);
};

struct IdleStanding final : PepperEventState
{
    void OnEnter() override;
    void Update() override;
    // void OnExit() override;

private:
    void OnPepper(std::any const& collisionEvent) override;
};

struct Walking final : PepperEventState
{
    void Update() override;

private:
    void OnPepper(std::any const& collisionEvent) override;
};

struct Climbing final : PepperEventState
{
    void OnEnter() override;
    void Update() override;

private:
    void OnPepper(std::any const& collisionEvent) override;
};

struct IdleClimbing final : PepperEventState
{
    void OnEnter() override;
    void Update() override;

private:
    void OnPepper(std::any const& collisionEvent) override;
};

struct StunnedStanding final : EnemyState
{
    void OnEnter() const;
    void Update() override;
    void OnExit() const;
};

struct StunnedClimbing final : EnemyState
{
    void OnEnter() const;
    void Update() override;
    void OnExit() const;
};

struct Falling final : EnemyState
{
    void OnEnter() const;
    void Update() override;
    void OnExit() const;
};

struct Dying final : EnemyState
{
    void OnEnter() const;
    void Update() override;
};

}  // namespace enemystates
}  // namespace bt

#endif  // BURGERTIME_ENEMYSTATES_HPP
