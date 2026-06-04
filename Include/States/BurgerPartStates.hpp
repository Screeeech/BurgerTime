#ifndef BURGERTIME_BURGERPARTSTATES_HPP
#define BURGERTIME_BURGERPARTSTATES_HPP
#include <array>

#include "StateMachine.hpp"

namespace gla
{
class Timer;
class Sprite;
class Transform;
class CollisionRect;
}  // namespace gla

namespace bt
{
class BurgerPart;
class Stage;

namespace burgerpartstates
{

struct Idle;
struct Falling;
struct Finished;

struct Context final
{
    BurgerPart& part;
    gla::Transform& transform;
    gla::Timer& timer;
    Stage& stage;
    float deltaTime{};
};

using BurgerStateMachine = StateMachine<Context, Idle, Falling, Finished>;

struct Idle final
{
    BurgerStateMachine* machine;

    bool hasReset{};

    static void OnEnter(Context const& ctx);
    void Update(Context const& ctx);

private:
    static void LockOntoGround(gla::Transform& transform);
};

struct Falling final
{
    BurgerStateMachine* machine;

    bool hasResetCollider{};

    static void OnEnter(Context const& ctx);
    void Update(Context const& ctx);
    static void OnExit(Context const& ctx);

private:
    static auto IsOnPlatform(gla::Transform const& transform, Stage const& stage) -> bool;
};

struct Finished final
{
    BurgerStateMachine* machine;

    // static void OnEnter(Context const& ctx);
    static void Update(Context const& ctx);
};


}  // namespace burgerpartstates

}  // namespace bt

#endif  // BURGERTIME_BURGERPARTSTATES_HPP
