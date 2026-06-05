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

struct IdleStanding;
struct Walking;
struct StunnedStanding;
struct IdleClimbing;
struct Climbing;
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

using EnemyStateMachine = StateMachine<Context, IdleStanding, Walking, Climbing, IdleClimbing, StunnedStanding, StunnedClimbing, Falling, Dying>;
using EnemyState = HelperState<Context, EnemyStateMachine>;

struct PepperEventState : EnemyState
{
    PepperEventState() = default;
    PepperEventState(PepperEventState const&) = default;
    PepperEventState(PepperEventState&&) = default;
    auto operator=(PepperEventState const&) -> PepperEventState& = default;
    auto operator=(PepperEventState&&) -> PepperEventState& = default;
    ~PepperEventState() override = default;

    int entityIndex{};
    gla::Animation* animation{};
    gla::Timer* timer{};

    virtual void OnEnter(Context const& ctx);
    virtual void OnExit(Context const& ctx);

private:
    virtual void OnPepper(std::any const& collisionEvent) = 0;
    // void OnSquish(std::any const& collisionEvent);
    // void OnDrop(std::any const& collisionEvent);
};

struct IdleStanding final : PepperEventState
{
    void OnEnter(Context const& ctx) override;
    void Update(Context const& ctx) override;
    // void OnExit(Context const& ctx) override;

private:
    void OnPepper(std::any const& collisionEvent) override;
};

struct Walking final : PepperEventState
{
    void Update(Context const& ctx) override;

private:
    void OnPepper(std::any const& collisionEvent) override;
};

struct Climbing final : PepperEventState
{
    void OnEnter(Context const& ctx) override;
    void Update(Context const& ctx) override;

private:
    void OnPepper(std::any const& collisionEvent) override;
};

struct IdleClimbing final : PepperEventState
{
    void OnEnter(Context const& ctx) override;
    void Update(Context const& ctx) override;

private:
    void OnPepper(std::any const& collisionEvent) override;
};

struct StunnedStanding final : EnemyState
{
    static void OnEnter(Context const& ctx);
    void Update(Context const& ctx) override;
};

struct StunnedClimbing final : EnemyState
{
    static void OnEnter(Context const& ctx);
    void Update(Context const& ctx) override;
};

struct Falling final : EnemyState
{
    static void OnEnter(Context const& ctx);
    void Update(Context const& ctx) override;
};

struct Dying final : EnemyState
{
    static void OnEnter(Context const& ctx);
    void Update(Context const& ctx) override;
};

}  // namespace enemystates
}  // namespace bt

#endif  // BURGERTIME_ENEMYSTATES_HPP
