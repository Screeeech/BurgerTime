#ifndef BURGERTIME_BURGERPART_HPP
#define BURGERTIME_BURGERPART_HPP
#include <SDL3/SDL_rect.h>

#include <array>
#include <memory>

#include "Component.hpp"
#include "Renderable.hpp"

namespace gla
{
class Collider;
class Sprite;
class Texture2D;
class CollisionRect;
class Animation;
}  // namespace gla
namespace bt
{
class MoveComponent;
class Stage;

class BurgerPart final : public gla::Renderable
{
public:
    enum class Piece : std::uint8_t
    {
        TopBun,
        BottomBun,
        Cheese,
        Patty,
        Tomato,
        Lettuce,
    };

    explicit BurgerPart(gla::GameObject* pOwner, Stage* pStage, Piece pieceType, std::shared_ptr<gla::Texture2D> const& spriteSheetTexture);

protected:
    void FixedUpdate(float fixedDeltaTime) override;
    void Render() override;

private:
    static constexpr float pieceStepOffset{ 2.f };
    static constexpr float pieceSize{ 8.f };
    static constexpr float fallingSpeed{ 25.f };
    static constexpr int pieceCount{ 4 };

    static auto GetBurgerPieceSourceRect(Piece type, long index) -> SDL_FRect;
    void OnPieceStep(gla::Collider const& collider, long index);
    auto IsOnPlatform() const -> bool;
    void LockOntoGround() const;

    std::array<std::pair<gla::CollisionRect*, gla::Sprite*>, pieceCount> m_pieces{};
    int m_steppedPieces{};
    Stage* m_pStage;
    //MoveComponent* m_pMoveComponent;
};

}  // namespace bt

#endif  // BURGERTIME_BURGERPART_HPP
