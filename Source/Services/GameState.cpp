#include "Services/GameState.hpp"

#include <format>
#include <fstream>
#include <nlohmann/json.hpp>
#include <print>

#include "Commands/CallbackCommand.hpp"
#include "Components/EnemySpawner.hpp"
#include "Components/Stage.hpp"
#include "Components/Timer.hpp"
#include "Events.hpp"
#include "Locator.hpp"
#include "Services/EventManager.hpp"
#include "Services/InputManager.hpp"
#include "Services/SceneManager.hpp"
#include "Utils.hpp"

namespace vw = std::ranges::views;

template<>
struct nlohmann::adl_serializer<bt::Initials>
{
    static void to_json(json& j, bt::Initials const& init) { j = std::string{ init.first, init.last }; }

    static void from_json(json const& j, bt::Initials& init)
    {
        auto const s = j.get<std::string>();
        if (s.size() != 2)
        {
            throw std::runtime_error("Initials must be exactly 2 characters");
        }
        init.first = s.at(0);
        init.last = s.at(1);
    }
};  // namespace nlohmann

using namespace nlohmann;

namespace bt
{

GameState::GameState(gla::GameObject* pOwner)
    : Component(pOwner)
    , m_pStageObject(gla::Locator::Get<gla::SceneManager>().GetPersistentScene().GetRoot()->CreateChild(32, 32, "Persistent Stage Object"))
    , m_pStartTimer(pOwner->AddComponent<gla::Timer>())
    , m_pEndTimer(pOwner->AddComponent<gla::Timer>())
{
}

void GameState::StartGame()
{
    m_gameStarted = true;
    CreateStage();
    m_pStartTimer->Start(game::loadingTime, [this] -> void { BeginRound(); });
}

void GameState::BeginRound() const
{
    if (not m_gameStarted)
        return;

    auto& sceneManager = gla::Locator::Get<gla::SceneManager>();
    switch (m_gameMode)
    {
        case GameMode::Singleplayer:
            sceneManager.ResetScene("Singleplayer");
            sceneManager.LoadScene("Singleplayer");
            break;
        case GameMode::Coop:
            sceneManager.ResetScene("Coop");
            sceneManager.LoadScene("Coop");
            break;
        case GameMode::Versus:
            sceneManager.ResetScene("Versus");
            sceneManager.LoadScene("Versus");
            break;
    }

    m_pStageObject->Activate();
}

void GameState::EndGame()
{
    m_gameStarted = false;
    SaveHighScoreData();

    health = game::startingPepper;
    pepper = game::startingPepper;
    score = 0;
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

auto GameState::GetSpawnPositions() const -> std::pair<glm::vec2, glm::vec2>
{
    auto const [player1, player2, enemy] = m_pStageObject->GetComponent<Stage>()->GetSpawnPositions();
    switch (m_gameMode)
    {
        case GameMode::Singleplayer:
            return { player1, {} };
        case GameMode::Coop:
            return { player1, player2 };
        case GameMode::Versus:
            return { player1, enemy };
        default:
            return {};
    }
}

auto GameState::GetEnemyCounts() const -> std::unordered_map<Entity::Type, int>
{
    auto enemyCounts = m_pStageObject->GetComponent<Stage>()->GetEnemyCounts();
    if (m_gameMode == GameMode::Versus)
        enemyCounts[Entity::Type::HotDog]--;

    return enemyCounts;
}

void GameState::OnActivate()
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.BindEvent("OnPlayerConnect"_h, this, &GameState::OnPlayerConnect);
    eventManager.BindEvent("OnPlayerDisconnect"_h, this, &GameState::OnPlayerDisconnect);
    eventManager.BindEvent("PlayerDeath"_h, this, &GameState::OnDeath);
    eventManager.BindEvent("StageCompleted"_h, this, &GameState::OnStageComplete);

    auto& inputManager = gla::Locator::Get<gla::InputManager>();
    inputManager.RegisterInput(SDL_SCANCODE_F1, gla::Input::Type::released, "__skip_stage"_h, 0);
    inputManager.BindAction<gla::CallbackCommand>("__skip_stage"_h, 0, [this] -> void { NextStage(); });
}

void GameState::OnDeactivate()
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.UnbindEvents(this);

