#ifndef ENGINE_CALLBACKCOMMAND_H
#define ENGINE_CALLBACKCOMMAND_H

#include <functional>

#include "Command.hpp"

namespace dae
{

class CallbackCommand : public Command
{
public:
    explicit CallbackCommand(std::function<void()> callback);
    ~CallbackCommand() noexcept override = default;
    void Execute() override;
private:
    std::function<void()> m_callback;
};

}  // namespace dae

#endif  // ENGINE_CALLBACKCOMMAND_H
