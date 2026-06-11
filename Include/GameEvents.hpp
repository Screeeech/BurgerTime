#ifndef BURGERTIME_EVENTS_HPP
#define BURGERTIME_EVENTS_HPP
#include "Events.hpp"
#include <glm/vec2.hpp>

namespace bt
{
class Pepper;

struct HealthEvent : gla::EntityEvent
{
    explicit HealthEvent(EventID id, int playerIndex, int healthChange)
        : EntityEvent(id, playerIndex)
        , healthChange(healthChange)
    {
    }

    int healthChange;
};

struct ScoreEvent : gla::Event
{
    explicit ScoreEvent(EventID id, int scoreChange)
        : Event(id)
        , score{ scoreChange }
    {
    }

    int score;
};

struct PlateFinishedEvent final : gla::Event
{
    explicit PlateFinishedEvent(EventID id, int plateIndex)
        : Event(id)
        , plateIndex(plateIndex)
    {
    }

    int plateIndex;
};

struct PepperEvent final : gla::EntityEvent
{
    explicit PepperEvent(EventID id, int playerIndex, glm::vec2 inputDirection, glm::vec2 position, Pepper* pPepper)
        : EntityEvent(id, playerIndex)
        , inputDirection(inputDirection)
        , position(position)
        , pPepper(pPepper)
    {
    }

    glm::vec2 inputDirection;
    glm::vec2 position;
    Pepper* pPepper;
};

}  // namespace bt

#endif  // BURGERTIME_EVENTS_HPP
