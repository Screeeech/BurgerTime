#ifndef BURGERTIME_PLAYERSTATES_HPP
#define BURGERTIME_PLAYERSTATES_HPP
#include <any>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "StateMachine.hpp"

namespace gla
{
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

// Some data that we want to be passed to the states
struct Context final
{
    gla::Animation& animation;
    MoveComponent& moveComponent;
};

using PlayerStateMachine = StateMachine<Context, StandingIdle, Walking, ClimbingIdle, Climbing, Dying>;
using PlayerState = HelperState<Context, PlayerStateMachine>;

struct PepperableState : PlayerState
{
    virtual void OnEnter();
    virtual void OnExit();
    virtual void OnPepper(std::any const& eventArgs) = 0;
};

struct StandingIdle final : PepperableState
{
    void OnEnter() override;
    void Update() override;
    void OnExit() override;
    void OnPepper(std::any const& eventArgs) override;

private:
    void ChangeAnimation() const;
};

struct Walking final : PepperableState
{
    void Update() override;
    void OnPepper(std::any const& eventArgs) override;

private:
    void ChangeAnimation() const;
};

struct ClimbingIdle final : PepperableState
{
    void OnEnter() override;
    void Update() override;
    void OnPepper(std::any const& eventArgs) override;

private:
    void ChangeAnimation() const;
};

struct Climbing final : PepperableState
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
    static constexpr float animationWait{ 1.8f };
    static constexpr float totalTime{ 6.f };

    void OnEnter() const;
    void Update() override;
};


}  // namespace bt::playerstates

#endif  // BURGERTIME_PLAYERSTATES_HPP