    auto& inputManager = gla::Locator::Get<gla::InputManager>();
    inputManager.UnregisterInput(SDL_SCANCODE_F1, "__skip_stage"_h, 0);
    inputManager.UnbindAction("__skip_stage"_h, 0);
}

void GameState::CreateStage() const
{
    m_pStageObject->Deactivate();
    m_pStageObject->RemoveComponent<Stage>();
    m_pStageObject->RemoveComponent<gla::Timer>();

    // Get rid of all of the buger pieces
    for (auto* child : m_pStageObject->GetChildren())
        child->QueueDelete();

    m_pStageObject->AddComponent<Stage>(std::format("Stages/stage{}.json", m_stageIndex + 1));
}

void GameState::OnStageComplete(std::any const& /*eventArgs*/)
{
    gla::Locator::Get<gla::EventManager>().InvokeEvent(gla::Event{ "DisableEntities"_h });
    m_pEndTimer->Start(game::stageEndDelay, [this] -> void { NextStage(); });
}

void GameState::OnDeath(std::any const& /*eventArgs*/)
{
    gla::Locator::Get<gla::EventManager>().InvokeEvent(gla::Event{ "DisableEntities"_h });
    m_pEndTimer->Start(game::stageEndDelay, [this] -> void { Respawn(); });
}

void GameState::Respawn()
{
    auto& sceneManager = gla::Locator::Get<gla::SceneManager>();

    health--;
    if (health <= 0)
    {
        sceneManager.ResetScene("GameOver");
        sceneManager.LoadScene("GameOver");
        EndGame();
        return;
    }

    m_pStageObject->Deactivate();

    std::println("Respawning...");
    m_pStartTimer->Start(game::loadingTime, [this] -> void { BeginRound(); });

    sceneManager.ResetScene("Loading");
    sceneManager.LoadScene("Loading");
}

void GameState::NextStage()
{
    if (not m_gameStarted)
        return;

    m_stageIndex = ++m_stageIndex % game::stageCount;

    CreateStage();

    m_pStartTimer->Start(game::loadingTime, [this] -> void { BeginRound(); });

    auto& sceneManager = gla::Locator::Get<gla::SceneManager>();
    sceneManager.ResetScene("Loading");
    sceneManager.LoadScene("Loading");
}

void GameState::OnPlayerConnect(std::any const& connectEvent)
{
    auto const args = std::any_cast<gla::PlayerConnectionEvent>(connectEvent);
    auto& inputManager = gla::Locator::Get<gla::InputManager>();

    if (args.isGamepad)
    {
        inputManager.RegisterInput(SDL_GAMEPAD_BUTTON_EAST, gla::Input::Type::released, "select"_h, args.entityIndex);
        inputManager.RegisterInput(SDL_GAMEPAD_BUTTON_SOUTH, gla::Input::Type::released, "start"_h, args.entityIndex);
        inputManager.RegisterInput(SDL_GAMEPAD_BUTTON_WEST, gla::Input::Type::held, "attack"_h, args.entityIndex);
        inputManager.RegisterInput(SDL_GAMEPAD_BUTTON_DPAD_UP, gla::Input::Type::held, "moveUp"_h, args.entityIndex);
        inputManager.RegisterInput(SDL_GAMEPAD_BUTTON_DPAD_LEFT, gla::Input::Type::held, "moveLeft"_h, args.entityIndex);
        inputManager.RegisterInput(SDL_GAMEPAD_BUTTON_DPAD_DOWN, gla::Input::Type::held, "moveDown"_h, args.entityIndex);
        inputManager.RegisterInput(SDL_GAMEPAD_BUTTON_DPAD_RIGHT, gla::Input::Type::held, "moveRight"_h, args.entityIndex);
    }
    else
    {
        inputManager.RegisterInput(SDL_SCANCODE_U, gla::Input::Type::released, "select"_h, args.entityIndex);
        inputManager.RegisterInput(SDL_SCANCODE_I, gla::Input::Type::released, "start"_h, args.entityIndex);
        inputManager.RegisterInput(SDL_SCANCODE_J, gla::Input::Type::held, "attack"_h, args.entityIndex);
        inputManager.RegisterInput(SDL_SCANCODE_W, gla::Input::Type::held, "moveUp"_h, args.entityIndex);
        inputManager.RegisterInput(SDL_SCANCODE_A, gla::Input::Type::held, "moveLeft"_h, args.entityIndex);
        inputManager.RegisterInput(SDL_SCANCODE_S, gla::Input::Type::held, "moveDown"_h, args.entityIndex);
        inputManager.RegisterInput(SDL_SCANCODE_D, gla::Input::Type::held, "moveRight"_h, args.entityIndex);
    }
}

