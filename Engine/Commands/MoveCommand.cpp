#include "MoveCommand.h"

#include <print>

#include "Components/PlayerController.h"
#include "GameObject.h"

namespace dae
{
MoveCommand::MoveCommand(GameObject* pCaller, const glm::vec3& velocity)
    : GameObjectCommand(pCaller)
    , m_direction(velocity)
{
}

MoveCommand::MoveCommand(GameObject* pCaller, float x, float y, float z)
    : GameObjectCommand(pCaller)
    , m_direction(x, y, z)
{
}

void MoveCommand::Execute()
{
    auto* playerController = m_pCaller->GetComponent<PlayerController>();
    playerController->SetDirection(m_direction);
}

}  // namespace dae