#ifndef BURGERTIME_EVENTS_HPP
#define BURGERTIME_EVENTS_HPP
#include "Events.hpp"

namespace bt
{
class Pepper;

struct PlateFinishedEvent final : gla::Event
{
    explicit PlateFinishedEvent(EventID id, int plateIndex)
        : Event(id)
        , plateIndex(plateIndex)
    {
    }

    int plateIndex;
};

struct PepperEvent final : gla::PlayerEvent
{
    ~PepperEvent() override = default;

    explicit PepperEvent(EventID id, int playerIndex, glm::vec2 inputDirection, glm::vec2 position, Pepper* pPepper)
        : PlayerEvent(id, playerIndex)
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
