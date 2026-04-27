#include "Commands/MoveCommand.hpp"

#include "Components/PlayerController.hpp"
#include "GameObject.hpp"

namespace bt
{
MoveCommand::MoveCommand(gla::GameObject* pCaller, const glm::vec3& velocity)
    : GameObjectCommand(pCaller)
    , m_direction(velocity)
{
}

MoveCommand::MoveCommand(gla::GameObject* pCaller, float x, float y, float z)
    : GameObjectCommand(pCaller)
    , m_direction(x, y, z)
{
}

void MoveCommand::Execute()
{
    auto* playerController = m_pCaller->GetComponent<PlayerController>();
    playerController->SetDirection(m_direction);
}

}  // namespace bt