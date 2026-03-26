#ifndef ENGINE_EVENTS_H
#define ENGINE_EVENTS_H

namespace dae
{

struct Event
{
    virtual ~Event() = default;

    Event(EventID id)
        : eventID{ id }
    {
    }

    const EventID eventID{};
};

struct PlayerEvent : Event
{
    ~PlayerEvent() override = default;

    explicit PlayerEvent(EventID id, int playerIndex)
        : Event(id)
        , playerIndex{ playerIndex }
    {
    }

    int playerIndex;
};

struct HealthEvent : PlayerEvent
{
    ~HealthEvent() override = default;

    explicit HealthEvent(EventID id, int playerIndex, int healthChange)
        : PlayerEvent(id, playerIndex)
        , healthChange(healthChange)
    {
    }

    int healthChange;
};

struct ScoreEvent : PlayerEvent
{
    ~ScoreEvent() override = default;

    explicit ScoreEvent(EventID id, int playerIndex, int scoreChange)
        : PlayerEvent(id, playerIndex)
        , scoreChange{ scoreChange }
    {
    }

    int scoreChange;
};

}  // namespace dae

#endif  // ENGINE_EVENTS_H
