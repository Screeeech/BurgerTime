#include "EventManager.h"

#include "Events.h"

namespace dae
{

void EventManager::InvokeEvent(const Event& event)
{
    auto range = m_listeners.equal_range(event.eventID);
    for(auto&& [key, value]  : std::ranges::subrange(range.first, range.second))
    {
        value.second(event);
    }
}

}  // namespace dae