//
// Created by lily-laptop on 09/03/2026.
//

#include "MoveCommand.h"

#include "GameObject.h"

dae::MoveCommand::MoveCommand(float x, float y, float z)
    : m_move(x, y, z)
{
}

dae::MoveCommand::MoveCommand(glm::vec3 move)
    : m_move(move)
{
}

void dae::MoveCommand::execute(GameObject* pCaller)
{
    const auto pos{ pCaller->GetTransform().GetLocalPosition() };
    pCaller->GetTransform().SetLocalPosition(pos + m_move);
}