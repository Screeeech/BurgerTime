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

auto IsCorrectCollider(int entityIndex, gla::Collider& collider) -> bool
{
    auto const* enemy = collider.m_pOwner->GetComponent<Enemy>();
    assert(enemy and "The collider's GameObject must also have a valid Enemy component");

    return enemy->m_entityIndex == entityIndex;
}

}  // namespace

void PepperEventState::OnEnter(Context const& ctx)
{
    entityIndex = ctx.entityIndex;
    animation = ctx.animation;
    timer = ctx.timer;

    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.BindEvent("OnPepper"_h, this, &PepperEventState::OnPepper);
}

void PepperEventState::OnExit(Context const& /*ctx*/)
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.UnbindEvent("OnPepper"_h, this);
}

// ==================== STANDING IDLE ====================
void IdleStanding::OnEnter(Context const& ctx)
{
    PepperEventState::OnEnter(ctx);

    assert(ctx.animation and "Animation cannot be null");
    ctx.animation->SetAnimation("idle"_h, true);

    if (ctx.moveComponent)
        ctx.moveComponent->LockOntoGround();
}

void IdleStanding::Update(Context const& ctx)
{
    assert(ctx.moveComponent and "MoveComponent cannot be null");

    if ((ctx.moveComponent->GetDirection().y < 0 and ctx.moveComponent->CanClimbUp()) or
        (ctx.moveComponent->GetDirection().y > 0 and ctx.moveComponent->CanClimbDown()))
    {
        machine->TransitionTo<Climbing>(ctx);
        return;
    }

    if ((ctx.moveComponent->GetDirection().x != 0.f and ctx.moveComponent->CanWalk()))
    {
        machine->TransitionTo<Walking>(ctx);
        return;
    }
}

void IdleStanding::OnPepper(std::any const& collisionEvent)
{
    auto const& args = std::any_cast<gla::CollisionEvent const&>(collisionEvent);
    if (not IsCorrectCollider(entityIndex, *args.pCollider))
        return;

    machine->TransitionTo<StunnedStanding>({ .animation = animation, .timer = timer, .entityIndex = entityIndex });
}

// ==================== WALKING ====================
void Walking::Update(Context const& ctx)
{
    assert(ctx.animation and "Animation cannot be null");
    assert(ctx.moveComponent and "MoveComponent cannot be null");

    if (ctx.moveComponent->GetDirection().x < 0)
        ctx.animation->SetAnimation("walkLeft"_h, true);
    if (ctx.moveComponent->GetDirection().x > 0)
        ctx.animation->SetAnimation("walkRight"_h, true);


    if ((ctx.moveComponent->GetDirection().y < 0.f and ctx.moveComponent->CanClimbUp()) or
        (ctx.moveComponent->GetDirection().y > 0.f and ctx.moveComponent->CanClimbDown()))
    {
        machine->TransitionTo<Climbing>(ctx);
        return;
    }
    if (ctx.moveComponent->GetDirection().x == 0.f or not ctx.moveComponent->CanWalk())
    {
        machine->TransitionTo<IdleStanding>(ctx);
        return;
    }

    ctx.moveComponent->Walk();
}

void Walking::OnPepper(std::any const& collisionEvent)
{
    auto const& args = std::any_cast<gla::CollisionEvent const&>(collisionEvent);
    if (not IsCorrectCollider(entityIndex, *args.pCollider))
        return;

    machine->TransitionTo<StunnedStanding>({ .animation = animation, .timer = timer, .entityIndex = entityIndex });
}


// ==================== CLIMBING ====================
void Climbing::OnEnter(Context const& ctx)
{
    PepperEventState::OnEnter(ctx);

    if (ctx.moveComponent)
        ctx.moveComponent->LockOntoLadder();
}

