#include "States/EnemyStates.hpp"

#include <cassert>

#include "Components/Animation.hpp"
#include "Components/Collider.hpp"
#include "Components/Enemy.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/Stage.hpp"
#include "Components/Timer.hpp"
#include "Events.hpp"
#include "Services/EventManager.hpp"
#include "States/PlayerStates.hpp"
#include "Utils.hpp"

namespace bt::enemystates
{

struct Walking;
struct Climbing;
struct StunnedStanding;
struct Dying;


namespace
{
constexpr float stunTime{ 3.f };

auto IsCorrectCollider(int entityIndex, gla::Collider const& collider) -> bool
{
    auto const* enemy = collider.m_pOwner->GetComponent<Enemy>();
    assert(enemy and "The collider's GameObject must also have a valid Enemy component");

    return enemy->m_entityIndex == entityIndex;
}

}  // namespace

void PepperEventState::OnEnter()
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.BindEvent("OnPepper"_h, this, &PepperEventState::OnPepper);
}

void PepperEventState::OnExit()
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.UnbindEvent("OnPepper"_h, this);
}

// ==================== STANDING IDLE ====================
void IdleStanding::OnEnter()
{
    PepperEventState::OnEnter();

    std::println("Enemy entered idle standing state");

    ctx->animation.SetAnimation("idle"_h, true);
    ctx->moveComponent.LockOntoGround();
}

void IdleStanding::Update()
{

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

void IdleStanding::OnPepper(std::any const& collisionEvent)
{
    auto const& args = std::any_cast<gla::CollisionEvent const&>(collisionEvent);
    if (not IsCorrectCollider(ctx->entityIndex, *args.pCollider))
        return;

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

void Walking::OnPepper(std::any const& collisionEvent)
{
    auto const& args = std::any_cast<gla::CollisionEvent const&>(collisionEvent);
    if (not IsCorrectCollider(ctx->entityIndex, *args.pCollider))
        return;

    machine->TransitionTo<StunnedStanding>();
}


// ==================== CLIMBING ====================
void Climbing::OnEnter()
{
    PepperEventState::OnEnter();

    std::println("Enemy entered climbing state");

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

void Climbing::OnPepper(std::any const& collisionEvent)
{
    auto const& args = std::any_cast<gla::CollisionEvent const&>(collisionEvent);
    if (not IsCorrectCollider(ctx->entityIndex, *args.pCollider))
        return;

    machine->TransitionTo<StunnedClimbing>();
}


// ==================== STUNNED WALKING ====================
void StunnedStanding::OnEnter() const
{
    std::println("Enemy entered stunned standing state");

    // assert(ctx->animation and "Animation cannot be null");
    ctx->animation.SetAnimation("stunned"_h, true);

    // assert(ctx->stunTimer and "Timer cannot be null");
    ctx->stunTimer.Start(stunTime);

    // assert(ctx->hitBox and "HitBox cannot be null");
    ctx->playerHitbox.DisableCollisionMasks(gla::Collider::Bits::Layer2);
}

void StunnedStanding::Update()
{
    // assert(ctx->stunTimer and "Timer cannot be null");

    if (ctx->stunTimer.IsFinished())
        machine->TransitionTo<IdleStanding>();
}

void StunnedStanding::OnExit() const
{
    // assert(ctx->hitBox and "HitBox cannot be null");
    ctx->playerHitbox.EnableCollisionMasks(gla::Collider::Bits::Layer2);
}


// ==================== STUNNED CLIMBING ====================
void StunnedClimbing::OnEnter() const
{
    std::println("Enemy entered stunned climbing state");

    // assert(ctx->animation and "Animation cannot be null");
    ctx->animation.SetAnimation("stunned"_h, true);

    // assert(ctx->stunTimer and "Timer cannot be null");
    ctx->stunTimer.Start(stunTime);

    // assert(ctx->hitBox and "HitBox cannot be null");
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


// ==================== DYING ====================
void Dying::OnEnter() const
{
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

// ==================== CLIMBING IDLE ====================
void IdleClimbing::OnEnter()
{
    PepperEventState::OnEnter();

    ctx->animation.SetAnimation("idle"_h);
}
void IdleClimbing::Update()
{
    if (ctx->moveComponent.GetDirection().y != 0)
        machine->TransitionTo<Climbing>();
}

void IdleClimbing::OnPepper(std::any const& collisionEvent)
{
    auto const& args = std::any_cast<gla::CollisionEvent const&>(collisionEvent);
    if (not IsCorrectCollider(ctx->entityIndex, *args.pCollider))
        return;

    machine->TransitionTo<StunnedClimbing>();
}


// ==================== FALLING ====================
void Falling::OnEnter() const
{
    std::println("Enemy entered falling state");

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
    ctx->playerHitbox.EnableCollisionMasks(gla::Collider::Bits::Layer2);
}


}  // namespace bt::enemystates
