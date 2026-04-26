#ifndef ENGINE_EVENTMANAGER_H
#define ENGINE_EVENTMANAGER_H
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>

#include "Singleton.hpp"

namespace dae
{
class PlayerController;
struct Event;
class EventListener;
class Observer;

using EventID = unsigned int;
using EventCallback = std::function<void(const Event&)>;

class EventManager final : public Singleton<EventManager>
{
public:
    template <typename T>
    void BindEvent(EventID id, T* listener, void(T::*callback)(const Event&))
    {
        m_listeners.emplace( id, std::pair{ listener, std::bind_front(callback, listener) } );
    }

    template <typename T>
    void UnbindEvents(T* listener)
    {
        std::erase_if(m_listeners, [&](const auto& pair)
        {
            return pair.second.first == listener;
        });
    }

    template <typename T>
    void UnbindEvent(EventID id, T* listener)
    {
        std::erase_if(m_listeners, [&](const auto& pair)
        {
            return pair.first == id and pair.second.first == listener;
        });
    }

    void InvokeEvent(const Event& event);

    template <typename T>
        requires std::derived_from<T, Event>
    void QueueEvent(const T& event)
    {
        auto range = m_listeners.equal_range(event.eventID);
        for(auto&& [key, value]  : std::ranges::subrange(range.first, range.second))
        {
            m_queuedEvents.emplace(std::make_unique<T>(event), value.second);
        }
    }


    void ExecuteQueuedEvents();

private:
    // What if object gets deleted before event can fire?
    std::queue<std::pair<std::unique_ptr<Event>, EventCallback>> m_queuedEvents;
    std::unordered_multimap<EventID, std::pair<void*, EventCallback>> m_listeners;
};

}  // namespace dae

#endif  // ENGINE_EVENTMANAGER_H
