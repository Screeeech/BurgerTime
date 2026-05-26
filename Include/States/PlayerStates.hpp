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
};

using PlayerStateMachine = StateMachine<StandingIdle, Context, StandingIdle, Walking, ClimbingIdle, Climbing>;

struct StandingIdle final
{
    static void Update(PlayerStateMachine& machine, Context const& context);
    static void OnEnter(Context const& context);
    // void OnExit(Context const& context);
};

struct Walking final
{
    int wait{};
    void Update(PlayerStateMachine& machine, Context& context);
    static void OnEnter(Context const& context);
    // void OnExit(Context const& context);

private:
    static void ChangeAnimation(Context const& context);
};

struct ClimbingIdle final
{
    static void OnEnter(Context const& context);
    static void Update(PlayerStateMachine& machine, Context& context);
};

struct Climbing final
{
    bool firstClimb{ true };
    int wait{};
    float previousYDirection{};
    void OnEnter(Context const& context);
    void Update(PlayerStateMachine& machine, Context& context);
    // void OnExit(Context const& context);

private:
    static void ChangeAnimation(Context const& context);
};


}  // namespace bt::playerstates

#endif  // BURGERTIME_PLAYERSTATES_HPP
