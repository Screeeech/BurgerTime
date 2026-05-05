#include "States/PlayerStates.hpp"

#include <print>

namespace bt::playerstates
{

// ==================== IDLE ====================
void Idle::Update(PlayerStateMachine& machine)
{
    std::println("Idle state!");
    machine.TransitionTo<Walking>();
}

void Idle::OnEnter()
{
    std::println("Entering idle state!");
}

void Idle::OnExit()
{
    std::println("Exiting idle state!");
}


// ==================== WALKING ====================
void Walking::Update(PlayerStateMachine& machine)
{
    std::println("Walking state!");
    machine.TransitionTo<Climbing>();
}
void Walking::OnEnter()
{
    std::println("Entering walking state!");
}

void Walking::OnExit()
{
    std::println("Exiting walking state!");
}

// ==================== CLIMBING ====================
void Climbing::Update(PlayerStateMachine& machine)
{
    std::println("Climbing state!");
    machine.TransitionTo<Idle>();
}

void Climbing::OnEnter()
{
    std::println("Entering climbing state!");
}
void Climbing::OnExit()
{
    std::println("Exiting climbing state!");
}

}  // namespace bt::playerstates