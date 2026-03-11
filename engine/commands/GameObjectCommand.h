//
// Created by lily-laptop on 11/03/2026.
//

#ifndef ENGINE_GAMEOBJECTCOMMAND_H
#define ENGINE_GAMEOBJECTCOMMAND_H
#include <glm/vec3.hpp>

#include "Command.h"

namespace dae
{

class GameObjectCommand : public Command
{
public:
    explicit GameObjectCommand(GameObject* pCaller);
    void Execute() override;
protected:
    GameObject* m_pCaller;
};

}  // namespace dae

#endif  // ENGINE_GAMEOBJECTCOMMAND_H
