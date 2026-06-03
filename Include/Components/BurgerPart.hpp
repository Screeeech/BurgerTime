#ifndef BURGERTIME_BURGERPART_HPP
#define BURGERTIME_BURGERPART_HPP
#include <SDL3/SDL_rect.h>

#include <array>
#include <memory>

#include "Component.hpp"
#include "States/BurgerPartStates.hpp"

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

class BurgerPart final : public gla::Component
{
public:
    enum class Type : std::uint8_t
    {
        TopBun,
        BottomBun,
        Cheese,
        Patty,
        Tomato,
        Lettuce,
    };

    explicit BurgerPart(gla::GameObject* pOwner, Stage* pStage, Type pieceType, std::shared_ptr<gla::Texture2D> const& spriteSheetTexture);

    static constexpr float pieceStepOffset{ 2.f };
    static constexpr float pieceSize{ 8.f };
    static constexpr float fallingSpeed{ 50.f };
    static constexpr int pieceCount{ 4 };
protected:
    void FixedUpdate(float fixedDeltaTime) override;

private:

    static auto GetBurgerPieceSourceRect(Type type, long index) -> SDL_FRect;
    void OnPieceStep(long index);

    burgerpartstates::BurgerStateMachine m_stateMachine;
    std::array<std::pair<gla::CollisionRect*, gla::Sprite*>, pieceCount> m_pieces{};
    int m_steppedPieces{};
    Stage* m_pStage;
};

}  // namespace bt

#endif  // BURGERTIME_BURGERPART_HPP
