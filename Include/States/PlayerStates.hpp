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
}
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
    glm::vec2 direction{};
    gla::Animation* animation{};
    MoveComponent* moveComponent{};
    float deltaTime{};
};

using PlayerStateMachine = StateMachine<Context, StandingIdle, Walking, ClimbingIdle, Climbing, Dying>;

struct StandingIdle final
{
    PlayerStateMachine* machine{};
    float previousXDirection{};

    void OnEnter(Context const& context);
    void Update(Context const& context) const;
    void OnExit(Context const& context);
    void OnPepper(std::any const& eventArgs);

private:
    void ChangeAnimation(Context const& context) const;
};

struct Walking final
{
    PlayerStateMachine* machine{};
    float previousXDirection{};

    void OnEnter(Context const& context);
    void Update(Context& context);
    void OnExit(Context const& context);
    void OnPepper(std::any const& eventArgs);

private:
    static void ChangeAnimation(Context const& context);
};

struct ClimbingIdle final
{
    PlayerStateMachine* machine{};
    float previousYDirection{};

    void OnEnter(Context const& context);
    void Update(Context& context) const;
    void OnExit(Context const& context);
    void OnPepper(std::any const& eventArgs);

private:
    void ChangeAnimation(Context const& context) const;
};

struct Climbing final
{
    PlayerStateMachine* machine{};
    int wait{};
    float previousYDirection{};

    void OnEnter(Context const& context);
    void Update(Context& context);
    void OnExit(Context const& context);
    void OnPepper(std::any const& eventArgs);

private:
    void ChangeAnimation(Context const& context) const;
};

struct Dying final
{
    PlayerStateMachine* machine{};
    float wait{};
    static constexpr float animationWait{ 1.8f };
    static constexpr float totalTime{ 6.f };

    static void OnEnter(Context const& context);
    void Update(Context const& context);
};


}  // namespace bt::playerstates

#endif  // BURGERTIME_PLAYERSTATES_HPP
