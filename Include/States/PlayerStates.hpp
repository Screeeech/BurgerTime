#ifndef BURGERTIME_PLAYERSTATES_HPP
#define BURGERTIME_PLAYERSTATES_HPP
#include <glm/vec3.hpp>

#include "Components/Animation.hpp"
#include "StateMachine.hpp"

namespace bt::playerstates
{
struct Idle;
struct Walking;
struct Climbing;

// Some data that we want to be passed to the states
struct Context
{
    float deltaTime{};
    glm::vec3 direction{};
    gla::Animation* animation{};
};

using PlayerStateMachine = StateMachine<Idle, Context, Idle, Walking, Climbing>;

struct Idle
{
    static void Update(PlayerStateMachine& machine, Context const& context);
    static void OnEnter(Context const& context);
    //void OnExit(Context const& context);
};

struct Walking
{
    static void Update(PlayerStateMachine& machine, Context const& context);
    static void OnEnter(Context const& context);
    //void OnExit(Context const& context);

private:
    static void ChangeAnimation(Context const& context);
};

struct Climbing
{
    static void Update(PlayerStateMachine& machine, Context const& context);
    //void OnEnter(Context const& context);
    //void OnExit(Context const& context);
};


}  // namespace bt::playerstates

#endif  // BURGERTIME_PLAYERSTATES_HPP
