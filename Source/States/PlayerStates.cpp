#include "States/PlayerStates.hpp"

#include <cassert>
#include <glm/gtc/constants.hpp>
#include <print>

#include "Utils.hpp"

namespace bt::playerstates
{

// ==================== IDLE ====================
void Idle::Update(PlayerStateMachine& machine, Context const& context)
{
    if (context.direction != glm::zero<glm::vec3>())
        machine.TransitionTo<Walking>({
            .direction = context.direction,
            .animation = context.animation,
        });
}

void Idle::OnEnter(Context const& context)
{
    assert(context.animation != nullptr && "Animation cannot be null!");

    context.animation->SetActiveAnimation("idle"_h, true);
}

// ==================== WALKING ====================
void Walking::Update(PlayerStateMachine& machine, Context const& context)
{
    assert(context.animation != nullptr && "Animation cannot be null!");
    if (context.direction == glm::zero<glm::vec3>())
    {
        machine.TransitionTo<Idle>({ .animation = context.animation });
        return;
    }

    assert(context.direction != glm::zero<glm::vec3>() && "Direction cannot be zero vector");
    ChangeAnimation(context);

}
void Walking::OnEnter(Context const& context)
{
    assert(context.animation != nullptr && "Animation cannot be null!");
    assert(context.direction != glm::zero<glm::vec3>() && "Direction cannot be zero vector");

    ChangeAnimation(context);
}

void Walking::ChangeAnimation(Context const& context)
{
    auto const& direction = context.direction;
    auto const& animation = context.animation;

    if (direction.y < 0.f)
        animation->SetActiveAnimation("walkUp"_h, true);
    else if (direction.y > 0.f)
        animation->SetActiveAnimation("walkDown"_h, true);
    else if (direction.x > 0.f)
        animation->SetActiveAnimation("walkRight"_h, true);
    else if (direction.x < 0.f)
        animation->SetActiveAnimation("walkLeft"_h, true);
}

// ==================== CLIMBING ====================
void Climbing::Update([[maybe_unused]] PlayerStateMachine& machine, [[maybe_unused]] Context const& context)
{
}

}  // namespace bt::playerstates