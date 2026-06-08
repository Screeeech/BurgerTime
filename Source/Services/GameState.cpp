#include "Services/GameState.hpp"

namespace bt
{

void GameState::StartGame()
{
    m_gameStarted = true;
}

void GameState::SetGameMode(GameMode mode)
{
    if (not m_gameStarted)
        m_gameMode = mode;
}

auto GameState::GetGameMode() const -> GameMode
{
    return m_gameMode;
}

}  // namespace bt