void GameState::OnPlayerDisconnect(std::any const& connectEvent)
{
    auto const args = std::any_cast<gla::PlayerConnectionEvent>(connectEvent);
    auto& inputManager = gla::Locator::Get<gla::InputManager>();

    if (args.isGamepad)
    {
        inputManager.UnregisterInput(SDL_GAMEPAD_BUTTON_EAST, "select"_h, args.entityIndex);
        inputManager.UnregisterInput(SDL_GAMEPAD_BUTTON_SOUTH, "start"_h, args.entityIndex);
        inputManager.UnregisterInput(SDL_GAMEPAD_BUTTON_WEST, "attack"_h, args.entityIndex);
        inputManager.UnregisterInput(SDL_GAMEPAD_BUTTON_DPAD_UP, "moveUp"_h, args.entityIndex);
        inputManager.UnregisterInput(SDL_GAMEPAD_BUTTON_DPAD_LEFT, "moveLeft"_h, args.entityIndex);
        inputManager.UnregisterInput(SDL_GAMEPAD_BUTTON_DPAD_DOWN, "moveDown"_h, args.entityIndex);
        inputManager.UnregisterInput(SDL_GAMEPAD_BUTTON_DPAD_RIGHT, "moveRight"_h, args.entityIndex);
    }
    else
    {
        inputManager.UnregisterInput(SDL_SCANCODE_U, "select"_h, args.entityIndex);
        inputManager.UnregisterInput(SDL_SCANCODE_I, "start"_h, args.entityIndex);
        inputManager.UnregisterInput(SDL_SCANCODE_J, "attack"_h, args.entityIndex);
        inputManager.UnregisterInput(SDL_SCANCODE_W, "moveUp"_h, args.entityIndex);
        inputManager.UnregisterInput(SDL_SCANCODE_A, "moveLeft"_h, args.entityIndex);
        inputManager.UnregisterInput(SDL_SCANCODE_S, "moveDown"_h, args.entityIndex);
        inputManager.UnregisterInput(SDL_SCANCODE_D, "moveRight"_h, args.entityIndex);
    }
}

void GameState::LoadHighScoreData()
{
    std::ifstream file(game::highScoreFile);

    if (not file.is_open())
        return;

    json const highScoreList = json::parse(file);

    if (not highScoreList.is_array())
        return;

    for (auto const& item : highScoreList)
    {
        try
        {
            auto const initial = item.at("initials").get<Initials>();
            int const score = item.at("score").get<int>();
            m_highScores[initial] = score;
        }
        catch (std::exception const&)
        {
            std::println("Warning!\t{} contains an invalid entry, skipping entry", game::highScoreFile);
        }
    }

    if (m_highScores.contains(currentInitials))
        highScore = m_highScores.at(currentInitials);
}

void GameState::SaveHighScoreData()
{
    json jsonArray = json::array();

    for (auto const& [initials, score] : m_highScores)
        jsonArray.push_back({ { "initials", initials }, { "score", score } });

    std::ofstream file(game::highScoreFile);
    file << jsonArray.dump(4);
}


}  // namespace bt