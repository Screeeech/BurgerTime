#include "States/PlayerStates.hpp"

#include <algorithm>
#include <cassert>
#include <glm/gtc/constants.hpp>
#include <print>

#include "Components/Animation.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/Pepper.hpp"
#include "Components/Player.hpp"
#include "Components/Stage.hpp"
#include "Constants.hpp"
#include "GameEvents.hpp"
#include "Locator.hpp"
#include "Services/EventManager.hpp"
#include "Time.hpp"
#include "Utils.hpp"

namespace bt::playerstates
{
static float remainingPepperDuration{};
static glm::vec2 previousDirection{ 1.f, 1.f };


// ==================== PEPPER ABLE ====================
void PepperableState::OnEnter()
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.BindEvent("pepper"_h, this, &PepperableState::OnPepper);
}

void PepperableState::OnExit()
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.UnbindEvent("pepper"_h, this);
}

// ==================== STANDING IDLE ====================
void StandingIdle::OnEnter()
{
    PepperableState::OnEnter();

    // std::println("Entered Standing Idle state");
    ctx->animation.SetAnimation("idle"_h, true);
    ctx->moveComponent.LockOntoGround();
}

void StandingIdle::Update()
{
    if (remainingPepperDuration > 0.f)
    {
        remainingPepperDuration = std::max(remainingPepperDuration - gla::Time::Get().FixedDeltaTime(), 0.f);
        ChangeAnimation();
    }

    if (ctx->moveComponent.GetDirection().x != 0.f and ctx->moveComponent.CanWalk())
    {
        machine->TransitionTo<Walking>();
        return;
    }
    if ((ctx->moveComponent.GetDirection().y > 0.f and ctx->moveComponent.CanClimbDown()) or
        (ctx->moveComponent.GetDirection().y < 0.f and ctx->moveComponent.CanClimbUp()))
    {
        machine->TransitionTo<Climbing>();
        return;
    }
}

void StandingIdle::OnExit()
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.UnbindEvent("pepper"_h, this);
}

void StandingIdle::OnPepper(std::any const& eventArgs)
{
    auto const& pepperArgs = std::any_cast<PepperEvent>(eventArgs);

    pepperArgs.pPepper->SpawnPepper(pepperArgs.position, { previousDirection.x, 0.f });
    remainingPepperDuration = 1.f;
}

void StandingIdle::ChangeAnimation() const
{
    if (remainingPepperDuration > 0.f)
    {
        if (previousDirection.x >= 0)  // Right
            ctx->animation.SetAnimation("pepperRight"_h);
        else if (previousDirection.x < 0)  // Left
            ctx->animation.SetAnimation("pepperLeft"_h);
    }
    else
    {
        ctx->animation.SetAnimation("idle"_h);
    }
}


// ==================== WALKING ====================
void Walking::Update()
{
    // assert(ctx->animation != nullptr and "Animation cannot be null!");

    if (remainingPepperDuration > 0.f)
        remainingPepperDuration = std::max(remainingPepperDuration - gla::Time::Get().FixedDeltaTime(), 0.f);


    if (ctx->moveComponent.GetDirection().x == 0.f or not ctx->moveComponent.CanWalk())
    {
        // TODO: Find workaround for this
        // auto newContext = ctx;
        // newContext.direction.x = previousXDirection;
        machine->TransitionTo<StandingIdle>();
        return;
    }
    if ((ctx->moveComponent.GetDirection().y < 0.f and ctx->moveComponent.CanClimbUp()) or
        (ctx->moveComponent.GetDirection().y > 0.f and ctx->moveComponent.CanClimbDown()))
    {
        machine->TransitionTo<Climbing>();
        return;
    }

    previousDirection.x = ctx->moveComponent.GetDirection().x;

    ChangeAnimation();

    ctx->moveComponent.Walk();
}

void Walking::OnPepper(std::any const& eventArgs)
{
    auto const& pepperArgs = std::any_cast<PepperEvent>(eventArgs);

    pepperArgs.pPepper->SpawnPepper(pepperArgs.position, pepperArgs.inputDirection);
    remainingPepperDuration = 1.f;
}

void Walking::ChangeAnimation() const
{
    // assert(ctx.animation != nullptr and "Animation cannot be null!");

    auto const& direction = ctx->moveComponent.GetDirection();
    auto& animation = ctx->animation;

    if (direction.x > 0.f)
    {
        if (remainingPepperDuration > 0.f)
            animation.SetAnimation("pepperRight"_h, true);
        else
            animation.SetAnimation("walkRight"_h, true);
    }
    else if (direction.x < 0.f)
    {
        if (remainingPepperDuration > 0.f)
            animation.SetAnimation("pepperLeft"_h, true);
        else
            animation.SetAnimation("walkLeft"_h, true);
    }
}

