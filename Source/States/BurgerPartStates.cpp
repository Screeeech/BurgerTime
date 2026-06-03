#include "States/BurgerPartStates.hpp"

#include <glm/vec2.hpp>

#include "Components/BurgerPart.hpp"
#include "Components/CollisionRect.hpp"
#include "Components/Sprite.hpp"
#include "Components/Stage.hpp"

namespace bt
{

void burgerpartstates::Idle::OnEnter(Context const& context)
{
    LockOntoGround(context.transform);
    context.steppedPieces = 0;

    if (context.pieces)
    {
        for (auto const& [collider, sprite] : *context.pieces)
        {
            collider->m_collisionLayers |= gla::Collider::Bits::Layer3;
            sprite->m_offset.y = 0;
        }
    }
}

void burgerpartstates::Idle::Update(BurgerStateMachine& machine, Context const& context)
{
    if (context.steppedPieces >= BurgerPart::pieceCount)
        machine.TransitionTo<Falling>(context);
}

void burgerpartstates::Idle::LockOntoGround(gla::Transform& transform)
{
    static constexpr float platformOffset{ Stage::tileHeight - 2.f };

    auto const bottomBurgerPos = transform.GetLocalPosition() + glm::vec2{ 0.f, BurgerPart::pieceSize };

    auto const yOffsetIntoTile = static_cast<int>(bottomBurgerPos.y) % static_cast<int>(Stage::tileHeight);
    auto const bump = platformOffset - static_cast<float>(yOffsetIntoTile);

    transform.SetLocalPosition({ bottomBurgerPos.x, bottomBurgerPos.y + bump - BurgerPart::pieceSize });
}

void burgerpartstates::Falling::Update(BurgerStateMachine& machine, Context const& context)
{
    auto const& [transform, steppedPieces, deltaTime, stage, pieces] = context;

    transform.ChangeLocalPosition({ 0.f, BurgerPart::fallingSpeed * deltaTime });
    if (IsOnPlatform(transform, stage))
        machine.TransitionTo<Idle>(context);
}

auto burgerpartstates::Falling::IsOnPlatform(gla::Transform const& transform, Stage const& stage) -> bool
{
    auto const worldPos = transform.GetLocalPosition();
    auto const bottomYPos = std::ceil(worldPos.y) + BurgerPart::pieceSize;
    auto const mod = static_cast<float>(static_cast<int>(bottomYPos + 1) % static_cast<int>(Stage::tileHeight));
    if (mod == Stage::tileHeight - 1.f)
    {
        auto const tileType = stage.GetTileAtPosition(glm::vec2{ worldPos.x, bottomYPos } + glm::vec2{ 0.f, 1.f });
        if (tileType == Stage::TileType::Platform or tileType == Stage::TileType::LadderPlatform)
            return true;
    }

    return false;
}

void burgerpartstates::Finished::Update(BurgerStateMachine& /*machine*/, Context const& /*context*/) {}

}  // namespace bt
