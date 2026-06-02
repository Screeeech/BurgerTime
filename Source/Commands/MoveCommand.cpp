#include "Commands/MoveCommand.hpp"

#include "Components/MoveComponent.hpp"
#include "Components/Player.hpp"
#include "GameObject.hpp"

namespace bt
{
MoveCommand::MoveCommand(gla::GameObject* pCaller, glm::vec2 const& velocity)
    : GameObjectCommand(pCaller)
    , m_direction(velocity)
{
}

MoveCommand::MoveCommand(gla::GameObject* pCaller, float x, float y)
    : GameObjectCommand(pCaller)
    , m_direction(x, y)
{
}

void MoveCommand::Execute()
{
    if (auto* moveComponent = m_pCaller->GetComponent<MoveComponent>())
        moveComponent->SetDirection(m_direction);
}

}  // namespace bt