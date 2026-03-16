#include "EventManager.h"

namespace dae
{

void EventManager::InvokeEvent(EventID id, const Event& event)
{
    auto range = m_listeners.equal_range(id);
    for(auto&& [key, value]  : std::ranges::subrange(range.first, range.second))
    {
        value.second(event);
    }
}

}  // namespace dae