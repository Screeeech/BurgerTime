#ifndef BURGERTIME_ENEMYSTATES_HPP
#define BURGERTIME_ENEMYSTATES_HPP
#include <any>
#include <glm/vec2.hpp>

#include "StateMachine.hpp"

namespace gla
{
class Timer;
class Animation;
}  // namespace gla

namespace bt
{
class MoveComponent;
class Enemy;
class Stage;

namespace enemystates
{

static constexpr float stunTime{ 3.f };

struct StandingIdle;
struct Walking;
struct Climbing;
struct ClimbingIdle;
struct StunnedWalking;
struct StunnedClimbing;
struct Falling;
struct Dying;

struct Context final
{
    gla::Animation* animation{};
    gla::Timer* timer{};
    // Enemy* enemy{};
    MoveComponent* moveComponent{};
    int entityIndex{};
};

using EnemyStateMachine = StateMachine<Context, StandingIdle, Walking, Climbing, ClimbingIdle, StunnedWalking, StunnedClimbing, Falling, Dying>;
//using EnemyStateMachine = StateMachine<Context, Dying>;

struct ReactiveState
{
    EnemyStateMachine* machine{};
    ReactiveState() = default;
    ReactiveState(ReactiveState const&) = default;
    ReactiveState(ReactiveState&&) = default;
    auto operator=(ReactiveState const&) -> ReactiveState& = default;
    auto operator=(ReactiveState&&) -> ReactiveState& = default;
    virtual ~ReactiveState() = default;

    int entityIndex{};

    virtual void OnEnter(Context const& ctx);
    virtual void OnExit(Context const& ctx);

private:
    virtual void OnPepper(std::any const& collisionEvent) = 0;
    void OnSquish(std::any const& collisionEvent);
    void OnDrop(std::any const& collisionEvent);
};

struct StandingIdle final : ReactiveState
{
    EnemyStateMachine* machine;
    int entityIndex;

    void OnEnter(Context const& ctx) override;
    void Update(Context const& ctx) const;
    //void OnExit(Context const& ctx) override;

private:
    void OnPepper(std::any const& collisionEvent) override;
};

struct Walking final : ReactiveState
{
    EnemyStateMachine* machine;
    int entityIndex;

    void Update(Context const& ctx) const;

private:
    void OnPepper(std::any const& collisionEvent) override;
};

struct Climbing final : ReactiveState
{
    EnemyStateMachine* machine;
    int entityIndex;

    void OnEnter(Context const& ctx) override;
    void Update(Context const& ctx) const;

private:
    void OnPepper(std::any const& collisionEvent) override;
};

struct ClimbingIdle final : ReactiveState
{
    EnemyStateMachine* machine;
    int entityIndex;

    void OnEnter(Context const& ctx) override;
    void Update(Context const& ctx);

private:
    void OnPepper(std::any const& collisionEvent) override;
};

struct StunnedWalking final : ReactiveState
{
    EnemyStateMachine* machine;
    int entityIndex;

    void OnEnter(Context const& ctx) override;
    void Update(Context const& ctx) const;

private:
    void OnPepper(std::any const& collisionEvent) override;
};

struct StunnedClimbing final : ReactiveState
{
    EnemyStateMachine* machine;
    int entityIndex;

    void OnEnter(Context const& ctx) override;
    void Update(Context const& ctx) const;

private:
    void OnPepper(std::any const& collisionEvent) override;
};

struct Falling final
{
    EnemyStateMachine* machine;
    int entityIndex;

    void OnEnter(Context const& ctx);
    void Update(Context& ctx);

private:
    void OnPepper(std::any const& collisionEvent);
};

struct Dying final
{
    EnemyStateMachine* machine;
    int entityIndex;

    static void OnEnter(Context const& ctx);
    static void Update(Context& ctx);

private:
    static void OnPepper(std::any const& collisionEvent);
};

}  // namespace enemystates
}  // namespace bt

#endif  // BURGERTIME_ENEMYSTATES_HPP
