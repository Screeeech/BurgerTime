#include "Components/EnemyAI.hpp"

#include "Components/MoveComponent.hpp"
#include "Events.hpp"
#include "GameObject.hpp"
#include "Locator.hpp"
#include "Services/EventManager.hpp"
#include "Utils.hpp"

namespace bt
{

EnemyAI::EnemyAI(gla::GameObject* pOwner, int entityIndex, gla::GameObject* pPlayer1, gla::GameObject* pPlayer2)
    : Component(pOwner)
    , m_moveUp(pOwner, 0, -1)
    , m_moveLeft(pOwner, -1, 0)
    , m_moveDown(pOwner, 0, 1)
    , m_moveRight(pOwner, 1, 0)
    , m_entityIndex(entityIndex)
    , m_pMoveComponent(pOwner->GetComponent<MoveComponent>())
    , m_pPlayer1(pPlayer1)
    , m_pPlayer2(pPlayer2)
{
    assert(m_pPlayer1 and "Player1 can not be null");
    assert(m_pMoveComponent and "Enemy must have a MoveComponent");
}

void EnemyAI::LateUpdate()
{
    switch (m_currentDirection)
    {
        case Direction::Up:
            m_moveUp.Execute();
            break;
        case Direction::Left:
            m_moveLeft.Execute();
            break;
        case Direction::Down:
            m_moveDown.Execute();
            break;
        case Direction::Right:
            m_moveRight.Execute();
            break;
        default:
            break;
    }
}

void EnemyAI::OnActivate()
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.BindEvent("FindNewDirection"_h, this, &EnemyAI::OnFindNewDirection);

    FindNewDirection();
}

void EnemyAI::OnDeactivate()
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.UnbindEvents(this);
}

void EnemyAI::OnFindNewDirection(std::any const& entityEvent)
{
    if (std::any_cast<gla::EntityEvent const&>(entityEvent).entityIndex != m_entityIndex)
        return;

    FindNewDirection();
}

void EnemyAI::FindNewDirection()
{
    auto const target = GetClosestPlayerDirection();
    switch (m_currentDirection)
    {
        case Direction::Idle:
        {
            if (target.x > 0 and m_pMoveComponent->CanWalkRight())
                m_currentDirection = Direction::Right;
            else if (target.x < 0 and m_pMoveComponent->CanWalkLeft())
                m_currentDirection = Direction::Left;
            else if (target.y < 0 and m_pMoveComponent->CanClimbUp())
                m_currentDirection = Direction::Up;
            else if (target.y > 0 and m_pMoveComponent->CanClimbDown())
                m_currentDirection = Direction::Down;
        }
        break;
        case Direction::Up:
        case Direction::Down:
        {
            if (target.x > 0 and m_pMoveComponent->CanWalkRight())
                m_currentDirection = Direction::Right;
            else if (target.x < 0 and m_pMoveComponent->CanWalkLeft())
                m_currentDirection = Direction::Left;
            else if (not m_pMoveComponent->CanWalkLeft() and not m_pMoveComponent->CanWalkRight())
                m_currentDirection = FlipDirections(m_currentDirection);
        }
        break;
        case Direction::Left:
        case Direction::Right:
        {
            if (target.y < 0 and m_pMoveComponent->CanClimbUp())
                m_currentDirection = Direction::Up;
            else if (target.y > 0 and m_pMoveComponent->CanClimbDown())
                m_currentDirection = Direction::Down;
            else if (not m_pMoveComponent->CanClimbUp() and not m_pMoveComponent->CanClimbDown())
                m_currentDirection = FlipDirections(m_currentDirection);
        }
        break;
    }
}

auto EnemyAI::GetClosestPlayerDirection() const -> glm::vec2
{
    auto const myPos = m_pOwner->GetTransform().GetWorldPosition();

    auto const p1WorldPos = m_pPlayer1->GetTransform().GetWorldPosition();
    auto direction = p1WorldPos - myPos;

    if (m_pPlayer2)
    {
        auto const p2WorldPos = m_pPlayer2->GetTransform().GetWorldPosition();
        auto const p2Direction = p2WorldPos - myPos;
        direction = glm::dot(direction, direction) < glm::dot(p2Direction, p2Direction) ? p2Direction : direction;
    }

    direction.x = std::round(direction.x);
    direction.y = std::round(direction.y);

    return direction;
}

auto EnemyAI::FlipDirections(Direction currentDirection) -> Direction
{
    switch (currentDirection)
    {
        case Direction::Up:
            return Direction::Down;
        case Direction::Left:
            return Direction::Right;
        case Direction::Down:
            return Direction::Up;
        case Direction::Right:
            return Direction::Left;
        default:
            return Direction::Idle;
    }
}


}  // namespace bt