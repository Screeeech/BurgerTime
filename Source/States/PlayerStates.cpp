#include "States/PlayerStates.hpp"

#include <cassert>
#include <glm/gtc/constants.hpp>
#include <print>

#include "Colors.hpp"
#include "Components/Pepper.hpp"
#include "Components/PlayerController.hpp"
#include "Components/Stage.hpp"
#include "GameEvents.hpp"
#include "Locator.hpp"
#include "Services/EventManager.hpp"
#include "Utils.hpp"

namespace bt::playerstates
{
static float remainingPepperDuration{};


// ==================== STANDING IDLE ====================
void StandingIdle::OnEnter(Context const& context)
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.BindEvent("pepper"_h, this, &StandingIdle::OnPepper);

    previousXDirection = context.direction.x;

    assert(context.animation != nullptr and "Animation cannot be null!");

    //std::println("Entered Standing Idle state");
    context.animation->SetAnimation("idle"_h, true);

    if (context.playerController)
    {
        auto const yOffsetIntoTile = static_cast<int>(context.position.y - Stage::stageOffset) % static_cast<int>(Stage::tileHeight);
        auto const bump = static_cast<float>(13 - yOffsetIntoTile);
        context.playerController->Move({ 0.f, bump });
    }
}

void StandingIdle::Update(PlayerStateMachine& machine, Context const& context) const
{
    auto const& [direction, position, animation, stage, player, deltaTime] = context;
    assert(stage != nullptr and "Stage cannot be null!");
    assert(animation != nullptr and "Animation cannot be null");

    if (remainingPepperDuration > 0.f)
    {
        remainingPepperDuration -= deltaTime;
        if (remainingPepperDuration <= 0.f)
            remainingPepperDuration = 0.f;
        ChangeAnimation(context);
    }

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

void StandingIdle::OnExit(Context const& /*context*/)
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.UnbindEvent("pepper"_h, this);
}

void StandingIdle::OnPepper(std::any const& eventArgs)
{
    auto const& pepperArgs = std::any_cast<PepperEvent>(eventArgs);

    pepperArgs.pPepper->SpawnPepper(pepperArgs.position, { previousXDirection, 0.f });
    remainingPepperDuration = 1.f;
}

void StandingIdle::ChangeAnimation(Context const& context) const
{
    if (remainingPepperDuration > 0.f)
    {
        if (previousXDirection >= 0)  // Right
            context.animation->SetAnimation("pepperRight"_h);
        else if (previousXDirection < 0)  // Left
            context.animation->SetAnimation("pepperLeft"_h);
    }
    else
    {
        context.animation->SetAnimation("idle"_h);
    }
}


// ==================== WALKING ====================
void Walking::OnEnter(Context const& context)
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.BindEvent("pepper"_h, this, &Walking::OnPepper);

    previousXDirection = context.direction.x;

    //std::println("Entered Walking state");
    ChangeAnimation(context);
}

void Walking::Update(PlayerStateMachine& machine, Context& context)
{
    auto& [direction, position, animation, stage, player, deltaTime] = context;
    assert(animation != nullptr and "Animation cannot be null!");

    if (remainingPepperDuration > 0.f)
    {
        remainingPepperDuration -= deltaTime;
        if (remainingPepperDuration <= 0.f)
            remainingPepperDuration = 0.f;
    }


    if (direction.x == 0.f or not stage->CanWalk(position, direction))
    {
        auto newContext = context;
        newContext.direction.x = previousXDirection;
        machine.TransitionTo<StandingIdle>(newContext);
        return;
    }
    if ((direction.y < 0.f and stage->CanClimbUp(position)) or (direction.y > 0.f and stage->CanClimbDown(position)))
    {
        machine.TransitionTo<Climbing>(context);
        return;
    }

    previousXDirection = context.direction.x;

    ChangeAnimation(context);

    // Walk every 2 frames
    // if (wait > 0)
    //{
    //    --wait;
    //    return;
    //}
    // wait = 1;

    player->Move({ direction.x, 0.f });
}

void Walking::OnExit(Context const& /*context*/)
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.UnbindEvent("pepper"_h, this);
}

void Walking::OnPepper(std::any const& eventArgs)
{
    [[maybe_unused]] auto const& pepperArgs = std::any_cast<PepperEvent>(eventArgs);

    pepperArgs.pPepper->SpawnPepper(pepperArgs.position, pepperArgs.inputDirection);
    remainingPepperDuration = 1.f;
}

void Walking::ChangeAnimation(Context const& context)
{
    assert(context.animation != nullptr and "Animation cannot be null!");

    auto const& direction = context.direction;
    auto const& animation = context.animation;

    if (direction.x > 0.f)
    {
        if (remainingPepperDuration > 0.f)
            animation->SetAnimation("pepperRight"_h, true);
        else
            animation->SetAnimation("walkRight"_h, true);
    }
    else if (direction.x < 0.f)
    {
        if (remainingPepperDuration > 0.f)
            animation->SetAnimation("pepperLeft"_h, true);
        else
            animation->SetAnimation("walkLeft"_h, true);
    }
}