void ClimbingIdle::OnEnter()
{
    PepperableState::OnEnter();

    ctx->animation.SetAnimation("idle"_h);
}


// ==================== CLIMBING IDLE ====================
void ClimbingIdle::Update()
{
    if (remainingPepperDuration > 0.f)
    {
        if (remainingPepperDuration > 0.f)
            remainingPepperDuration = std::max(remainingPepperDuration - gla::Time::Get().FixedDeltaTime(), 0.f);
        ChangeAnimation();
    }

    if (ctx->moveComponent.GetDirection().y != 0)
        machine->TransitionTo<Climbing>();
}

void ClimbingIdle::OnPepper(std::any const& eventArgs)
{
    auto const& pepperArgs = std::any_cast<PepperEvent>(eventArgs);

    pepperArgs.pPepper->SpawnPepper(pepperArgs.position, { 0.f, previousDirection.y });
    remainingPepperDuration = 1.f;
}

void ClimbingIdle::ChangeAnimation() const
{
    // Up
    if (previousDirection.y < 0)
    {
        if (remainingPepperDuration > 0.f)
        {
            ctx->animation.SetAnimation("pepperUp"_h);
        }
        else
        {
            ctx->animation.SetAnimation("walkUp"_h);
            ctx->animation.SetFrame(1, false);  // Idle frame
        }
    }
    // Down
    else if (previousDirection.y > 0)
    {
        if (remainingPepperDuration > 0.f)
        {
            ctx->animation.SetAnimation("pepperDown"_h);
        }
        else
        {
            ctx->animation.SetAnimation("walkDown"_h);
            ctx->animation.SetFrame(1, false);  // Idle frame
        }
    }
}


// ==================== CLIMBING ====================
void Climbing::OnEnter()
{
    PepperableState::OnEnter();

    // Lock the player onto a ladder when climbing
    ctx->moveComponent.LockOntoLadder();
}

void Climbing::Update()
{
    if (remainingPepperDuration > 0.f)
        remainingPepperDuration = std::max(remainingPepperDuration - gla::Time::Get().FixedDeltaTime(), 0.f);

    auto const direction = ctx->moveComponent.GetDirection();
    if (direction.y != 0)
        previousDirection.y = direction.y;

    // clang-format off
    if (ctx->moveComponent.IsOnGround() and (
           (direction.y == 0) or
           (direction.y < 0 and not ctx->moveComponent.CanClimbUp()) or
           (direction.y > 0 and not ctx->moveComponent.CanClimbDown())
       ))
    {
        machine->TransitionTo<StandingIdle>();
        return;
    }
    // clang-format on

    if (direction.y == 0)
    {
        // auto newContext = ctx;
        // newContext.direction.y = previousYDirection;
        machine->TransitionTo<ClimbingIdle>();
        return;
    }

    ChangeAnimation();

    ctx->moveComponent.Climb();
}

void Climbing::OnPepper(std::any const& eventArgs)
{
    auto const& pepperArgs = std::any_cast<PepperEvent>(eventArgs);

    pepperArgs.pPepper->SpawnPepper(pepperArgs.position, { 0.f, previousDirection.y });
    remainingPepperDuration = 1.f;
}

void Climbing::ChangeAnimation() const
{
    auto& animation = ctx->animation;

    if (previousDirection.y < 0.f)
    {
        if (remainingPepperDuration > 0.f)
            animation.SetAnimation("pepperUp"_h, true);
        else
            animation.SetAnimation("walkUp"_h, true);
    }
    else if (previousDirection.y > 0.f)
    {
        if (remainingPepperDuration > 0.f)
            animation.SetAnimation("pepperDown"_h, true);
        else
            animation.SetAnimation("walkDown"_h, true);
    }
}


// ==================== DYING ====================
void Dying::OnEnter() const
{
    ctx->animation.SetAnimation("death"_h, true, false);
}

void Dying::Update()
{
    wait += gla::Time::Get().FixedDeltaTime();
    if (wait >= totalTime)
        gla::Locator::Get<gla::EventManager>().InvokeEvent(gla::Event{ "reset"_h });
    if (wait >= animationWait)
        ctx->animation.SetAnimation("dying"_h, true);
}

}  // namespace bt::playerstates