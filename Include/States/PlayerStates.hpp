#ifndef BURGERTIME_PLAYERSTATES_HPP
#define BURGERTIME_PLAYERSTATES_HPP

#include <any>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

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
class Stage;
}  // namespace bt

namespace bt::playerstates
{
struct Dying;
struct StandingIdle;
struct Walking;
struct ClimbingIdle;
struct Climbing;
struct Disabled;

// Some data that we want to be passed to the states
struct Context final
{
    gla::Animation& animation;
    gla::Timer& pepperTimer;
    gla::Collider& collider;
    MoveComponent& moveComponent;
    int playerIndex;
    glm::vec2 previousDirection{ 1, 1 };
};

using PlayerStateMachine = StateMachine<Context, Disabled, StandingIdle, Walking, ClimbingIdle, Climbing, Dying>;
using PlayerState = HelperState<Context, PlayerStateMachine>;

struct PlayerActiveState : PlayerState
{
    virtual void OnEnter();
    virtual void OnExit();
    virtual void OnPepper(std::any const& eventArgs) = 0;
    virtual void OnDisable(std::any const& eventArgs);
};

struct StandingIdle final : PlayerActiveState
{
    void OnEnter() override;
    void Update() override;
    void OnExit() override;
    void OnPepper(std::any const& eventArgs) override;

private:
    void ChangeAnimation() const;
};

struct Walking final : PlayerActiveState
{
    void Update() override;
    void OnPepper(std::any const& eventArgs) override;

private:
    void ChangeAnimation() const;
};

struct ClimbingIdle final : PlayerActiveState
{
    void OnEnter() override;
    void Update() override;
    void OnPepper(std::any const& eventArgs) override;

private:
    void ChangeAnimation() const;
};

struct Climbing final : PlayerActiveState
{
    void OnEnter() override;
    void Update() override;
    void OnPepper(std::any const& eventArgs) override;

private:
    void ChangeAnimation() const;
};

struct Dying final : PlayerState
{
    float wait{};

    void OnEnter() const;
    void Update() override;
};

struct Disabled final : PlayerState
{
    void OnEnter();
    void Update() override;
    void OnExit();

private:
    void OnEnable(std::any const& eventArgs) const;
};

}  // namespace bt::playerstates

#endif  // BURGERTIME_PLAYERSTATES_HPP
