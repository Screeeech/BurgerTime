#ifndef ENGINE_EVENTMANAGER_H
#define ENGINE_EVENTMANAGER_H
#include <functional>
#include <unordered_map>

#include "Singleton.h"

namespace dae
{
struct Event;
class EventListener;
class Observer;

using EventID = unsigned int;
using EventCallback = std::function<void(const Event& event)>;

class EventManager final : public Singleton<EventManager>
{
public:
    template <typename T>
    void AddListener(EventID id, T* listener, EventCallback callback)
    {
        m_listeners.emplace( id, std::pair{ listener, std::move(callback) } );
    }

    template <typename T>
    void RemoveListeners(EventID id, T* listener)
    {
        std::erase_if(m_listeners, [&](const auto& pair)
        {
            return pair.first == id and pair.second.first == listener;
        });
    }

    void InvokeEvent(EventID id, const Event& event);

private:
    std::unordered_multimap<EventID, std::pair<void*, EventCallback>> m_listeners;
};

}  // namespace dae

#endif  // ENGINE_EVENTMANAGER_H
