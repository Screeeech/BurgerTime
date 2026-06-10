#include "States/EnemyStates.hpp"

#include <cassert>
#include <print>

#include "Components/Animation.hpp"
#include "Components/Collider.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/Stage.hpp"
#include "Components/Timer.hpp"
#include "Events.hpp"
#include "GameEvents.hpp"
#include "Services/EventManager.hpp"
#include "Services/Sound.hpp"
#include "States/PlayerStates.hpp"
#include "Utils.hpp"

namespace bt::enemystates
{

struct Walking;
struct Climbing;
struct StunnedStanding;
struct Dying;


constexpr float stunTime{ 3.f };

void EnemyActiveState::OnEnter()
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.BindEvent("OnPepper"_h, this, &EnemyActiveState::OnPepper);
    eventManager.BindEvent("DisableEntities"_h, this, &EnemyActiveState::OnDisable);
}

void EnemyActiveState::OnExit()
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.UnbindEvents(this);
}

void EnemyActiveState::OnDisable(std::any const& /*eventArgs*/)
{
    machine->TransitionTo<Disabled>();
}

// ==================== IDLE STANDING ====================
void IdleStanding::OnEnter()
{
    EnemyActiveState::OnEnter();

    ctx->animation.SetAnimation("idle"_h, true);
}

void IdleStanding::Update()
{
    if (nextFrame)
        ctx->moveComponent.LockOntoGround();
    else
        nextFrame = true;


    if ((ctx->moveComponent.GetDirection().y < 0 and ctx->moveComponent.CanClimbUp()) or
        (ctx->moveComponent.GetDirection().y > 0 and ctx->moveComponent.CanClimbDown()))
    {
        machine->TransitionTo<Climbing>();
        return;
    }

    if ((ctx->moveComponent.GetDirection().x != 0.f and ctx->moveComponent.CanWalk()))
    {
        machine->TransitionTo<Walking>();
        return;
    }
}

void IdleStanding::OnPepper(std::any const& playerEvent)
{
    if (std::any_cast<gla::PlayerEvent const&>(playerEvent).playerIndex != ctx->entityIndex)
        return;

    gla::Locator::Get<gla::Sound>().PlayAudio("enemy_sprayed"_h);
    machine->TransitionTo<StunnedStanding>();
}


// ==================== WALKING ====================
void Walking::Update()
{
    if (ctx->moveComponent.GetDirection().x < 0)
        ctx->animation.SetAnimation("walkLeft"_h, true);
    if (ctx->moveComponent.GetDirection().x > 0)
        ctx->animation.SetAnimation("walkRight"_h, true);


    if ((ctx->moveComponent.GetDirection().y < 0.f and ctx->moveComponent.CanClimbUp()) or
        (ctx->moveComponent.GetDirection().y > 0.f and ctx->moveComponent.CanClimbDown()))
    {
        machine->TransitionTo<Climbing>();
        return;
    }
    if (ctx->moveComponent.GetDirection().x == 0.f or not ctx->moveComponent.CanWalk())
    {
        machine->TransitionTo<IdleStanding>();
        return;
    }

    ctx->moveComponent.Walk();
}

void Walking::OnPepper(std::any const& playerEvent)
{
    if (std::any_cast<gla::PlayerEvent const&>(playerEvent).playerIndex != ctx->entityIndex)
        return;

    gla::Locator::Get<gla::Sound>().PlayAudio("enemy_sprayed"_h);
    machine->TransitionTo<StunnedStanding>();
}


// ==================== CLIMBING ====================
void Climbing::OnEnter()
{
    EnemyActiveState::OnEnter();

    ctx->moveComponent.LockOntoLadder();
}

void Climbing::Update()
{
    // clang-format off
    if (ctx->moveComponent.IsOnGround() and
        ((ctx->moveComponent.GetDirection().y == 0) or
        (ctx->moveComponent.GetDirection().y < 0 and not ctx->moveComponent.CanClimbUp()) or
        (ctx->moveComponent.GetDirection().y > 0 and not ctx->moveComponent.CanClimbDown())))
    {
        machine->TransitionTo<IdleStanding>();
        return;
    }
    // clang-format on

    if (ctx->moveComponent.GetDirection().y == 0)
    {
        machine->TransitionTo<IdleClimbing>();
        return;
    }


    if (ctx->moveComponent.GetDirection().y < 0)
        ctx->animation.SetAnimation("walkUp"_h, true);
    else if (ctx->moveComponent.GetDirection().y > 0)
        ctx->animation.SetAnimation("walkDown"_h, true);

    ctx->moveComponent.Climb();
}

void Climbing::OnPepper(std::any const& playerEvent)
{
    if (std::any_cast<gla::PlayerEvent const&>(playerEvent).playerIndex != ctx->entityIndex)
        return;

    gla::Locator::Get<gla::Sound>().PlayAudio("enemy_sprayed"_h);
    machine->TransitionTo<StunnedClimbing>();
}


// ==================== IDLE CLIMBING ====================
void IdleClimbing::OnEnter()
{
    EnemyActiveState::OnEnter();

    ctx->animation.SetAnimation("idle"_h);
}

