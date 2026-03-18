#ifndef ENGINE_EVENTS_H
#define ENGINE_EVENTS_H

namespace dae
{
template<int length>
struct sdbm_hash
{
    consteval static unsigned int _calculate(const char* const text, unsigned int& value)
    {
        const unsigned int character = sdbm_hash<length - 1>::_calculate(text, value);
        value = character + (value << 6) + (value << 16) - value;
        return text[length - 1];
    }

    consteval static unsigned int calculate(const char* const text)
    {
        unsigned int value = 0;
        const auto character = _calculate(text, value);
        return character + (value << 6) + (value << 16) - value;
    }
};

template<>
struct sdbm_hash<1>
{
    consteval static int _calculate(const char* const text, unsigned int&)
    {
        return text[0];
    }
};

template<size_t N>
consteval unsigned int sdbm(const char (&text)[N])
{
    return sdbm_hash<N - 1>::calculate(text);
}

struct Event
{
    virtual ~Event() = default;

    explicit Event(EventID id)
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
        , health(healthChange)
    {
    }

    int health;
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
