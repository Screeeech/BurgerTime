//
// Created by lily-laptop on 09/03/2026.
//

#ifndef ENGINE_MOVECOMMAND_H
#define ENGINE_MOVECOMMAND_H
#include <glm/vec3.hpp>

#include "Command.h"

namespace dae
{
class GameObject;

class MoveCommand : public Command
{
public:
    explicit MoveCommand(float x, float y, float z = 0);
    explicit MoveCommand(glm::vec3 move);
    void execute(GameObject* pCaller) override;

private:
    glm::vec3 m_move;
};
}  // namespace dae

#endif  // ENGINE_MOVECOMMAND_H
