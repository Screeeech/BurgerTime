#ifndef BURGERTIME_MOVECOMMAND_H
#define BURGERTIME_MOVECOMMAND_H
#include "Commands/GameObjectCommand.hpp"

namespace bt
{

class MoveCommand : public gla::GameObjectCommand
{
public:
    explicit MoveCommand(gla::GameObject* pCaller, const glm::vec3& velocity);
    explicit MoveCommand(gla::GameObject* pCaller, float x, float y, float z = 0);
    ~MoveCommand() noexcept override = default;
    void Execute() override;

private:
    glm::vec3 m_direction;
};

}  // namespace bt

#endif  // BURGERTIME_MOVECOMMAND_H
