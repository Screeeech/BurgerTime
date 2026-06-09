#include "StartMenu.hpp"

#include "Commands/CallbackCommand.hpp"
#include "GameObject.hpp"
#include "Locator.hpp"
#include "Services/GameState.hpp"
#include "Services/InputManager.hpp"
#include "Services/SceneManager.hpp"
#include "Utils.hpp"

namespace bt
{

StartMenu::StartMenu(gla::GameObject* pOwner, gla::GameObject* indicatorObject)
    : Component(pOwner)
    , m_pIndicatorObject(indicatorObject)
{
}

void StartMenu::OnActivate()
{
    auto& inputManager{ gla::Locator::Get<gla::InputManager>() };
    inputManager.BindAction<gla::CallbackCommand>("select"_h, 0, [this] -> void { OnSelect(); });
    inputManager.BindAction<gla::CallbackCommand>("start"_h, 0, [this] -> void { OnGameStart(); });
}

void StartMenu::OnDeactivate()
{
    auto& inputManager{ gla::Locator::Get<gla::InputManager>() };
    inputManager.UnbindAction("select"_h, 0);
    inputManager.UnbindAction("start"_h, 0);
}

void StartMenu::OnSelect()
{
    auto& transform = m_pIndicatorObject->GetTransform();
    auto pos = transform.GetLocalPosition();
    pos.y = static_cast<float>((static_cast<int>(pos.y) + 16) % 48);
    transform.SetLocalPosition(pos);
    switch (m_selectedMode)
    {
        case GameMode::Singleplayer:
            m_selectedMode = GameMode::Coop;
            break;
        case GameMode::Coop:
            m_selectedMode = GameMode::Versus;
            break;
        case GameMode::Versus:
            m_selectedMode = GameMode::Singleplayer;
            break;
    }
}

void StartMenu::OnGameStart() const
{
    auto& inputManager{ gla::Locator::Get<gla::InputManager>() };
    auto& gameState = gla::Locator::Get<GameState>();

    auto const activePlayerIndices = inputManager.GetActivePlayerIndices();
    switch (m_selectedMode)
    {
        case GameMode::Singleplayer:
        {
            if (not activePlayerIndices.empty())
            {
                gameState.peterPepperPlayerIndex = *activePlayerIndices.begin();
            }
            else
            {
                std::println("Must have at least 1 player assigned");
                return;
            }
        }
        break;
        case GameMode::Coop:
        {
            if (activePlayerIndices.size() >= 2)
            {
                gameState.peterPepperPlayerIndex = *activePlayerIndices.begin();
                gameState.sallySaltPlayerIndex = *(++activePlayerIndices.begin());
            }
            else
            {
                std::println("Must have at least 2 players assigned");
                return;
            }
        }
        break;
        case GameMode::Versus:
        {
            if (activePlayerIndices.size() >= 2)
            {
                gameState.peterPepperPlayerIndex = *activePlayerIndices.begin();
                gameState.enemyPlayerIndex = *(++activePlayerIndices.begin());
            }
            else
            {
                std::println("Must have at least 2 players assigned");
                return;
            }
        }
        break;
    }
    gameState.SetGameMode(m_selectedMode);
    gameState.StartGame();
    gla::Locator::Get<gla::SceneManager>().LoadScene("Loading");
}

}  // namespace bt