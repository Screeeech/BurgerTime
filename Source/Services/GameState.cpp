#include "Services/GameState.hpp"

#include "Events.hpp"
#include "Locator.hpp"
#include "Services/EventManager.hpp"
#include "Services/InputManager.hpp"
#include "Services/SceneManager.hpp"
#include "Utils.hpp"

namespace bt
{

GameState::GameState()
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.BindEvent("OnPlayerConnect"_h, this, &GameState::OnPlayerConnect);
    eventManager.BindEvent("OnPlayerDisconnect"_h, this, &GameState::OnPlayerDisconnect);
    eventManager.BindEvent("Respawn"_h, this, &GameState::OnRespawn);
}

void GameState::StartGame()
{
    m_gameStarted = true;

    auto& sceneManager = gla::Locator::Get<gla::SceneManager>();
    switch (m_gameMode)
    {
        case GameMode::Singleplayer:
            sceneManager.LoadScene("Singleplayer");
            break;
        case GameMode::Coop:
            sceneManager.LoadScene("Coop");
            break;
        case GameMode::Versus:
            sceneManager.LoadScene("Versus");
            break;
    }
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

auto GameState::GetHealth() const -> int
{
    return health;
}

void GameState::OnRespawn(std::any const& /*respawnEvent*/)
{
    health--;
    if (health <= 0)
    {
        gla::Locator::Get<gla::SceneManager>().LoadScene("GameOver");
        return;
    }
    gla::Locator::Get<gla::SceneManager>().LoadScene("Loading");
}

void GameState::OnPlayerConnect(std::any const& connectEvent)
{
    auto const args = std::any_cast<gla::PlayerConnectionEvent>(connectEvent);
    auto& inputManager = gla::Locator::Get<gla::InputManager>();

    if (args.isGamepad)
    {
        inputManager.RegisterInput(SDL_GAMEPAD_BUTTON_EAST, gla::Input::Type::released, "select"_h, args.playerIndex);
        inputManager.RegisterInput(SDL_GAMEPAD_BUTTON_SOUTH, gla::Input::Type::released, "start"_h, args.playerIndex);
        inputManager.RegisterInput(SDL_GAMEPAD_BUTTON_WEST, gla::Input::Type::held, "attack"_h, args.playerIndex);
        inputManager.RegisterInput(SDL_GAMEPAD_BUTTON_DPAD_UP, gla::Input::Type::held, "moveUp"_h, args.playerIndex);
        inputManager.RegisterInput(SDL_GAMEPAD_BUTTON_DPAD_LEFT, gla::Input::Type::held, "moveLeft"_h, args.playerIndex);
        inputManager.RegisterInput(SDL_GAMEPAD_BUTTON_DPAD_DOWN, gla::Input::Type::held, "moveDown"_h, args.playerIndex);
        inputManager.RegisterInput(SDL_GAMEPAD_BUTTON_DPAD_RIGHT, gla::Input::Type::held, "moveRight"_h, args.playerIndex);
    }
    else
    {
        inputManager.RegisterInput(SDL_SCANCODE_U, gla::Input::Type::released, "select"_h, args.playerIndex);
        inputManager.RegisterInput(SDL_SCANCODE_I, gla::Input::Type::released, "start"_h, args.playerIndex);
        inputManager.RegisterInput(SDL_SCANCODE_SPACE, gla::Input::Type::held, "attack"_h, args.playerIndex);
        inputManager.RegisterInput(SDL_SCANCODE_W, gla::Input::Type::held, "moveUp"_h, args.playerIndex);
        inputManager.RegisterInput(SDL_SCANCODE_A, gla::Input::Type::held, "moveLeft"_h, args.playerIndex);
        inputManager.RegisterInput(SDL_SCANCODE_S, gla::Input::Type::held, "moveDown"_h, args.playerIndex);
        inputManager.RegisterInput(SDL_SCANCODE_D, gla::Input::Type::held, "moveRight"_h, args.playerIndex);
    }
}

void GameState::OnPlayerDisconnect(std::any const& connectEvent)
{
    auto const args = std::any_cast<gla::PlayerConnectionEvent>(connectEvent);
    auto& inputManager = gla::Locator::Get<gla::InputManager>();

    if (args.isGamepad)
    {
        inputManager.UnregisterInput(SDL_GAMEPAD_BUTTON_EAST, "select"_h, args.playerIndex);
        inputManager.UnregisterInput(SDL_GAMEPAD_BUTTON_SOUTH, "start"_h, args.playerIndex);
        inputManager.UnregisterInput(SDL_GAMEPAD_BUTTON_WEST, "attack"_h, args.playerIndex);
        inputManager.UnregisterInput(SDL_GAMEPAD_BUTTON_DPAD_UP, "moveUp"_h, args.playerIndex);
        inputManager.UnregisterInput(SDL_GAMEPAD_BUTTON_DPAD_LEFT, "moveLeft"_h, args.playerIndex);
        inputManager.UnregisterInput(SDL_GAMEPAD_BUTTON_DPAD_DOWN, "moveDown"_h, args.playerIndex);
        inputManager.UnregisterInput(SDL_GAMEPAD_BUTTON_DPAD_RIGHT, "moveRight"_h, args.playerIndex);
    }
    else
    {
        inputManager.UnregisterInput(SDL_SCANCODE_U, "select"_h, args.playerIndex);
        inputManager.UnregisterInput(SDL_SCANCODE_I, "start"_h, args.playerIndex);
        inputManager.UnregisterInput(SDL_SCANCODE_SPACE, "attack"_h, args.playerIndex);
        inputManager.UnregisterInput(SDL_SCANCODE_W, "moveUp"_h, args.playerIndex);
        inputManager.UnregisterInput(SDL_SCANCODE_A, "moveLeft"_h, args.playerIndex);
        inputManager.UnregisterInput(SDL_SCANCODE_S, "moveDown"_h, args.playerIndex);
        inputManager.UnregisterInput(SDL_SCANCODE_D, "moveRight"_h, args.playerIndex);
    }
}


}  // namespace bt