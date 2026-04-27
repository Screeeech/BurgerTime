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

    MoveCommand(MoveCommand const&) = delete;
    auto operator=(MoveCommand const&) -> MoveCommand& = delete;
    MoveCommand(MoveCommand&&) = delete;
    auto operator=(MoveCommand&&) -> MoveCommand& = delete;

    void Execute() override;

private:
    glm::vec3 m_direction;
};

}  // namespace bt

#endif  // BURGERTIME_MOVECOMMAND_H
