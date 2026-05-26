#include "States/PlayerStates.hpp"

#include <cassert>
#include <glm/gtc/constants.hpp>
#include <print>

#include "Colors.hpp"
#include "Components/PlayerController.hpp"
#include "Components/Stage.hpp"
#include "Locator.hpp"
#include "Utils.hpp"

namespace bt::playerstates
{


// ==================== STANDING IDLE ====================
void StandingIdle::Update(PlayerStateMachine& machine, Context const& context)
{
    auto const& [direction, position, animation, stage, player] = context;
    assert(stage != nullptr and "Stage cannot be null!");

    if (direction != glm::zero<glm::vec3>() and stage->CanWalk(position, direction))
        machine.TransitionTo<Walking>(context);
}

void StandingIdle::OnEnter(Context const& context)
{
    assert(context.animation != nullptr and "Animation cannot be null!");

    std::println("Entered Standing Idle state");
    context.animation->SetAnimation("idle"_h, true);
}


// ==================== WALKING ====================
void Walking::Update(PlayerStateMachine& machine, Context& context)
{
    auto& [direction, position, animation, stage, player] = context;
    assert(animation != nullptr and "Animation cannot be null!");

    if (direction.x == 0.f or not stage->CanWalk(position, direction))
    {
        machine.TransitionTo<StandingIdle>(context);
        return;
    }
    if ((direction.y < 0.f and stage->CanClimbUp(position)) /* or (direction.y > 0.f and stage->CanClimbDown(position))*/)
    {
        machine.TransitionTo<Climbing>(context);
        return;
    }

    ChangeAnimation(context);

    // Walk every 2 frames
    if (wait > 1)
    {
        --wait;
        return;
    }
    wait = 1;

    player->Walk(direction.x);

    stage->PrintTileType(position);
}

void Walking::OnEnter(Context const& context)
{
    std::println("Entered walking state");
    ChangeAnimation(context);
}

void Walking::ChangeAnimation(Context const& context)
{
    assert(context.animation != nullptr and "Animation cannot be null!");

    auto const& direction = context.direction;
    auto const& animation = context.animation;

    if (direction.x > 0.f)
        animation->SetAnimation("walkRight"_h, true);
    else if (direction.x < 0.f)
        animation->SetAnimation("walkLeft"_h, true);
    // else if (direction.y < 0.f)
    //     animation->SetActiveAnimation("walkUp"_h, true);
    // else if (direction.y > 0.f)
    //     animation->SetActiveAnimation("walkDown"_h, true);
}


// ==================== CLIMBING IDLE ====================
void ClimbingIdle::OnEnter(Context const& context)
{
    std::println("Entered Climbing Idle state");

    auto const& [direction, position, animation, stage, player] = context;
    assert(animation != nullptr and "Animation cannot be null!");
    assert(direction.y != 0 and "Previous direction cannot be 0 when entering climbind idle state");

    // Up
    if (direction.y < 0)
    {
        animation->SetAnimation("walkUp"_h);
        animation->SetFrame(1, false); // Idle frame
    }
    // Down
    else if (direction.y > 0)
    {
        animation->SetAnimation("walkDown"_h);
        animation->SetFrame(1, false); // Idle frame
    }
}

void ClimbingIdle::Update(PlayerStateMachine& machine, Context& context)
{
    auto const& [direction, position, animation, stage, player] = context;

    if (direction.y != 0)
        machine.TransitionTo<Climbing>(context);
}


// ==================== CLIMBING ====================

void Climbing::Update(PlayerStateMachine& machine, Context& context)
{
    auto& [direction, position, animation, stage, player] = context;
    assert(stage != nullptr and "Stage cannot be null!");
    assert(animation != nullptr and "Animation cannot be null!");

    if (stage->IsOnGround(position))
    {
        // Reached top
        if (direction.y == 0)
        {
            machine.TransitionTo<StandingIdle>(context);
            return;
        }
        if (direction.y < 0 and not stage->CanClimbUp(position))
        {
            machine.TransitionTo<StandingIdle>(context);
            return;
        }
    }
    if (direction.y == 0)
    {
        auto newContext = context;
        newContext.direction.y = previousYDirection;
        machine.TransitionTo<ClimbingIdle>(newContext);
        return;
    }

    if (direction.y > 0)
        animation->SetAnimation("walkDown"_h, true);
    else if (direction.y < 0)
        animation->SetAnimation("walkUp"_h, true);

    previousYDirection = direction.y;

    // This is to make the moving happen every 3 frames
    if (wait <= 2)
    {
        std::println("Wait! {}", wait);
        ++wait;
        return;
    }
    wait = 0;

    std::println("Climb");
    player->Climb(direction.y);
}

void Climbing::OnEnter(Context const& context)
{
    assert(context.animation != nullptr and "Animation cannot be null!");
    assert(context.direction.y != 0.f and "Y input direction cannot be 0 when entering climbing state");

    std::println("Entered Climbing state");
    ChangeAnimation(context);

    previousYDirection = context.direction.y;
}

void Climbing::ChangeAnimation(Context const& context)
{
    auto const& direction = context.direction;
    auto const& animation = context.animation;

    if (direction.y < 0.f)
        animation->SetAnimation("walkUp"_h, true);
    else if (direction.y > 0.f)
        animation->SetAnimation("walkDown"_h, true);
}

}  // namespace bt::playerstates