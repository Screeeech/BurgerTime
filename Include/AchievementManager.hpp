#ifndef BURGERTIME_ACHIEVEMENTMANAGER_H
#define BURGERTIME_ACHIEVEMENTMANAGER_H

#include <cstdint>
#include <set>

#include "Singleton.hpp"

namespace gla
{
struct Event;
}

namespace bt
{

enum class AchievementType : uint8_t
{
    win
};

class AchievementManager : public gla::Singleton<AchievementManager>
{

public:
    void Achieve(AchievementType achievement);
    void OnWin(const gla::Event& event);

    std::set<AchievementType> m_Achievements{};
};

}  // namespace bt

#endif  // BURGERTIME_ACHIEVEMENTMANAGER_H
