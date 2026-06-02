#ifndef BURGERTIME_ENEMYSTATES_HPP
#define BURGERTIME_ENEMYSTATES_HPP
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

struct StandingIdle;
struct Walking;
struct Climbing;
struct ClimbingIdle;
struct StunnedWalking;
struct StunnedClimbing;
struct Dying;

struct Context final
{
    gla::Animation* animation{};
    gla::Timer* timer{};
    //Enemy* enemy{};
    MoveComponent* moveComponent{};
};

using EnemyStateMachine = StateMachine<Context, StandingIdle, Walking, Climbing, ClimbingIdle, Dying, StunnedWalking, StunnedClimbing>;

struct StandingIdle final
{
    static void OnEnter(Context const& context);
    static void Update(EnemyStateMachine& machine, Context const& context);
    // void OnExit(Context const& context);
};

struct Walking final
{
    //static void OnEnter(Context const& context);
    static void Update(EnemyStateMachine& machine, Context const& context);
    // void OnExit(Context const& context);
};

struct Climbing final
{
    static void OnEnter(Context const& context);
    static void Update(EnemyStateMachine& machine, Context const& context);
};

struct ClimbingIdle final
{
    //static void OnEnter(Context const& context);
    static void Update(EnemyStateMachine& machine, Context const& context);
};

struct Falling final
{
    static void OnEnter(Context const& context);
    static void Update(EnemyStateMachine& machine, Context& context);
    // void OnExit(Context const& context);
};

struct StunnedWalking final
{
    static constexpr float stunTime{ 3.f };

    static void OnEnter(Context const& context);
    static void Update(EnemyStateMachine& machine, Context const& context);
    // void OnExit(Context const& context);
};

struct StunnedClimbing final
{
    static constexpr float stunTime{ 3.f };

    static void OnEnter(Context const& context);
    static void Update(EnemyStateMachine& machine, Context const& context);
    // void OnExit(Context const& context);
};

struct Dying final
{
    static void OnEnter(Context const& context);
    static void Update(EnemyStateMachine& machine, Context& context);
    // void OnExit(Context const& context);
};

}  // namespace enemystates
}  // namespace bt

#endif  // BURGERTIME_ENEMYSTATES_HPP
