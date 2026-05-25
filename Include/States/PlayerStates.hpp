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
struct Idle;
struct Walking;
struct Climbing;

// Some data that we want to be passed to the states
struct Context final
{
    glm::vec3 direction{};
    glm::vec3 position{};
    gla::Animation* animation{};
    Stage* stage{};
    PlayerController* playerController{};
};

using PlayerStateMachine = StateMachine<Idle, Context, Idle, Walking, Climbing>;

struct Idle final
{
    static void Update(PlayerStateMachine& machine, Context const& context);
    static void OnEnter(Context const& context);
    // void OnExit(Context const& context);
};

struct Walking final
{
    static void Update(PlayerStateMachine& machine, Context& context);
    static void OnEnter(Context const& context);
    // void OnExit(Context const& context);

private:
    static void ChangeAnimation(Context const& context);
};

struct Climbing final
{
    static void Update(PlayerStateMachine& machine, Context& context);
    static void OnEnter(Context const& context);
    // void OnExit(Context const& context);

private:
    static void ChangeAnimation(Context const& context);
};


}  // namespace bt::playerstates

#endif  // BURGERTIME_PLAYERSTATES_HPP
