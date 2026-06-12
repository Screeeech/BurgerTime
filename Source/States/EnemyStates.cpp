#include "States/EnemyStates.hpp"

#include <cassert>
#include <print>

#include "Components/Animation.hpp"
#include "Components/Collider.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/Stage.hpp"
#include "Components/Timer.hpp"
#include "Constants.hpp"
#include "Events.hpp"
#include "GameEvents.hpp"
#include "Services/EventManager.hpp"
#include "Services/Sound.hpp"
#include "States/PlayerStates.hpp"
#include "Utils.hpp"

namespace bt::enemystates
{

constexpr float aiDelay{ 0.5f };
constexpr float spawnWalkingTime{ 1.5f };

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
    if (firstFrame)
    {
        firstFrame = false;
        ctx->moveComponent.LockOntoGround();
    }


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
    if (std::any_cast<gla::EntityEvent const&>(playerEvent).entityIndex != ctx->entityIndex)
        return;

    gla::Locator::Get<gla::Sound>().PlayAudio("enemy_sprayed"_h);
    ctx->stunTimer.Start(game::stunDuration);
    machine->TransitionTo<StunnedStanding>();
}


// ==================== WALKING ====================
void Walking::Update()
{
    if ((ctx->moveComponent.CanClimbUp() or ctx->moveComponent.CanClimbDown()) and not ctx->aiDelayTimer.IsRunning())
    {
        gla::Locator::Get<gla::EventManager>().InvokeEvent(gla::EntityEvent("FindNewDirection"_h, ctx->entityIndex));
        ctx->aiDelayTimer.Start(aiDelay);
    }

    if (ctx->moveComponent.GetDirection().x < 0)
        ctx->animation.SetAnimation("walkLeft"_h, true);
    if (ctx->moveComponent.GetDirection().x > 0)
        ctx->animation.SetAnimation("walkRight"_h, true);


    bool const canClimbUp = ctx->moveComponent.CanClimbUp();
    bool const canClimbDown = ctx->moveComponent.CanClimbDown();
    auto const direction = ctx->moveComponent.GetDirection();

    if ((direction.y < 0.f and canClimbUp) or (direction.y > 0.f and canClimbDown))
    {
        machine->TransitionTo<Climbing>();
        return;
    }
    if (direction.x == 0.f or not ctx->moveComponent.CanWalk())
    {
        machine->TransitionTo<IdleStanding>();
        return;
    }

    ctx->moveComponent.Walk();
}

void Walking::OnPepper(std::any const& playerEvent)
{
    if (std::any_cast<gla::EntityEvent const&>(playerEvent).entityIndex != ctx->entityIndex)
        return;

    gla::Locator::Get<gla::Sound>().PlayAudio("enemy_sprayed"_h);
    ctx->stunTimer.Start(game::stunDuration);
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
    if (ctx->moveComponent.IsOnGround() and not ctx->aiDelayTimer.IsRunning())
    {
        gla::Locator::Get<gla::EventManager>().InvokeEvent(gla::EntityEvent("FindNewDirection"_h, ctx->entityIndex));
        ctx->aiDelayTimer.Start(aiDelay);
    }

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
    if (std::any_cast<gla::EntityEvent const&>(playerEvent).entityIndex != ctx->entityIndex)
        return;

    gla::Locator::Get<gla::Sound>().PlayAudio("enemy_sprayed"_h);
    ctx->stunTimer.Start(game::stunDuration);
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
    if (std::any_cast<gla::EntityEvent const&>(playerEvent).entityIndex != ctx->entityIndex)
        return;

    gla::Locator::Get<gla::Sound>().PlayAudio("enemy_sprayed"_h);
    ctx->stunTimer.Start(game::stunDuration);
    machine->TransitionTo<StunnedClimbing>();
}


// ==================== STUNNED WALKING ====================
void StunnedStanding::OnEnter()
{
    gla::Locator::Get<gla::EventManager>().BindEvent("DisableEntities"_h, this, &StunnedStanding::OnDisable);

    ctx->animation.SetAnimation("stunned"_h, true);
    ctx->playerHitbox.DisableCollisionMasks(gla::Collider::Bits::Layer2);
}

void StunnedStanding::Update()
{
    if (not ctx->stunTimer.IsRunning())
        machine->TransitionTo<IdleStanding>();
}

void StunnedStanding::OnExit() const
{
    gla::Locator::Get<gla::EventManager>().UnbindEvents(this);
    ctx->playerHitbox.EnableCollisionMasks(gla::Collider::Bits::Layer2);
}

void StunnedStanding::OnDisable(std::any const& /*eventArgs*/) const
{
    machine->TransitionTo<Disabled>();
}


