#ifndef BURGERTIME_CUSTOMEVENTS_HPP
#define BURGERTIME_CUSTOMEVENTS_HPP
#include "Events.hpp"

namespace bt
{

struct PlateFinishedEvent final : gla::Event
{
    explicit PlateFinishedEvent(EventID id, int plateIndex)
        : Event(id)
        , plateIndex(plateIndex)
    {
    }

    int plateIndex;
};

}  // namespace bt

#endif  // BURGERTIME_CUSTOMEVENTS_HPP
