#include "States/PlayerStates.hpp"

#include <algorithm>
#include <cassert>
#include <glm/gtc/constants.hpp>
#include <print>

#include "Colors.hpp"
#include "Components/Animation.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/Pepper.hpp"
#include "Components/Player.hpp"
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
    auto const& [direction, animation, moveComponent, deltaTime] = context;

    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.BindEvent("pepper"_h, this, &StandingIdle::OnPepper);

    previousXDirection = direction.x;

    assert(animation != nullptr and "Animation cannot be null!");

    // std::println("Entered Standing Idle state");
    animation->SetAnimation("idle"_h, true);

    if (moveComponent)
        moveComponent->LockOntoGround();
}

void StandingIdle::Update(PlayerStateMachine& machine, Context const& context) const
{
    auto const& [direction, animation, moveComponent, deltaTime] = context;
    assert(animation != nullptr and "Animation cannot be null");

    if (remainingPepperDuration > 0.f)
    {
        remainingPepperDuration -= deltaTime;
        remainingPepperDuration = std::max(remainingPepperDuration, 0.f);
        ChangeAnimation(context);
    }

    if (direction.x != 0.f and moveComponent->CanWalk())
    {
        machine.TransitionTo<Walking>(context);
        return;
    }
    if (direction.y > 0.f and moveComponent->CanClimbDown())
    {
        machine.TransitionTo<Walking>(context);
        return;
    }
    if (direction.y < 0.f and moveComponent->CanClimbUp())
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

    // std::println("Entered Walking state");
    ChangeAnimation(context);
}

void Walking::Update(PlayerStateMachine& machine, Context& context)
{
    auto& [direction, animation, moveComponent, deltaTime] = context;
    assert(animation != nullptr and "Animation cannot be null!");

    if (remainingPepperDuration > 0.f)
    {
        remainingPepperDuration -= deltaTime;
        if (remainingPepperDuration <= 0.f)
            remainingPepperDuration = 0.f;
    }


    if (direction.x == 0.f or not moveComponent->CanWalk())
    {
        auto newContext = context;
        newContext.direction.x = previousXDirection;
        machine.TransitionTo<StandingIdle>(newContext);
        return;
    }
    if ((direction.y < 0.f and moveComponent->CanClimbUp()) or (direction.y > 0.f and moveComponent->CanClimbDown()))
    {
        machine.TransitionTo<Climbing>(context);
        return;
    }

    previousXDirection = context.direction.x;

    ChangeAnimation(context);

    moveComponent->Walk();
}

void Walking::OnExit(Context const& /*context*/)
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.UnbindEvent("pepper"_h, this);
}

void Walking::OnPepper(std::any const& eventArgs)
{
    auto const& pepperArgs = std::any_cast<PepperEvent>(eventArgs);

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

    // std::println("Entered Climbing Idle state");

    auto const& [direction, animation, moveComponent, deltaTime] = context;
    assert(animation != nullptr and "Animation cannot be null!");
    assert(direction.y != 0 and "Previous direction cannot be 0 when entering climbing idle state");

    ChangeAnimation(context);
}

void ClimbingIdle::Update(PlayerStateMachine& machine, Context& context) const
{
    auto const& [direction, animation, moveComponent, deltaTime] = context;

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

    auto& [direction, animation, moveComponent, deltaTime] = context;
    assert(animation != nullptr and "Animation cannot be null!");
    assert(direction.y != 0.f and "Y input direction cannot be 0 when entering climbing state");

    // std::println("Entered Climbing state");
    ChangeAnimation(context);

    previousYDirection = direction.y;

    // Lock the player onto a ladder when climbing
    moveComponent->LockOntoLadder();
}

void Climbing::Update(PlayerStateMachine& machine, Context& context)
{
    auto& [direction, animation, moveComponent, deltaTime] = context;
    assert(animation != nullptr and "Animation cannot be null!");

    if (remainingPepperDuration > 0.f)
    {
        remainingPepperDuration -= deltaTime;
        remainingPepperDuration = std::max(remainingPepperDuration, 0.f);
    }

    // clang-format off
    if (moveComponent->IsOnGround() and
        ((direction.y == 0) or
        (direction.y < 0 and not moveComponent->CanClimbUp()) or
        (direction.y > 0 and not moveComponent->CanClimbDown())))
    {
        machine.TransitionTo<StandingIdle>(context);
        return;
    }
    // clang-format on

    if (direction.y == 0)
    {
        auto newContext = context;
        newContext.direction.y = previousYDirection;
        machine.TransitionTo<ClimbingIdle>(newContext);
        return;
    }

    ChangeAnimation(context);

    previousYDirection = direction.y;

    moveComponent->Climb();
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


// ==================== DYING ====================
void Dying::OnEnter(Context const& context)
{
    assert(context.animation and "Animation cannot be null");

    context.animation->SetAnimation("death"_h, true);
}

void Dying::Update(PlayerStateMachine& /*machine*/, Context const& context)
{
    wait += context.deltaTime;
    if (wait >= totalTime)
        gla::Locator::Get<gla::EventManager>().InvokeEvent(gla::Event{ "reset"_h });
    if (wait >= animationWait)
        context.animation->SetAnimation("dying"_h, true);
}

}  // namespace bt::playerstates