//
// Created by lily-laptop on 09/03/2026.
//

#ifndef ENGINE_COMMAND_H
#define ENGINE_COMMAND_H

namespace dae
{
class GameObject;

class Command
{
public:
    Command() = default;
    virtual ~Command() = default;
    virtual void Execute() = 0;
};

}  // namespace dae

#endif  // ENGINE_COMMAND_H
