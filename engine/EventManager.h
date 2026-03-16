#ifndef ENGINE_EVENTMANAGER_H
#define ENGINE_EVENTMANAGER_H
#include "Singleton.h"

namespace dae
{

class EventManager final
{
public:
    explicit EventManager();
    ~EventManager() override = default;

private:
};

}  // namespace dae

#endif  // ENGINE_EVENTMANAGER_H
