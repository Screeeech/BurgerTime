#ifndef BURGERTIME_PLAYERSTATES_HPP
#define BURGERTIME_PLAYERSTATES_HPP
#include <glm/vec3.hpp>

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
};

using PlayerStateMachine = StateMachine<Idle, Context, Idle, Walking, Climbing>;

struct Idle
{
    void Update(PlayerStateMachine& machine, Context const& context);
    void OnEnter();
    void OnExit();
};

struct Walking
{
    void Update(PlayerStateMachine& machine, Context const& context);
    void OnEnter();
    void OnExit();
};

struct Climbing
{
    void Update(PlayerStateMachine& machine, Context const& context);
    void OnEnter();
    void OnExit();
};


}  // namespace bt::playerstates

#endif  // BURGERTIME_PLAYERSTATES_HPP
