#include "States/PlayerStates.hpp"

#include <algorithm>
#include <cassert>
#include <glm/gtc/constants.hpp>
#include <print>

#include "Components/Animation.hpp"
#include "Components/Collider.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/Pepper.hpp"
#include "Components/Stage.hpp"
#include "Components/Timer.hpp"
#include "Constants.hpp"
#include "GameEvents.hpp"
#include "Locator.hpp"
#include "Services/EventManager.hpp"
#include "Services/Sound.hpp"
#include "Time.hpp"
#include "Utils.hpp"

namespace gla
{
class Sound;
}
namespace bt::playerstates
{
static constexpr float pepperDuration{ 1.f };


// ==================== ACTIVE BASE STATE ====================
void PlayerActiveState::OnEnter()
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.BindEvent("Pepper"_h, this, &PlayerActiveState::OnPepper);
    eventManager.BindEvent("DisableEntities"_h, this, &PlayerActiveState::OnDisable);
}

void PlayerActiveState::OnExit()
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.UnbindEvents(this);
}

void PlayerActiveState::OnDisable(std::any const& /*eventArgs*/)
{
    machine->TransitionTo<Disabled>();
}

// ==================== STANDING IDLE ====================
void StandingIdle::OnEnter()
{
    PlayerActiveState::OnEnter();

    // std::println("Entered Standing Idle state");
    ctx->animation.SetAnimation("idle"_h, true);
    ctx->moveComponent.LockOntoGround();
    ctx->collider.EnableCollisionMasks(gla::Collider::Bits::Layer3);
}

void StandingIdle::Update()
{
    ChangeAnimation();

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
    eventManager.UnbindEvents(this);
}

void StandingIdle::OnPepper(std::any const& eventArgs)
{
    auto const& pepperArgs = std::any_cast<PepperEvent>(eventArgs);
    if (pepperArgs.entityIndex != ctx->playerIndex)
        return;

    pepperArgs.pPepper->SpawnPepper(pepperArgs.position, { ctx->previousDirection.x, 0.f });
    ctx->pepperTimer.Start(pepperDuration);
}

void StandingIdle::ChangeAnimation() const
{
    if (ctx->pepperTimer.IsRunning())
    {
        if (ctx->previousDirection.x >= 0)  // Right
            ctx->animation.SetAnimation("pepperRight"_h);
        else if (ctx->previousDirection.x < 0)  // Left
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

    ctx->previousDirection.x = ctx->moveComponent.GetDirection().x;

    ChangeAnimation();

    ctx->moveComponent.Walk();
}

void Walking::OnPepper(std::any const& eventArgs)
{
    auto const& pepperArgs = std::any_cast<PepperEvent>(eventArgs);
    if (pepperArgs.entityIndex != ctx->playerIndex)
        return;

    pepperArgs.pPepper->SpawnPepper(pepperArgs.position, pepperArgs.inputDirection);
    ctx->pepperTimer.Start(pepperDuration);
}

void Walking::ChangeAnimation() const
{
    // assert(ctx.animation != nullptr and "Animation cannot be null!");

    auto const& direction = ctx->moveComponent.GetDirection();
    auto& animation = ctx->animation;

    if (direction.x > 0.f)
    {
        if (ctx->pepperTimer.IsRunning())
            animation.SetAnimation("pepperRight"_h, true);
        else
            animation.SetAnimation("walkRight"_h, true);
    }
    else if (direction.x < 0.f)
    {
        if (ctx->pepperTimer.IsRunning())
            animation.SetAnimation("pepperLeft"_h, true);
        else
            animation.SetAnimation("walkLeft"_h, true);
    }
}

void ClimbingIdle::OnEnter()
{
    PlayerActiveState::OnEnter();

    ctx->animation.SetAnimation("idle"_h);
}


// ==================== CLIMBING IDLE ====================
void ClimbingIdle::Update()
{
    ChangeAnimation();

    if (ctx->moveComponent.GetDirection().y != 0)
        machine->TransitionTo<Climbing>();
}

void ClimbingIdle::OnPepper(std::any const& eventArgs)
{
    auto const& pepperArgs = std::any_cast<PepperEvent>(eventArgs);
    if (pepperArgs.entityIndex != ctx->playerIndex)
        return;

    pepperArgs.pPepper->SpawnPepper(pepperArgs.position, { 0.f, ctx->previousDirection.y });
    ctx->pepperTimer.Start(pepperDuration);
}

void ClimbingIdle::ChangeAnimation() const
{
    // Up
    if (ctx->previousDirection.y < 0)
    {
        if (ctx->pepperTimer.IsRunning())
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
    else if (ctx->previousDirection.y > 0)
    {
        if (ctx->pepperTimer.IsRunning())
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
    PlayerActiveState::OnEnter();

    // Lock the player onto a ladder when climbing
    ctx->moveComponent.LockOntoLadder();
    ctx->collider.DisableCollisionMasks(gla::Collider::Bits::Layer3);
}

void Climbing::Update()
{
    auto const direction = ctx->moveComponent.GetDirection();
    if (direction.y != 0)
        ctx->previousDirection.y = direction.y;

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
    if (pepperArgs.entityIndex != ctx->playerIndex)
        return;

    pepperArgs.pPepper->SpawnPepper(pepperArgs.position, { 0.f, ctx->previousDirection.y });
    ctx->pepperTimer.Start(pepperDuration);
}

void Climbing::ChangeAnimation() const
{
    auto& animation = ctx->animation;

    if (ctx->previousDirection.y < 0.f)
    {
        if (ctx->pepperTimer.IsRunning())
            animation.SetAnimation("pepperUp"_h, true);
        else
            animation.SetAnimation("walkUp"_h, true);
    }
    else if (ctx->previousDirection.y > 0.f)
    {
        if (ctx->pepperTimer.IsRunning())
            animation.SetAnimation("pepperDown"_h, true);
        else
            animation.SetAnimation("walkDown"_h, true);
    }
}


// ==================== DYING ====================
void Dying::OnEnter() const
{
    ctx->animation.SetAnimation("death"_h, true, false);
    gla::Locator::Get<gla::EventManager>().InvokeEvent(gla::Event{ "PlayerDeath"_h });
}

void Dying::Update()
{
    wait += gla::Time::Get().FixedDeltaTime();
    if (wait >= animationWait)
    {
        gla::Locator::Get<gla::Sound>().PlayAudio("death"_h);
        ctx->animation.SetAnimation("dying"_h, true);
    }
}

// ==================== DISABLED ====================

void Disabled::OnEnter()
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.BindEvent("EnableEntities"_h, this, &Disabled::OnEnable);

    ctx->animation.SetAnimation("idle"_h, true);
    ctx->moveComponent.LockOntoGround();
}
void Disabled::Update() {}
void Disabled::OnExit()
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.UnbindEvents(this);
}

void Disabled::OnEnable(std::any const& /*eventArgs*/) const
{
    machine->TransitionTo<StandingIdle>();
}

}  // namespace bt::playerstates