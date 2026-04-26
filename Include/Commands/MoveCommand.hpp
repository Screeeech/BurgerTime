#ifndef ENGINE_MOVECOMMAND_H
#define ENGINE_MOVECOMMAND_H
#include "Commands/GameObjectCommand.hpp"

namespace dae
{

class MoveCommand : public GameObjectCommand
{
public:
    explicit MoveCommand(GameObject* pCaller, const glm::vec3& velocity);
    explicit MoveCommand(GameObject* pCaller, float x, float y, float z = 0);
    ~MoveCommand() noexcept override = default;
    void Execute() override;

private:
    glm::vec3 m_direction;
};

}  // namespace dae

#endif  // ENGINE_MOVECOMMAND_H
