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
class Enemy;
class Stage;

namespace enemystates
{

struct Walking;
struct Climbing;
struct StunnedWalking;
struct StunnedClimbing;
struct Dying;

struct Context final
{
    glm::vec2 direction{};
    glm::vec2 position{};
    gla::Animation* animation{};
    gla::Timer* timer{};
    Enemy* enemy{};
    Stage* stage{};
};

using EnemyStateMachine = StateMachine<Walking, Context, Walking, Climbing, Dying, StunnedWalking, StunnedClimbing>;

struct Walking final
{
    static void OnEnter(Context const& context);
    static void Update(EnemyStateMachine& machine, Context const& context);
    // void OnExit(Context const& context);
};

struct Climbing final
{
    static void OnEnter(Context const& context);
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
