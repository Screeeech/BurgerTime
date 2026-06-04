#ifndef BURGERTIME_BURGERPART_HPP
#define BURGERTIME_BURGERPART_HPP
#include <SDL3/SDL_rect.h>

#include <array>
#include <memory>
#include <vector>

#include "Component.hpp"
#include "States/BurgerPartStates.hpp"

namespace gla
{
class Timer;
class Collider;
class Sprite;
class Texture2D;
class CollisionRect;
class Animation;
}  // namespace gla
namespace bt
{

class Enemy;
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
    static constexpr float resetTime{ 0.5f };
    static constexpr float pieceStepOffset{ 2.f };
    static constexpr float pieceSize{ 8.f };
    static constexpr float fallingSpeed{ 60.f };
    static constexpr int pieceCount{ 4 };
    using Pieces = std::array<std::pair<gla::CollisionRect*, gla::Sprite*>, pieceCount>;


    explicit BurgerPart(gla::GameObject* pOwner, Stage* pStage, Type pieceType, std::shared_ptr<gla::Texture2D> const& spriteSheetTexture);

    void AcquireEnemy(gla::GameObject& enemyObject, Enemy& enemy);
    void ReleaseEnemies();

    auto GetSteppedPieces() const -> int;
    void SetSteppedPieces(int steppedPieces);

    auto GetPieces() -> Pieces&;
protected:
    void FixedUpdate(float fixedDeltaTime) override;

private:
    static auto GetBurgerPieceSourceRect(Type type, long index) -> SDL_FRect;
    void OnPieceStep(long index);

    gla::Timer* m_pResetTimer{};
    Pieces m_pieces{};
    std::vector<Enemy*> m_fallingEnemies;
    int m_steppedPieces{};
    Stage* m_pStage;
    burgerpartstates::BurgerStateMachine m_stateMachine;
};

}  // namespace bt

#endif  // BURGERTIME_BURGERPART_HPP
