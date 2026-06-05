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


// ==================== STANDING IDLE ====================
void StandingIdle::OnEnter()
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.BindEvent("pepper"_h, this, &StandingIdle::OnPepper);

    // previousXDirection = ctx->direction.x;

    // assert(ctx->animation != nullptr and "Animation cannot be null!");

    // std::println("Entered Standing Idle state");
    ctx->animation.SetAnimation("idle"_h, true);

    // TODO: Fix this pls :)
    // if (ctx->moveComponent)
    ctx->moveComponent.LockOntoGround();
}

void StandingIdle::Update()
{
    //assert(ctx->animation != nullptr and "Animation cannot be null");

    if (remainingPepperDuration > 0.f)
    {
        remainingPepperDuration -= gla::Time::Get().FixedDeltaTime();
        remainingPepperDuration = std::max(remainingPepperDuration, 0.f);
        ChangeAnimation(*ctx);
    }

    if (ctx->moveComponent.GetDirection().x != 0.f and ctx->moveComponent.CanWalk())
    {
        machine->TransitionTo<Walking>();
        return;
    }
    if (ctx->moveComponent.GetDirection().y > 0.f and ctx->moveComponent.CanClimbDown())
    {
        machine->TransitionTo<Walking>();
        return;
    }
    if (ctx->moveComponent.GetDirection().y < 0.f and ctx->moveComponent.CanClimbUp())
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

    pepperArgs.pPepper->SpawnPepper(pepperArgs.position, { previousXDirection, 0.f });
    remainingPepperDuration = 1.f;
}

void StandingIdle::ChangeAnimation(Context const& ctx) const
{
    if (remainingPepperDuration > 0.f)
    {
        if (previousXDirection >= 0)  // Right
            ctx.animation.SetAnimation("pepperRight"_h);
        else if (previousXDirection < 0)  // Left
            ctx.animation.SetAnimation("pepperLeft"_h);
    }
    else
    {
        ctx.animation.SetAnimation("idle"_h);
    }
}


// ==================== WALKING ====================
void Walking::OnEnter()
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.BindEvent("pepper"_h, this, &Walking::OnPepper);

    previousXDirection = ctx->moveComponent.GetDirection().x;

    // std::println("Entered Walking state");
    ChangeAnimation(*ctx);
}

void Walking::Update()
{
    //assert(ctx->animation != nullptr and "Animation cannot be null!");

    if (remainingPepperDuration > 0.f)
    {
        remainingPepperDuration -= gla::Time::Get().FixedDeltaTime();
        if (remainingPepperDuration <= 0.f)
            remainingPepperDuration = 0.f;
    }


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

    previousXDirection = ctx->moveComponent.GetDirection().x;

    ChangeAnimation(*ctx);

    ctx->moveComponent.Walk();
}

void Walking::OnExit()
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

void Walking::ChangeAnimation(Context const& ctx)
{
    //assert(ctx.animation != nullptr and "Animation cannot be null!");

    auto const& direction = ctx.moveComponent.GetDirection();
    auto& animation = ctx.animation;

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


// ==================== CLIMBING IDLE ====================
void ClimbingIdle::OnEnter()
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.BindEvent("pepper"_h, this, &ClimbingIdle::OnPepper);

    previousYDirection = ctx->moveComponent.GetDirection().y;

    // std::println("Entered Climbing Idle state");

    //assert(ctx->animation != nullptr and "Animation cannot be null!");
    //assert(ctx->moveComponent->GetDirection().y != 0 and "Previous direction cannot be 0 when entering climbing idle state");

    ChangeAnimation(*ctx);
}

void ClimbingIdle::Update()
{
    if (remainingPepperDuration > 0.f)
    {
        remainingPepperDuration -= gla::Time::Get().FixedDeltaTime();
        if (remainingPepperDuration <= 0.f)
            remainingPepperDuration = 0.f;
        ChangeAnimation(*ctx);
    }

    if (ctx->moveComponent.GetDirection().y != 0)
        machine->TransitionTo<Climbing>();
}

