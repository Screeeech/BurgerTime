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
    auto& gameState = gla::Locator::Get<GameState>();
    gameState.SetGameMode(m_selectedMode);
    gameState.StartGame();

    auto& sceneManager = gla::Locator::Get<gla::SceneManager>();
    sceneManager.LoadScene("Game");
}

}  // namespace bt