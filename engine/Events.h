#ifndef ENGINE_EVENTS_H
#define ENGINE_EVENTS_H

namespace dae
{

struct Event
{
    virtual ~Event() = default;
    explicit Event(EventID id)
        : eventID{ id }
    {}
    const EventID eventID{};
};

struct HealthEvent : Event
{
    explicit HealthEvent(EventID id, int _health)
        : Event(id)
        , health(_health)
    {}
    ~HealthEvent() override = default;
    int health;
};

class EventListener
{
public:
    virtual ~EventListener() = default;
    virtual void OnEvent(const Event& event) = 0;
};

}  // namespace dae

#endif  // ENGINE_EVENTS_H