// ==================== STUNNED CLIMBING ====================
void StunnedClimbing::OnEnter()
{
    gla::Locator::Get<gla::EventManager>().BindEvent("DisableEntities"_h, this, &StunnedClimbing::OnDisable);

    ctx->animation.SetAnimation("stunned"_h, true);
    ctx->playerHitbox.DisableCollisionMasks(gla::Collider::Bits::Layer2);
}

void StunnedClimbing::Update()
{
    if (not ctx->stunTimer.IsRunning())
        machine->TransitionTo<IdleClimbing>();
}

void StunnedClimbing::OnExit() const
{
    gla::Locator::Get<gla::EventManager>().UnbindEvents(this);
    ctx->playerHitbox.EnableCollisionMasks(gla::Collider::Bits::Layer2);
}

void StunnedClimbing::OnDisable(std::any const& /*eventArgs*/) const
{
    machine->TransitionTo<Disabled>();
}


// ==================== FALLING ====================
void Falling::OnEnter()
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.BindEvent("OnLanding"_h, this, &Falling::OnLanding);
    eventManager.BindEvent("EnemyFellOnPlate"_h, this, &Falling::OnPlate);
    eventManager.BindEvent("DisableEntities"_h, this, &Falling::OnDisable);

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
    if (std::any_cast<gla::EntityEvent const&>(playerEvent).entityIndex != ctx->entityIndex)
        return;

    // Re-enable feet hurtbox for next
    ctx->feetHurtBox.EnableCollisionLayers(gla::Collider::Bits::Layer6);


    if (ctx->disabled)
        machine->TransitionTo<Disabled>();
    else if (ctx->stunTimer.IsRunning())
        machine->TransitionTo<StunnedStanding>();
    else
        machine->TransitionTo<IdleStanding>();
}

void Falling::OnPlate(std::any const& playerEvent) const
{
    if (std::any_cast<gla::EntityEvent const&>(playerEvent).entityIndex != ctx->entityIndex)
        return;

    machine->TransitionTo<Dying>();
}

void Falling::OnDisable(std::any const& /*eventArgs*/) const
{
    // In the case that the enemy is falling I don't want to immediately put them in the disabled state.
    // Because then some logic wouldn't play, like death and score
    // So I set a flag to make sure I go to the disabled state when the enemy lands
    ctx->disabled = true;
}


// ==================== DYING ====================
void Dying::OnEnter() const
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.InvokeEvent(ScoreEvent("ScoreChange"_h, Entity::GetScoreForEnemyType(ctx->type)));
    eventManager.InvokeEvent(EnemyDeathEvent("EnemyDeath"_h, ctx->entityIndex, ctx->type));
    gla::Locator::Get<gla::Sound>().PlayAudio("enemy_squashed"_h);

    ctx->animation.SetAnimation("dying"_h, true, false);
    ctx->stunTimer.Start(0.5f, [this] -> void { ctx->moveComponent.m_pOwner->QueueDelete(); });

    // Disable all collisions
    ctx->playerHitbox.SetCollisionLayers(0);
    ctx->playerHitbox.SetCollisionMasks(0);
    ctx->headBurtBox.SetCollisionLayers(0);
    ctx->headBurtBox.SetCollisionMasks(0);
    ctx->feetHurtBox.SetCollisionLayers(0);
    ctx->feetHurtBox.SetCollisionMasks(0);
}

void Dying::Update() {}


// ==================== DISABLED ====================
void Disabled::OnEnter()
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.BindEvent("EnableEntities"_h, this, &Disabled::OnEnable);

    ctx->animation.SetPlaying(false);
    // ctx->moveComponent.LockOntoGround();
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

void Spawning::OnEnter()
{
    EnemyActiveState::OnEnter();

    if (ctx->initialWalkingDirection.x > 0)
        ctx->animation.SetAnimation("walkRight"_h, true);
    else if (ctx->initialWalkingDirection.x < 0)
        ctx->animation.SetAnimation("walkLeft"_h, true);
    else  // 0 initialWalkingDirection means it's a player controlled enemy
        machine->TransitionTo<IdleStanding>();

    ctx->aiDelayTimer.Start(spawnWalkingTime);
}

void Spawning::Update()
{
    if (not ctx->aiDelayTimer.IsRunning())
    {
        machine->TransitionTo<IdleStanding>();
        gla::Locator::Get<gla::EventManager>().InvokeEvent(gla::EntityEvent("FindNewDirection"_h, ctx->entityIndex));
    }

    ctx->moveComponent.SetDirection(ctx->initialWalkingDirection);
    ctx->moveComponent.Walk();
}

void Spawning::OnPepper(std::any const& /*collisionEvent*/) {}


}  // namespace bt::enemystates
