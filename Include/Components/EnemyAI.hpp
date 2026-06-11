#ifndef BURGERTIME_ENEMYAI_HPP
#define BURGERTIME_ENEMYAI_HPP

#include <any>

#include "Commands/MoveCommand.hpp"
#include "Component.hpp"

namespace bt
{
class MoveComponent;

class EnemyAI final : public gla::Component
{
public:
    explicit EnemyAI(gla::GameObject* pOwner, int entityIndex, gla::GameObject* pPlayer1, gla::GameObject* pPlayer2);


protected:
    void LateUpdate() override;
    void OnActivate() override;
    void OnDeactivate() override;

private:
    void OnFindNewDirection(std::any const& entityEvent);
    void FindNewDirection();
    auto GetClosestPlayerDirection() const -> glm::vec2;

    MoveCommand m_moveUp;
    MoveCommand m_moveLeft;
    MoveCommand m_moveDown;
    MoveCommand m_moveRight;

    int m_entityIndex;
    MoveComponent* m_pMoveComponent;
    gla::GameObject* m_pPlayer1;
    gla::GameObject* m_pPlayer2;

    enum class Direction
    {
        Idle,
        Up,
        Left,
        Down,
        Right,
    } m_currentDirection{ Direction::Idle };
    static auto FlipDirections(Direction currentDirection) -> Direction;
};

}  // namespace bt

#endif  // BURGERTIME_ENEMYAI_HPP
