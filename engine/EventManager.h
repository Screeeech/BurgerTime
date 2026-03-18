#ifndef ENGINE_EVENTMANAGER_H
#define ENGINE_EVENTMANAGER_H
#include <functional>
#include <string>
#include <unordered_map>

#include "Singleton.h"

namespace dae
{
class PlayerController;
struct Event;
class EventListener;
class Observer;

using EventID = std::string;
using EventCallback = std::function<void(const Event&)>;

class EventManager final : public Singleton<EventManager>
{
public:
    template <typename T>
    void BindEvent(EventID id, T* listener, void(T::*callback)(const Event&))
    {
        m_listeners.emplace( id, std::pair{ listener, std::bind(callback, listener, std::placeholders::_1) } );
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

private:
    std::unordered_multimap<EventID, std::pair<void*, EventCallback>> m_listeners;
};

}  // namespace dae

#endif  // ENGINE_EVENTMANAGER_H