void Climbing::Update(Context const& ctx)
{
    assert(ctx.animation and "Animation cannot be null");
    assert(ctx.moveComponent and "MoveComponent cannot be null");

    // clang-format off
    if (ctx.moveComponent->IsOnGround() and
        ((ctx.moveComponent->GetDirection().y == 0) or
        (ctx.moveComponent->GetDirection().y < 0 and not ctx.moveComponent->CanClimbUp()) or
        (ctx.moveComponent->GetDirection().y > 0 and not ctx.moveComponent->CanClimbDown())))
    {
        machine->TransitionTo<IdleStanding>(ctx);
        return;
    }
    // clang-format on

    if (ctx.moveComponent->GetDirection().y == 0)
    {
        machine->TransitionTo<IdleClimbing>(ctx);
        return;
    }


    if (ctx.moveComponent->GetDirection().y < 0)
        ctx.animation->SetAnimation("walkUp"_h, true);
    else if (ctx.moveComponent->GetDirection().y > 0)
        ctx.animation->SetAnimation("walkDown"_h, true);

    ctx.moveComponent->Climb();
}

void Climbing::OnPepper(std::any const& collisionEvent)
{
    auto const& args = std::any_cast<gla::CollisionEvent const&>(collisionEvent);
    if (not IsCorrectCollider(entityIndex, *args.pCollider))
        return;

    machine->TransitionTo<StunnedClimbing>({ .animation = animation, .timer = timer, .entityIndex = entityIndex });
}


// ==================== STUNNED WALKING ====================
void StunnedStanding::OnEnter(Context const& ctx)
{
    assert(ctx.animation and "Animation cannot be null");
    ctx.animation->SetAnimation("stunned"_h, true);

    assert(ctx.timer and "Timer cannot be null");
    ctx.timer->Start(stunTime);
}

void StunnedStanding::Update(Context const& ctx)
{
    assert(ctx.timer and "Timer cannot be null");

    if (ctx.timer->IsFinished())
        machine->TransitionTo<IdleStanding>(ctx);
}


// ==================== STUNNED CLIMBING ====================
void StunnedClimbing::OnEnter(Context const& ctx)
{
    assert(ctx.animation and "Animation cannot be null");
    ctx.animation->SetAnimation("stunned"_h, true);

    assert(ctx.timer and "Timer cannot be null");
    ctx.timer->Start(stunTime);
}

void StunnedClimbing::Update(Context const& ctx)
{
    assert(ctx.timer and "Timer cannot be null");

    if (ctx.timer->IsFinished())
        machine->TransitionTo<IdleClimbing>(ctx);
}


// ==================== DYING ====================
void Dying::OnEnter([[maybe_unused]] Context const& ctx)
{
    assert(ctx.animation and "Animation cannot be null");
    // ctx.animation->SetAnimation("dying"_h, true);
}
void Dying::Update(Context const& /*ctx*/) {}

// ==================== CLIMBING IDLE ====================
void IdleClimbing::OnEnter(Context const& ctx)
{
    PepperEventState::OnEnter(ctx);

    assert(ctx.animation and "Animation cannot be null");
    ctx.animation->SetAnimation("idle"_h);
}
void IdleClimbing::Update(Context const& ctx)
{
    assert(ctx.moveComponent and "MoveComponent cannot be null");

    if (ctx.moveComponent->GetDirection().y != 0)
        machine->TransitionTo<Climbing>(ctx);
}

void IdleClimbing::OnPepper(std::any const& collisionEvent)
{
    auto const& args = std::any_cast<gla::CollisionEvent const&>(collisionEvent);
    if (not IsCorrectCollider(entityIndex, *args.pCollider))
        return;

    machine->TransitionTo<StunnedClimbing>({ .animation = animation, .timer = timer, .entityIndex = entityIndex });
}


// ==================== FALLING ====================
void Falling::OnEnter(Context const& ctx)
{
    assert(ctx.animation and "Animation cannot be null");
    ctx.animation->SetAnimation("idle"_h, true);
}
void Falling::Update(Context const& /*ctx*/) {}


}  // namespace bt::enemystates
