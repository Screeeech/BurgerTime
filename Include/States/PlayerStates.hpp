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

struct StandingIdle final : PlayerState
{
    float previousXDirection{};

    void OnEnter();
    void Update() override;
    void OnExit();
    void OnPepper(std::any const& eventArgs);

private:
    void ChangeAnimation(Context const& ctx) const;
};

struct Walking final : PlayerState
{
    float previousXDirection{};

    void OnEnter();
    void Update() override;
    void OnExit();
    void OnPepper(std::any const& eventArgs);

private:
    static void ChangeAnimation(Context const& ctx);
};

struct ClimbingIdle final : PlayerState
{
    float previousYDirection{};

    void OnEnter();
    void Update() override;
    void OnExit();
    void OnPepper(std::any const& eventArgs);

private:
    void ChangeAnimation(Context const& ctx) const;
};

struct Climbing final : PlayerState
{
    PlayerStateMachine* machine{};
    int wait{};
    float previousYDirection{};

    void OnEnter();
    void Update() override;
    void OnExit();
    void OnPepper(std::any const& eventArgs);

private:
    void ChangeAnimation(Context const& ctx) const;
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
