#include "States/BurgerPartStates.hpp"

#include <array>
#include <cassert>
#include <glm/vec2.hpp>
#include <print>

#include "Components/BurgerPart.hpp"
#include "Components/CollisionRect.hpp"
#include "Components/Sprite.hpp"
#include "Components/Stage.hpp"
#include "Components/Timer.hpp"

namespace bt
{

void burgerpartstates::Idle::OnEnter(Context const& ctx)
{
    std::println("Burger entered Idle state");

    LockOntoGround(ctx.transform);
    ctx.part.SetSteppedPieces(0);
    ctx.timer.Start(BurgerPart::resetTime);
}

void burgerpartstates::Idle::Update(Context const& ctx)
{
    if (ctx.timer.IsFinished() and not hasReset)
    {
        for (auto const& collider : ctx.part.GetPieces() | std::views::keys)
            collider->CombineCollisionLayer(gla::Collider::Bits::Layer3 | gla::Collider::Bits::Layer4);

        hasReset = true;
    }

    if (ctx.part.GetSteppedPieces() >= BurgerPart::pieceCount)
        machine->TransitionTo<Falling>(ctx);
}

void burgerpartstates::Idle::LockOntoGround(gla::Transform& transform)
{
    static constexpr float platformOffset{ Stage::tileHeight - 2.f };

    auto const bottomBurgerPos = transform.GetLocalPosition() + glm::vec2{ 0.f, BurgerPart::pieceSize };

    auto const yOffsetIntoTile = static_cast<int>(bottomBurgerPos.y) % static_cast<int>(Stage::tileHeight);
    auto const bump = platformOffset - static_cast<float>(yOffsetIntoTile);

    transform.SetLocalPosition({ bottomBurgerPos.x, bottomBurgerPos.y + bump - BurgerPart::pieceSize });
}

void burgerpartstates::Falling::OnEnter(Context const& ctx)
{
    std::println("Burger entered Falling state");

    for (auto const& collider : ctx.part.GetPieces() | std::views::keys)
    {
        // Turn off player and burger collision layer
        collider->ClearCollisionLayer(gla::Collider::Bits::Layer3 | gla::Collider::Bits::Layer4);

        // Turn on enemy feet collision mask
        collider->CombineCollisionMask(gla::Collider::Bits::Layer6);
    }
}

void burgerpartstates::Falling::Update(Context const& ctx)
{
    if (not hasResetCollider)
    {
        for (auto const& collider : ctx.part.GetPieces() | std::views::keys)
            // Turn off enemy feet collision mask
            collider->ClearCollisionMask(gla::Collider::Bits::Layer6);

        hasResetCollider = true;
    }

    ctx.transform.ChangeLocalPosition({ 0.f, BurgerPart::fallingSpeed * ctx.deltaTime });
    if (IsOnPlatform(ctx.transform, ctx.stage))
        machine->TransitionTo<Idle>(ctx);
}

void burgerpartstates::Falling::OnExit(Context const& ctx)
{
    std::println("Burger exited Falling state");

    for (auto const& [collider, sprite] : ctx.part.GetPieces())
    {
        // Turn off falling collider
        collider->ClearCollisionMask(gla::Collider::Bits::Layer6);

        // Reset sprite y offset
        sprite->m_offset.y = 0;
    }

    ctx.part.ReleaseEnemies();
}

auto burgerpartstates::Falling::IsOnPlatform(gla::Transform const& transform, Stage const& stage) -> bool
{
    auto const worldPos = transform.GetLocalPosition();
    auto const bottomYPos = std::ceil(worldPos.y) + BurgerPart::pieceSize;
    auto const mod = static_cast<float>(static_cast<int>(bottomYPos + 1) % static_cast<int>(Stage::tileHeight));
    if (mod == Stage::tileHeight - 1.f)
    {
        auto const tileType = stage.GetTileAtPosition(
            glm::vec2{ worldPos.x, bottomYPos } + glm::vec2{ BurgerPart::pieceSize * (BurgerPart::pieceCount / 2.f), 1.f });
        if (tileType == Stage::TileType::Platform or tileType == Stage::TileType::LadderPlatform)
            return true;
    }

    return false;
}

void burgerpartstates::Finished::Update(Context const& /*ctx*/) {}

}  // namespace bt
