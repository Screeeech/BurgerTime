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
struct StunnedWalking;
struct Dying;


namespace
{

void UnbindAllEvents(void* instance)
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.UnbindEvents(instance);
}

// template<typename T>
// void BindPepperEvent(T* instance, int entityIndex)
//{
//     instance->entityIndex = entityIndex;
//     auto& eventManager = gla::Locator::Get<gla::EventManager>();
//     eventManager.BindEvent("OnPepper"_h, instance, &T::OnPepper);
// }

auto IsCorrectCollider(int entityIndex, gla::Collider& collider) -> bool
{
    auto const* otherEnemy = collider.m_pOwner->GetComponent<Enemy>();
    assert(otherEnemy and "The collider's GameObject must also have a valid Enemy component");

    return otherEnemy->m_entityIndex == entityIndex;
}

}  // namespace

void ReactiveState::OnEnter(Context const& ctx)
{
    entityIndex = ctx.entityIndex;
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.BindEvent("OnPepper"_h, this, &ReactiveState::OnPepper);
}

void ReactiveState::OnExit(Context const& /*ctx*/)
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.UnbindEvents(this);
}


// ==================== STANDING IDLE ====================
void StandingIdle::OnEnter(Context const& ctx)
{
    ReactiveState::OnEnter(ctx);

    assert(ctx.animation and "Animation cannot be null");
    ctx.animation->SetAnimation("idle"_h, true);

    if (ctx.moveComponent)
        ctx.moveComponent->LockOntoGround();
}

void StandingIdle::Update(EnemyStateMachine& machine, Context const& ctx)
{
    assert(ctx.moveComponent and "MoveComponent cannot be null");

    if ((ctx.moveComponent->GetDirection().y < 0 and ctx.moveComponent->CanClimbUp()) or
        (ctx.moveComponent->GetDirection().y > 0 and ctx.moveComponent->CanClimbDown()))
    {
        machine.TransitionTo<Climbing>();
        return;
    }

    if ((ctx.moveComponent->GetDirection().x != 0.f and ctx.moveComponent->CanWalk()))
    {
        machine.TransitionTo<Walking>(ctx);
        return;
    }
}

void StandingIdle::OnPepper(std::any const& collisionEvent)
{
    auto const& args = std::any_cast<gla::CollisionEvent const&>(collisionEvent);
    if (not IsCorrectCollider(entityIndex, *args.pCollider))
        return;
}

// ==================== WALKING ====================
void Walking::Update(EnemyStateMachine& machine, Context const& ctx)
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
        machine.TransitionTo<Climbing>(ctx);
        return;
    }
    if (ctx.moveComponent->GetDirection().x == 0.f or not ctx.moveComponent->CanWalk())
    {
        machine.TransitionTo<StandingIdle>(ctx);
        return;
    }

    ctx.moveComponent->Walk();
}

void Walking::OnPepper(std::any const& /*collisionEvent*/) {}


// ==================== CLIMBING ====================
void Climbing::OnEnter(Context const& ctx)
{
    ReactiveState::OnEnter(ctx);

    if (ctx.moveComponent)
        ctx.moveComponent->LockOntoLadder();
}

void Climbing::Update(EnemyStateMachine& machine, Context const& ctx)
{
    assert(ctx.animation and "Animation cannot be null");
    assert(ctx.moveComponent and "MoveComponent cannot be null");

    // clang-format off
    if (ctx.moveComponent->IsOnGround() and
        ((ctx.moveComponent->GetDirection().y == 0) or
        (ctx.moveComponent->GetDirection().y < 0 and not ctx.moveComponent->CanClimbUp()) or
        (ctx.moveComponent->GetDirection().y > 0 and not ctx.moveComponent->CanClimbDown())))
    {
        machine.TransitionTo<StandingIdle>(ctx);
        return;
    }
    // clang-format on

    if (ctx.moveComponent->GetDirection().y == 0)
    {
        machine.TransitionTo<ClimbingIdle>(ctx);
        return;
    }


    if (ctx.moveComponent->GetDirection().y < 0)
        ctx.animation->SetAnimation("walkUp"_h, true);
    else if (ctx.moveComponent->GetDirection().y > 0)
        ctx.animation->SetAnimation("walkDown"_h, true);

    ctx.moveComponent->Climb();
}

void Climbing::OnPepper(std::any const& /*collisionEvent*/) {}


// ==================== STUNNED WALKING ====================
void StunnedWalking::OnEnter(Context const& ctx)
{
    ReactiveState::OnEnter(ctx);

    assert(ctx.animation and "Animation cannot be null");
    ctx.animation->SetAnimation("stunned"_h, true);

    assert(ctx.timer and "Timer cannot be null");
    ctx.timer->Start(stunTime);
}

void StunnedWalking::Update(EnemyStateMachine& machine, Context const& ctx)
{
    assert(ctx.timer and "Timer cannot be null");

    if (ctx.timer->IsFinished())
        machine.TransitionTo<StandingIdle>(ctx);
}

void StunnedWalking::OnPepper(std::any const& /*collisionEvent*/) {}


// ==================== STUNNED CLIMBING ====================
void StunnedClimbing::OnEnter(Context const& ctx)
{
    ReactiveState::OnEnter(ctx);

    assert(ctx.animation and "Animation cannot be null");
    ctx.animation->SetAnimation("stunned"_h, true);

    assert(ctx.timer and "Timer cannot be null");
    ctx.timer->Start(stunTime);
}

void StunnedClimbing::Update(EnemyStateMachine& machine, Context const& ctx)
{
    assert(ctx.timer and "Timer cannot be null");

    if (ctx.timer->IsFinished())
        machine.TransitionTo<ClimbingIdle>(ctx);
}

void StunnedClimbing::OnPepper(std::any const& /*collisionEvent*/) {}


// ==================== DYING ====================
void Dying::OnEnter(Context const& ctx)
{
    assert(ctx.animation and "Animation cannot be null");
    // ctx.animation->SetAnimation("dying"_h, true);
}

void Dying::Update([[maybe_unused]] EnemyStateMachine& machine, [[maybe_unused]] Context& ctx) {}

void Dying::OnExit(Context const& /*ctx*/)
{
    UnbindAllEvents(this);
}

void Dying::OnPepper(std::any const& /*collisionEvent*/) {}


// ==================== CLIMBING IDLE ====================
void ClimbingIdle::OnEnter(Context const& ctx)
{
    ReactiveState::OnEnter(ctx);

    assert(ctx.animation and "Animation cannot be null");
    ctx.animation->SetAnimation("idle"_h);
}
void ClimbingIdle::Update(EnemyStateMachine& machine, Context const& ctx)
{
    assert(ctx.moveComponent and "MoveComponent cannot be null");

    if (ctx.moveComponent->GetDirection().y != 0)
        machine.TransitionTo<Climbing>(ctx);
}
void ClimbingIdle::OnPepper(std::any const& /*collisionEvent*/) {}


// ==================== FALLING ====================
void Falling::OnEnter(Context const& ctx)
{
    assert(ctx.animation and "Animation cannot be null");
    ctx.animation->SetAnimation("idle"_h, true);
}
void Falling::Update(EnemyStateMachine& /*machine*/, Context& /*ctx*/) {}
void Falling::OnExit(Context const& /*ctx*/)
{
    UnbindAllEvents(this);
}

void Falling::OnPepper(std::any const& /*collisionEvent*/) {}

}  // namespace bt::enemystates
