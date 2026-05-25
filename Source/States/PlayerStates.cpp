#include "States/PlayerStates.hpp"

#include <cassert>
#include <glm/gtc/constants.hpp>
#include <print>

#include "Colors.hpp"
#include "Components/PlayerController.hpp"
#include "Components/Stage.hpp"
#include "ServiceLocator.hpp"
#include "Utils.hpp"

namespace bt::playerstates
{

// ==================== IDLE ====================
void Idle::Update(PlayerStateMachine& machine, Context const& context)
{
    auto const& [direction, position, animation, stage, player] = context;
    assert(stage != nullptr and "Stage cannot be null!");

    if (direction != glm::zero<glm::vec3>() and stage->CanWalk(position, direction))
        machine.TransitionTo<Walking>({ .direction = direction, .animation = animation });
}

void Idle::OnEnter(Context const& context)
{
    assert(context.animation != nullptr and "Animation cannot be null!");

    std::println("Entered idle!");
    context.animation->SetActiveAnimation("idle"_h, true);
}

// ==================== WALKING ====================
void Walking::Update(PlayerStateMachine& machine, Context& context)
{
    auto& [direction, position, animation, stage, player] = context;
    assert(animation != nullptr and "Animation cannot be null!");

    if (direction == glm::zero<glm::vec3>() or not stage->CanWalk(position, direction))
    {
        machine.TransitionTo<Idle>({ .animation = context.animation });
        return;
    }
    if ((direction.y < 0.f and stage->CanClimbUp(position)) /* or (direction.y > 0.f and stage->CanClimbDown(position))*/)
    {
        machine.TransitionTo<Climbing>({ .animation = context.animation });
        return;
    }

    direction.y = 0.f;
    player->Move(direction);

    stage->PrintTileType(position);

    ChangeAnimation(context);
}

void Walking::OnEnter(Context const& context)
{
    std::println("Entered walking!");
    ChangeAnimation(context);
}

void Walking::ChangeAnimation(Context const& context)
{
    assert(context.animation != nullptr and "Animation cannot be null!");

    auto const& direction = context.direction;
    auto const& animation = context.animation;

    if (direction.x > 0.f)
        animation->SetActiveAnimation("walkRight"_h, true);
    else if (direction.x < 0.f)
        animation->SetActiveAnimation("walkLeft"_h, true);
    // else if (direction.y < 0.f)
    //     animation->SetActiveAnimation("walkUp"_h, true);
    // else if (direction.y > 0.f)
    //     animation->SetActiveAnimation("walkDown"_h, true);
}

// ==================== CLIMBING ====================
// TODO: Climbing state is unfinished

void Climbing::Update(PlayerStateMachine& machine, Context& context)
{
    auto& [direction, position, animation, stage, player] = context;
    assert(stage != nullptr and "Stage cannot be null!");
    assert(animation != nullptr and "Animation cannot be null!");

    animation->SetPlaying(direction.y == 0.f);

    direction.x = 0.f;
    player->Move(direction);

    stage->PrintTileType(position);

    if (stage->IsOnGround(position))
    {
        machine.TransitionTo<Idle>({ .animation = context.animation });
    }
}

void Climbing::OnEnter(Context const& context)
{
    std::println("Entered climbing!");
    ChangeAnimation(context);
}

void Climbing::ChangeAnimation(Context const& context)
{
    auto const& animation = context.animation;
    auto const& direction = context.direction;

    assert(animation != nullptr and "Animation cannot be null!");

    if (direction.y < 0.f)
        animation->SetActiveAnimation("walkUp"_h, true);
    else if (direction.y > 0.f)
        animation->SetActiveAnimation("walkDown"_h, true);
}

}  // namespace bt::playerstates