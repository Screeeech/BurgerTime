#ifndef BURGERTIME_ENEMYSTATES_HPP
#define BURGERTIME_ENEMYSTATES_HPP

#include <any>
#include <glm/vec2.hpp>

#include "Components/Entity.hpp"
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
struct Disabled;
struct Spawning;

struct Context final
{
    gla::Animation& animation;
    gla::Timer& stunTimer;
    gla::Timer& aiDelayTimer;
    MoveComponent& moveComponent;
    gla::Collider& playerHitbox;
    gla::Collider& headBurtBox;
    gla::Collider& feetHurtBox;
    int entityIndex;
    Entity::Type type;
    glm::vec2 initialWalkingDirection;
    bool disabled{};
};

using EnemyStateMachine =
    StateMachine<Context, Spawning, Disabled, IdleStanding, Walking, Climbing, IdleClimbing, StunnedStanding, StunnedClimbing, Falling, Dying>;
using EnemyState = HelperState<Context, EnemyStateMachine>;

struct EnemyActiveState : EnemyState
{
    virtual void OnEnter();
    virtual void OnExit();

private:
    virtual void OnPepper(std::any const& collisionEvent) = 0;
    virtual void OnDisable(std::any const& eventArgs);
};

struct IdleStanding final : EnemyActiveState
{
    bool nextFrame{};

    void OnEnter() override;
    void Update() override;
    // void OnExit() override;

private:
    void OnPepper(std::any const& playerEvent) override;
};

struct Walking final : EnemyActiveState
{
    void Update() override;

private:
    void OnPepper(std::any const& playerEvent) override;
};

struct Climbing final : EnemyActiveState
{
    void OnEnter() override;
    void Update() override;

private:
    void OnPepper(std::any const& playerEvent) override;
};

struct IdleClimbing final : EnemyActiveState
{
    void OnEnter() override;
    void Update() override;

private:
    void OnPepper(std::any const& playerEvent) override;
};

struct StunnedStanding final : EnemyState
{
    void OnEnter();
    void Update() override;
    void OnExit() const;

private:
    void OnDisable(std::any const& eventArgs) const;
};

struct StunnedClimbing final : EnemyState
{
    void OnEnter();
    void Update() override;
    void OnExit() const;

private:
    void OnDisable(std::any const& eventArgs) const;
};

struct Falling final : EnemyState
{
    void OnEnter();
    void Update() override;
    void OnExit() const;
private:
    void OnLanding(std::any const& playerEvent) const;
    void OnPlate(std::any const& playerEvent) const;
    void OnDisable(std::any const& eventArgs) const;
};

struct Dying final : EnemyState
{
    void OnEnter() const;
    void Update() override;
};

struct Disabled final : EnemyState
{
    void OnEnter();
    void Update() override;
    void OnExit();

private:
    void OnEnable(std::any const& eventArgs) const;
};

struct Spawning final : EnemyActiveState
{
    void OnEnter() override;
    void Update() override;

private:
    void OnPepper(std::any const& collisionEvent) override;
};

}  // namespace enemystates
}  // namespace bt

#endif  // BURGERTIME_ENEMYSTATES_HPP