void ClimbingIdle::OnExit()
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.UnbindEvent("pepper"_h, this);
}

void ClimbingIdle::OnPepper(std::any const& eventArgs)
{
    auto const& pepperArgs = std::any_cast<PepperEvent>(eventArgs);

    pepperArgs.pPepper->SpawnPepper(pepperArgs.position, { 0.f, previousYDirection });
    remainingPepperDuration = 1.f;
}

void ClimbingIdle::ChangeAnimation(Context const& ctx) const
{
    // Up
    if (previousYDirection < 0)
    {
        if (remainingPepperDuration > 0.f)
        {
            ctx.animation.SetAnimation("pepperUp"_h);
        }
        else
        {
            ctx.animation.SetAnimation("walkUp"_h);
            ctx.animation.SetFrame(1, false);  // Idle frame
        }
    }
    // Down
    else if (previousYDirection > 0)
    {
        if (remainingPepperDuration > 0.f)
        {
            ctx.animation.SetAnimation("pepperDown"_h);
        }
        else
        {
            ctx.animation.SetAnimation("walkDown"_h);
            ctx.animation.SetFrame(1, false);  // Idle frame
        }
    }
}


// ==================== CLIMBING ====================
void Climbing::OnEnter()
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.BindEvent("pepper"_h, this, &Climbing::OnPepper);

    // auto& [direction, animation, moveComponent, deltaTime] = ctx;
    // assert(ctx->animation != nullptr and "Animation cannot be null!");
    // assert(ctx->moveComponent->GetDirection().y != 0.f and "Y input direction cannot be 0 when entering climbing state");

    // std::println("Entered Climbing state");
    ChangeAnimation(*ctx);

    previousYDirection = ctx->moveComponent.GetDirection().y;

    // Lock the player onto a ladder when climbing
    ctx->moveComponent.LockOntoLadder();
}

void Climbing::Update()
{
    // auto& [direction, animation, moveComponent, deltaTime] = ctx;
    // assert(animation != nullptr and "Animation cannot be null!");

    if (remainingPepperDuration > 0.f)
    {
        remainingPepperDuration -= gla::Time::Get().FixedDeltaTime();
        remainingPepperDuration = std::max(remainingPepperDuration, 0.f);
    }

    // clang-format off
    if (ctx->moveComponent.IsOnGround() and
        ((ctx->moveComponent.GetDirection().y == 0) or
        (ctx->moveComponent.GetDirection().y < 0 and not ctx->moveComponent.CanClimbUp()) or
        (ctx->moveComponent.GetDirection().y > 0 and not ctx->moveComponent.CanClimbDown())))
    {
        machine->TransitionTo<StandingIdle>();
        return;
    }
    // clang-format on

    if (ctx->moveComponent.GetDirection().y == 0)
    {
        // auto newContext = ctx;
        // newContext.direction.y = previousYDirection;
        machine->TransitionTo<ClimbingIdle>();
        return;
    }

    ChangeAnimation(*ctx);

    previousYDirection = ctx->moveComponent.GetDirection().y;

    ctx->moveComponent.Climb();
}

void Climbing::OnExit()
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

void Climbing::ChangeAnimation(Context const& ctx) const
{
    auto& animation = ctx.animation;

    if (previousYDirection < 0.f)
    {
        if (remainingPepperDuration > 0.f)
            animation.SetAnimation("pepperUp"_h, true);
        else
            animation.SetAnimation("walkUp"_h, true);
    }
    else if (previousYDirection > 0.f)
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
    // assert(ctx->animation and "Animation cannot be null");

    ctx->animation.SetAnimation("death"_h, true);
}

void Dying::Update()
{
    wait += gla::Time::Get().FixedDeltaTime();
    if (wait >= totalTime)
        gla::Locator::Get<gla::EventManager>().InvokeEvent(gla::Event{ "reset"_h });
    if (wait >= animationWait)
        ctx->animation.SetAnimation("dying"_h, true, false);
}

}  // namespace bt::playerstates