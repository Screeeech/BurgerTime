#include "States/EnemyStates.hpp"

#include <cassert>

#include "Components/Animation.hpp"
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


// ==================== WALKING ====================
void Walking::OnEnter(Context const& context)
{
    assert(context.animation and "Animation cannot be null");
    context.animation->SetAnimation("walkLeft"_h, true);
}

void Walking::Update(EnemyStateMachine& machine, Context const& context)
{
    auto const& [direction, position, animation, timer, enemy, stage] = context;
    assert(stage and "Stage cannot be null");

    // clang-format off
    if ((direction.y < 0 and stage->CanClimbUp(position)) or
        (direction.y > 0 and stage->CanClimbDown(position)))
        machine.TransitionTo<Climbing>();
    // clang-format on
}


// ==================== CLIMBING ====================
void Climbing::OnEnter(Context const& context)
{
    auto const& [direction, position, animation, timer, enemy, stage] = context;
    assert(animation and "Animation cannot be null");

    if (direction.y < 0)
        animation->SetAnimation("climbUp"_h, true);
    else if (direction.y > 0)
        animation->SetAnimation("climbDown"_h, true);
}

void Climbing::Update(EnemyStateMachine& machine, Context const& context)
{
    auto const& [direction, position, animation, timer, enemy, stage] = context;

    if (stage->IsOnGround(position))
        machine.TransitionTo<Walking>();
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

}  // namespace bt::enemystates
