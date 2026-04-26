#include "GameObjectCommand.h"

namespace dae
{

GameObjectCommand::GameObjectCommand(GameObject* pCaller)
    : m_pCaller(pCaller)
{
}

void GameObjectCommand::Execute() {}

}  // namespace dae