// ==================== CLIMBING IDLE ====================
void ClimbingIdle::OnEnter(Context const& context)
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.BindEvent("pepper"_h, this, &ClimbingIdle::OnPepper);

    previousYDirection = context.direction.y;

    //std::println("Entered Climbing Idle state");

    auto const& [direction, position, animation, stage, player, deltaTime] = context;
    assert(animation != nullptr and "Animation cannot be null!");
    assert(direction.y != 0 and "Previous direction cannot be 0 when entering climbing idle state");

    ChangeAnimation(context);
}

void ClimbingIdle::Update(PlayerStateMachine& machine, Context& context) const
{
    auto const& [direction, position, animation, stage, player, deltaTime] = context;

    if (remainingPepperDuration > 0.f)
    {
        remainingPepperDuration -= deltaTime;
        if (remainingPepperDuration <= 0.f)
            remainingPepperDuration = 0.f;
        ChangeAnimation(context);
    }

    if (direction.y != 0)
        machine.TransitionTo<Climbing>(context);
}

void ClimbingIdle::OnExit(Context const& /*context*/)
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.UnbindEvent("pepper"_h, this);
}

void ClimbingIdle::OnPepper(std::any const& eventArgs)
{
    [[maybe_unused]] auto const& pepperArgs = std::any_cast<PepperEvent>(eventArgs);

    pepperArgs.pPepper->SpawnPepper(pepperArgs.position, { 0.f, previousYDirection });
    remainingPepperDuration = 1.f;
}

void ClimbingIdle::ChangeAnimation(Context const& context) const
{
    // Up
    if (previousYDirection < 0)
    {
        if (remainingPepperDuration > 0.f)
        {
            context.animation->SetAnimation("pepperUp"_h);
        }
        else
        {
            context.animation->SetAnimation("walkUp"_h);
            context.animation->SetFrame(1, false);  // Idle frame
        }
    }
    // Down
    else if (previousYDirection > 0)
    {
        if (remainingPepperDuration > 0.f)
        {
            context.animation->SetAnimation("pepperDown"_h);
        }
        else
        {
            context.animation->SetAnimation("walkDown"_h);
            context.animation->SetFrame(1, false);  // Idle frame
        }
    }
}


// ==================== CLIMBING ====================
void Climbing::OnEnter(Context const& context)
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.BindEvent("pepper"_h, this, &Climbing::OnPepper);

    auto& [direction, position, animation, stage, player, deltaTime] = context;
    assert(animation != nullptr and "Animation cannot be null!");
    assert(direction.y != 0.f and "Y input direction cannot be 0 when entering climbing state");

    //std::println("Entered Climbing state");
    ChangeAnimation(context);

    previousYDirection = direction.y;

    // Lock the player onto a ladder when climbing
    auto const xOffsetIntoTile = static_cast<int>(position.x - Stage::stageOffset) % static_cast<int>(Stage::tileWidth);
    auto const bump = static_cast<float>(7 - xOffsetIntoTile);
    player->Move({ bump, 0.f });
}

void Climbing::Update(PlayerStateMachine& machine, Context& context)
{
    auto& [direction, position, animation, stage, player, deltaTime] = context;
    assert(stage != nullptr and "Stage cannot be null!");
    assert(animation != nullptr and "Animation cannot be null!");

    if (remainingPepperDuration > 0.f)
    {
        remainingPepperDuration -= deltaTime;
        if (remainingPepperDuration <= 0.f)
            remainingPepperDuration = 0.f;
    }

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

    ChangeAnimation(context);

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

void Climbing::OnExit(Context const& /*context*/)
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.UnbindEvent("pepper"_h, this);
}

void Climbing::OnPepper(std::any const& eventArgs)
{
    [[maybe_unused]] auto const& pepperArgs = std::any_cast<PepperEvent>(eventArgs);

    pepperArgs.pPepper->SpawnPepper(pepperArgs.position, { 0.f, previousYDirection });
    remainingPepperDuration = 1.f;
}

void Climbing::ChangeAnimation(Context const& context) const
{
    auto const& animation = context.animation;

    if (previousYDirection < 0.f)
    {
        if (remainingPepperDuration > 0.f)
            animation->SetAnimation("pepperUp"_h, true);
        else
            animation->SetAnimation("walkUp"_h, true);
    }
    else if (previousYDirection > 0.f)
    {
        if (remainingPepperDuration > 0.f)
            animation->SetAnimation("pepperDown"_h, true);
        else
            animation->SetAnimation("walkDown"_h, true);
    }
}

}  // namespace bt::playerstates