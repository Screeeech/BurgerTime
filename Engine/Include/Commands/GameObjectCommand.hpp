#ifndef ENGINE_GAMEOBJECTCOMMAND_H
#define ENGINE_GAMEOBJECTCOMMAND_H
#include <glm/vec3.hpp>

#include "Command.hpp"

namespace dae
{

class GameObjectCommand : public Command
{
public:
    explicit GameObjectCommand(GameObject* pCaller);
    ~GameObjectCommand() noexcept override = default;

    void Execute() override;
protected:
    GameObject* m_pCaller;
};

}  // namespace dae

#endif  // ENGINE_GAMEOBJECTCOMMAND_H