void IdleClimbing::Update()
{
    if (ctx->moveComponent.GetDirection().y != 0)
        machine->TransitionTo<Climbing>();
}

void IdleClimbing::OnPepper(std::any const& playerEvent)
{
    if (std::any_cast<gla::PlayerEvent const&>(playerEvent).playerIndex != ctx->entityIndex)
        return;

    gla::Locator::Get<gla::Sound>().PlayAudio("enemy_sprayed"_h);
    machine->TransitionTo<StunnedClimbing>();
}


// ==================== STUNNED WALKING ====================
void StunnedStanding::OnEnter() const
{
    ctx->animation.SetAnimation("stunned"_h, true);
    ctx->stunTimer.Start(stunTime);
    ctx->playerHitbox.DisableCollisionMasks(gla::Collider::Bits::Layer2);
}

void StunnedStanding::Update()
{
    if (ctx->stunTimer.IsFinished())
        machine->TransitionTo<IdleStanding>();
}

void StunnedStanding::OnExit() const
{
    ctx->playerHitbox.EnableCollisionMasks(gla::Collider::Bits::Layer2);
}


// ==================== STUNNED CLIMBING ====================
void StunnedClimbing::OnEnter() const
{
    ctx->animation.SetAnimation("stunned"_h, true);
    ctx->stunTimer.Start(stunTime);
    ctx->playerHitbox.DisableCollisionMasks(gla::Collider::Bits::Layer2);
}

void StunnedClimbing::Update()
{
    // assert(ctx->stunTimer and "Timer cannot be null");

    if (ctx->stunTimer.IsFinished())
        machine->TransitionTo<IdleClimbing>();
}

void StunnedClimbing::OnExit() const
{
    // assert(ctx->hitBox and "HitBox cannot be null");
    ctx->playerHitbox.EnableCollisionMasks(gla::Collider::Bits::Layer2);
}


// ==================== FALLING ====================
void Falling::OnEnter()
{
    gla::Locator::Get<gla::EventManager>().BindEvent("OnLanding"_h, this, &Falling::OnLanding);
    gla::Locator::Get<gla::EventManager>().BindEvent("EnemyFellOnPlate"_h, this, &Falling::OnPlate);
    gla::Locator::Get<gla::Sound>().PlayAudio("enemy_fall"_h);

    ctx->playerHitbox.DisableCollisionMasks(gla::Collider::Bits::Layer2);

    if (ctx->stunTimer.IsRunning())
        ctx->animation.SetAnimation("stunned"_h, true);
    else
        ctx->animation.SetAnimation("idle"_h, true);
}

void Falling::Update()
{
    if (ctx->stunTimer.IsFinished())
        ctx->animation.SetAnimation("idle"_h, true);
}
void Falling::OnExit() const
{
    gla::Locator::Get<gla::EventManager>().UnbindEvents(this);
    ctx->playerHitbox.EnableCollisionMasks(gla::Collider::Bits::Layer2);
}

void Falling::OnLanding(std::any const& playerEvent) const
{
    if (std::any_cast<gla::PlayerEvent const&>(playerEvent).playerIndex != ctx->entityIndex)
        return;

    // Re-enable feet hurtbox for next
    ctx->feetHurtBox.EnableCollisionLayers(gla::Collider::Bits::Layer6);

    if (ctx->stunTimer.IsRunning())
        machine->TransitionTo<StunnedStanding>();
    else
        machine->TransitionTo<IdleStanding>();
}

void Falling::OnPlate(std::any const& playerEvent) const
{
    if (std::any_cast<gla::PlayerEvent const&>(playerEvent).playerIndex != ctx->entityIndex)
        return;

    machine->TransitionTo<Dying>();
}


// ==================== DYING ====================
void Dying::OnEnter() const
{
    gla::Locator::Get<gla::EventManager>().InvokeEvent(ScoreEvent("ScoreChange"_h, Entity::GetScoreForEnemyType(ctx->type)));
    gla::Locator::Get<gla::Sound>().PlayAudio("enemy_squashed"_h);

    ctx->animation.SetAnimation("dying"_h, true, false);

    // Disable all collisions
    ctx->playerHitbox.SetCollisionLayers(0);
    ctx->playerHitbox.SetCollisionMasks(0);
    ctx->headBurtBox.SetCollisionLayers(0);
    ctx->headBurtBox.SetCollisionMasks(0);
    ctx->feetHurtBox.SetCollisionLayers(0);
    ctx->feetHurtBox.SetCollisionMasks(0);
}

void Dying::Update() {}


// ==================== DYING ====================
void Disabled::OnEnter()
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.BindEvent("EnableEntities"_h, this, &Disabled::OnEnable);

    ctx->animation.SetAnimation("idle"_h, true);
}
void Disabled::Update() {}
void Disabled::OnExit()
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.UnbindEvents(this);
}

void Disabled::OnEnable(std::any const& /*eventArgs*/) const
{
    machine->TransitionTo<IdleStanding>();
}


}  // namespace bt::enemystates
