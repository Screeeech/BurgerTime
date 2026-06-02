#ifndef BURGERTIME_BURGERPART_HPP
#define BURGERTIME_BURGERPART_HPP
#include <SDL3/SDL_rect.h>

#include <array>
#include <memory>

#include "Component.hpp"

namespace gla
{
class Sprite;
class Texture2D;
class CollisionRect;
class Animation;
}  // namespace gla
namespace bt
{
class Stage;

class BurgerPart final : public gla::Component
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

    explicit BurgerPart(gla::GameObject* pOwner, Piece pieceType, std::shared_ptr<gla::Texture2D> const& spriteSheetTexture);

private:
    static constexpr float pieceStepOffset{ 2.f };
    static constexpr float pieceSize{ 8.f };
    static constexpr int pieceCount{ 4 };

    static auto GetBurgerPieceSourceRect(Piece type, long index) -> SDL_FRect;
    void OnPieceStep(long index);

    std::array<std::pair<gla::CollisionRect*, gla::Sprite*>, pieceCount> m_pieces{};
    int m_steppedPieces{};
};

}  // namespace bt

#endif  // BURGERTIME_BURGERPART_HPP
