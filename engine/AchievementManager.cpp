#include "AchievementManager.h"

#include <string>

#if USE_STEAMWORKS
#include <steam_api.h>
#endif

namespace dae
{


void AchievementManager::Achieve(AchievementType achievement)
{
    // I am aware this is not the best implementation, but for now this is what I have
    if(m_Achievements.contains(achievement))
        return;

#if USE_STEAMWORKS
    std::string name{};
    switch (achievement)
    {
        case AchievementType::win:
            name = "ACH_WIN_ONE_GAME";
            break;
    }

    SteamUserStats()->SetAchievement(name.c_str());
    SteamUserStats()->StoreStats();
#endif
}

void AchievementManager::OnWin(const Event&)
{
    Achieve(AchievementType::win);
}

}  // namespace dae