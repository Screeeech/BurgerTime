#ifndef ENGINE_COMMAND_H
#define ENGINE_COMMAND_H

namespace dae
{
class GameObject;

class Command
{
public:
    Command() = default;
    virtual ~Command() noexcept = default;
    virtual void Execute() = 0;
};

}  // namespace dae

#endif  // ENGINE_COMMAND_H
