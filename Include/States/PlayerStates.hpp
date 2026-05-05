#ifndef BURGERTIME_PLAYERSTATES_HPP
#define BURGERTIME_PLAYERSTATES_HPP
#include "StateMachine.hpp"

namespace bt::playerstates
{
struct Idle;
struct Walking;
struct Climbing;

using PlayerStateMachine = StateMachine<Idle, Walking, Climbing>;

struct Idle
{
    void Update(PlayerStateMachine& machine);
    void OnEnter();
    void OnExit();
};

struct Walking
{
    void Update(PlayerStateMachine& machine);
    void OnEnter();
    void OnExit();
};

struct Climbing
{
    void Update(PlayerStateMachine& machine);
    void OnEnter();
    void OnExit();
};


}  // namespace bt::playerstates

#endif  // BURGERTIME_PLAYERSTATES_HPP
