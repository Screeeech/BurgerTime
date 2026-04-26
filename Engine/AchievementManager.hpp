#ifndef ENGINE_ACHIEVEMENTMANAGER_H
#define ENGINE_ACHIEVEMENTMANAGER_H

#include <cstdint>
#include <set>

#include "Singleton.hpp"

namespace dae
{
struct Event;

enum class AchievementType : uint8_t
{
    win
};

class AchievementManager : public Singleton<AchievementManager>
{

public:
    void Achieve(AchievementType achievement);
    void OnWin(const Event& event);

    std::set<AchievementType> m_Achievements{};
};

}  // namespace dae

#endif  // ENGINE_ACHIEVEMENTMANAGER_H
