#include "States/PlayerStates.hpp"

#include <glm/gtc/constants.hpp>
#include <print>

namespace bt::playerstates
{

// ==================== IDLE ====================
void Idle::Update(PlayerStateMachine& machine, Context const& context)
{
    //std::println("Idle state!");

    if (context.direction != glm::zero<glm::vec3>())
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
void Walking::Update(PlayerStateMachine& machine, Context const& context)
{
    //std::println("Walking state!");

    if (context.direction == glm::zero<glm::vec3>())
        machine.TransitionTo<Idle>();
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
void Climbing::Update(PlayerStateMachine& machine, [[maybe_unused]] Context const& context)
{
    //std::println("Climbing state!");
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