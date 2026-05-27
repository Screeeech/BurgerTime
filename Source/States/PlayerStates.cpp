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

    if (direction.x != 0.f and stage->CanWalk(position, direction))
    {
        machine.TransitionTo<Walking>(context);
        return;
    }
    if (direction.y > 0.f and stage->CanClimbDown(position))
    {
        machine.TransitionTo<Walking>(context);
        return;
    }
    if (direction.y < 0.f and stage->CanClimbUp(position))
    {
        machine.TransitionTo<Climbing>(context);
        return;
    }
}

void StandingIdle::OnEnter(Context const& context)
{
    assert(context.animation != nullptr and "Animation cannot be null!");

    std::println("Entered Standing Idle state");
    context.animation->SetAnimation("idle"_h, true);

     if (context.playerController)
    {
         auto const yOffsetIntoTile = static_cast<int>(context.position.y - Stage::stageOffset) % static_cast<int>(Stage::tileHeight);
         auto const bump = static_cast<float>(13 - yOffsetIntoTile);
         std::println("Bump y: {}", bump);
         context.playerController->Move({ 0.f, bump });
     }
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
    if ((direction.y < 0.f and stage->CanClimbUp(position)) or (direction.y > 0.f and stage->CanClimbDown(position)))
    {
        machine.TransitionTo<Climbing>(context);
        return;
    }

    ChangeAnimation(context);

    // Walk every 2 frames
    if (wait > 0)
    {
        --wait;
        return;
    }
    wait = 1;

    player->Move({ direction.x, 0.f });
}

void Walking::OnEnter(Context const& context)
{
    std::println("Entered Walking state");
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
        animation->SetFrame(1, false);  // Idle frame
    }
    // Down
    else if (direction.y > 0)
    {
        animation->SetAnimation("walkDown"_h);
        animation->SetFrame(1, false);  // Idle frame
    }
}

void ClimbingIdle::Update(PlayerStateMachine& machine, Context& context)
{
    auto const& [direction, position, animation, stage, player] = context;

    if (direction.y != 0)
        machine.TransitionTo<Climbing>(context);
}


// ==================== CLIMBING ====================

void Climbing::OnEnter(Context const& context)
{
    auto& [direction, position, animation, stage, player] = context;
    assert(animation != nullptr and "Animation cannot be null!");
    assert(direction.y != 0.f and "Y input direction cannot be 0 when entering climbing state");

    std::println("Entered Climbing state");
    ChangeAnimation(context);

    previousYDirection = direction.y;

    // Lock the player onto a ladder when climbing
    auto const xOffsetIntoTile = static_cast<int>(position.x - Stage::stageOffset) % static_cast<int>(Stage::tileWidth);
    auto const bump = static_cast<float>(7 - xOffsetIntoTile);
    std::println("Bump x: {}", bump);
    player->Move({ bump, 0.f });
}

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
        if (direction.y > 0 and not stage->CanClimbDown(position))
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
    if (wait > 0)
    {
        --wait;
        return;
    }
    wait = 1;

    player->Move({ 0.f, direction.y });
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