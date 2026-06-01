#ifndef BURGERTIME_PLAYERSTATES_HPP
#define BURGERTIME_PLAYERSTATES_HPP
#include <glm/vec3.hpp>

#include "Components/Animation.hpp"
#include "StateMachine.hpp"

namespace bt
{
class PlayerController;
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
    glm::vec2 position{};
    gla::Animation* animation{};
    Stage* stage{};
    PlayerController* playerController{};
    float deltaTime{};
};

using PlayerStateMachine = StateMachine<StandingIdle, Context, StandingIdle, Walking, ClimbingIdle, Climbing, Dying>;

struct StandingIdle final
{
    float previousXDirection{};

    void OnEnter(Context const& context);
    void Update(PlayerStateMachine& machine, Context const& context) const;
    void OnExit(Context const& context);
    void OnPepper(std::any const& eventArgs);

private:
    void ChangeAnimation(Context const& context) const;
};

struct Walking final
{
    int wait{};
    float previousXDirection{};

    void OnEnter(Context const& context);
    void Update(PlayerStateMachine& machine, Context& context);
    void OnExit(Context const& context);
    void OnPepper(std::any const& eventArgs);

private:
    static void ChangeAnimation(Context const& context);
};

struct ClimbingIdle final
{
    float previousYDirection{};

    void OnEnter(Context const& context);
    void Update(PlayerStateMachine& machine, Context& context) const;
    void OnExit(Context const& context);
    void OnPepper(std::any const& eventArgs);

private:
    void ChangeAnimation(Context const& context) const;
};

struct Climbing final
{
    int wait{};
    float previousYDirection{};

    void OnEnter(Context const& context);
    void Update(PlayerStateMachine& machine, Context& context);
    void OnExit(Context const& context);
    void OnPepper(std::any const& eventArgs);

private:
    void ChangeAnimation(Context const& context) const;
};

struct Dying final
{
    float wait{};
    static constexpr float animationWait{ 1.8f };
    static constexpr float totalTime{ 6.f };

    void OnEnter(Context const& context);
    void Update(PlayerStateMachine& machine, Context const& context);
};


}  // namespace bt::playerstates

#endif  // BURGERTIME_PLAYERSTATES_HPP
