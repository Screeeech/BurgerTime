#ifndef BURGERTIME_BURGERPARTSTATES_HPP
#define BURGERTIME_BURGERPARTSTATES_HPP
#include <array>

#include "StateMachine.hpp"

namespace gla
{
class Sprite;
class Transform;
class CollisionRect;
}

namespace bt
{
class Stage;

namespace burgerpartstates
{

struct Idle;
struct Falling;
struct Finished;

struct Context final
{
    gla::Transform& transform;
    int& steppedPieces;
    float deltaTime{};
    Stage& stage;
    std::array<std::pair<gla::CollisionRect*, gla::Sprite*>, 4> const* pieces{};
};

using BurgerStateMachine = StateMachine<Context, Idle, Falling, Finished>;

struct Idle final
{
    static void OnEnter(Context const& context);
    static void Update(BurgerStateMachine& machine, Context const& context);

private:
    static void LockOntoGround(gla::Transform& transform);
};

struct Falling final
{
    static void Update(BurgerStateMachine& machine, Context const& context);

private:
    static auto IsOnPlatform(gla::Transform const& transform, Stage const& stage) -> bool;
};

struct Finished final
{
    //static void OnEnter(Context const& context);
    static void Update(BurgerStateMachine& machine, Context const& context);
};


}  // namespace burgerpartstates

}  // namespace bt

#endif  // BURGERTIME_BURGERPARTSTATES_HPP
