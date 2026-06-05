#ifndef BURGERTIME_BURGERPARTSTATES_HPP
#define BURGERTIME_BURGERPARTSTATES_HPP
#include <array>

#include "StateMachine.hpp"

namespace gla
{
class Collider;
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
};

using BurgerStateMachine = StateMachine<Context, Idle, Falling, Finished>;
using BurgerState = HelperState<Context, BurgerStateMachine>;

struct Idle final : BurgerState
{
    bool hasReset{};

    void OnEnter() const;
    void Update() override;

private:
    static void LockOntoGround(gla::Transform& transform);
};

struct Falling final : BurgerState
{
    bool hasResetCollider{};

    void OnEnter() const;
    void Update() override;
    void OnExit() const;

private:
    static auto IsOnPlatform(gla::Transform const& transform, Stage const& stage) -> bool;
};

struct Finished final : BurgerState
{
    // void OnEnter();
    void Update() override;
};


}  // namespace burgerpartstates

}  // namespace bt

#endif  // BURGERTIME_BURGERPARTSTATES_HPP
