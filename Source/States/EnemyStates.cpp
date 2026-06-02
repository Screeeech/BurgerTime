#include "States/EnemyStates.hpp"

#include <cassert>

#include "Components/Animation.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/Stage.hpp"
#include "Components/Timer.hpp"
#include "States/PlayerStates.hpp"
#include "Utils.hpp"

namespace bt::enemystates
{

struct Walking;
struct Climbing;
struct StunnedWalking;
struct Dying;


// ==================== STANDING IDLE ====================
void StandingIdle::OnEnter(Context const& context)
{
    auto const& [animation, timer, moveComponent] = context;
    assert(animation and "Animation cannot be null");

    animation->SetAnimation("idle"_h, true);

    if (moveComponent)
        moveComponent->LockOntoGround();
}

void StandingIdle::Update(EnemyStateMachine& machine, Context const& context)
{
    auto const& [animation, timer, moveComponent] = context;
    assert(moveComponent and "MoveComponent cannot be null");

    if ((moveComponent->GetDirection().y < 0 and moveComponent->CanClimbUp()) or
        (moveComponent->GetDirection().y > 0 and moveComponent->CanClimbDown()))
    {
        machine.TransitionTo<Climbing>();
        return;
    }

    if ((moveComponent->GetDirection().x != 0.f and moveComponent->CanWalk()))
    {
        machine.TransitionTo<Walking>(context);
        return;
    }
}

// ==================== WALKING ====================
void Walking::Update(EnemyStateMachine& machine, Context const& context)
{
    auto const& [animation, timer, moveComponent] = context;
    assert(animation and "Animation cannot be null");
    assert(moveComponent and "MoveComponent cannot be null");

    if (moveComponent->GetDirection().x < 0)
        animation->SetAnimation("walkLeft"_h, true);
    if (moveComponent->GetDirection().x > 0)
        animation->SetAnimation("walkRight"_h, true);


    if ((moveComponent->GetDirection().y < 0.f and moveComponent->CanClimbUp()) or
        (moveComponent->GetDirection().y > 0.f and moveComponent->CanClimbDown()))
    {
        machine.TransitionTo<Climbing>(context);
        return;
    }
    if (moveComponent->GetDirection().x == 0.f or not moveComponent->CanWalk())
    {
        machine.TransitionTo<StandingIdle>(context);
        return;
    }

    moveComponent->Walk();
}


// ==================== CLIMBING ====================
 void Climbing::OnEnter(Context const& context)
{
    if (context.moveComponent)
        context.moveComponent->LockOntoLadder();
}

void Climbing::Update(EnemyStateMachine& machine, Context const& context)
{
    auto const& [animation, timer, moveComponent] = context;
    assert(animation and "Animation cannot be null");
    assert(moveComponent and "MoveComponent cannot be null");

    // Reached top
    // clang-format off
    if (moveComponent->IsOnGround() and
        ((moveComponent->GetDirection().y == 0) or
        (moveComponent->GetDirection().y < 0 and not moveComponent->CanClimbUp()) or
        (moveComponent->GetDirection().y > 0 and not moveComponent->CanClimbDown())))
    {
        machine.TransitionTo<StandingIdle>(context);
        return;
    }
    // clang-format on

    if (moveComponent->GetDirection().y == 0)
    {
        machine.TransitionTo<ClimbingIdle>(context);
        return;
    }


    if (moveComponent->GetDirection().y < 0)
        animation->SetAnimation("walkUp"_h, true);
    else if (moveComponent->GetDirection().y > 0)
        animation->SetAnimation("walkDown"_h, true);

    moveComponent->Climb();
}


// ==================== STUNNED WALKING ====================
void StunnedWalking::OnEnter(Context const& context)
{
    assert(context.animation and "Animation cannot be null");
    context.animation->SetAnimation("stunned"_h, true);

    assert(context.timer and "Timer cannot be null");
    context.timer->Start(stunTime);
}

void StunnedWalking::Update(EnemyStateMachine& machine, Context const& context)
{
    assert(context.timer and "Timer cannot be null");

    if (context.timer->IsFinished())
        machine.TransitionTo<Walking>(context);
}


// ==================== STUNNED CLIMBING ====================
void StunnedClimbing::OnEnter(Context const& context)
{
    assert(context.animation and "Animation cannot be null");
    context.animation->SetAnimation("stunned"_h, true);

    assert(context.timer and "Timer cannot be null");
    context.timer->Start(stunTime);
}

void StunnedClimbing::Update(EnemyStateMachine& machine, Context const& context)
{
    assert(context.timer and "Timer cannot be null");

    if (context.timer->IsFinished())
        machine.TransitionTo<Walking>(context);
}


// ==================== DYING ====================
void Dying::OnEnter(Context const& context)
{
    assert(context.animation and "Animation cannot be null");
    // context.animation->SetAnimation("dying"_h, true);
}

void Dying::Update([[maybe_unused]] EnemyStateMachine& machine, [[maybe_unused]] Context& context) {}


// void ClimbingIdle::OnEnter(Context const& context) {}

void ClimbingIdle::Update(EnemyStateMachine& machine, Context const& context)
{
    auto const& [animation, timer, moveComponent] = context;
    assert(moveComponent and "MoveComponent cannot be null");

    if (moveComponent->GetDirection().y != 0)
        machine.TransitionTo<Climbing>(context);
}

}  // namespace bt::enemystates
