#include "GameScenes.hpp"

#include "Commands/CallbackCommand.hpp"
#include "Components/EnemyAI.hpp"
#include "Components/EnemySpawner.hpp"
#include "Components/Entity.hpp"
#include "Components/HealthDisplay.hpp"
#include "Components/HighScore.hpp"
#include "Components/InitialsInput.hpp"
#include "Components/LeaderBoard.hpp"
#include "Components/PepperDisplay.hpp"
#include "Components/Score.hpp"
#include "Components/Sprite.hpp"
#include "Components/Stage.hpp"
#include "Components/StartMenu.hpp"
#include "Components/TextComponent.hpp"
#include "Components/Timer.hpp"
#include "Constants.hpp"
#include "Locator.hpp"
#include "Scene.hpp"
#include "Services/GameState.hpp"
#include "Services/InputManager.hpp"
#include "Services/ResourceManager.hpp"
#include "Utils.hpp"


namespace bt
{

void LoadStartScene(gla::Scene const& scene)
{
    auto& resourceManager{ gla::Locator::Get<gla::ResourceManager>() };

    auto const* persistentRoot = gla::Locator::Get<gla::SceneManager>().GetPersistentScene().GetRoot();
    auto* gameState = persistentRoot->GetComponent<GameState>();

    auto const font = resourceManager.LoadFont("Fonts/nes.ttf", 8);
    auto const logoTexture = resourceManager.LoadTexture("Textures/logo.png");
    auto const spritesheetTexture = resourceManager.LoadTexture("Textures/spritesheet.png");

    auto* startMenu = scene.GetRoot()->CreateChild(0, 0, "Start menu");
    startMenu->CreateChild((256.f - 152.f) / 2, 20, "Logo")->AddComponent<gla::Sprite>(logoTexture);

    auto* leaderBoardObject = startMenu->CreateChild(70.f, 80.f, "Leaderboard");
    auto const highScore = leaderBoardObject->AddComponent<LeaderBoard>(font, 0)->GetHighScore();
    gameState->highScore = highScore;

    auto* singlePlayerTextObject = startMenu->CreateChild(100, 170, "Singleplayer");
    singlePlayerTextObject->AddComponent<gla::TextComponent>("1 PLAYER", font);

    auto* coopTextObject = singlePlayerTextObject->CreateChild(0, 16, "Co-op");
    coopTextObject->AddComponent<gla::TextComponent>("CO-OP", font);

    auto* versusTextObject = coopTextObject->CreateChild(0, 16, "Versus");
    versusTextObject->AddComponent<gla::TextComponent>("VERSUS", font);

    auto* indicator = singlePlayerTextObject->CreateChild(-20, 0, "Indicator");
    auto* indicatorSprite = indicator->AddComponent<gla::Sprite>(spritesheetTexture);
    indicatorSprite->SetSourceRect({ .x = 3 * 16, .y = 10 * 16, .w = 16, .h = 16 });
    indicatorSprite->m_offset.y = -4;

    startMenu->AddComponent<StartMenu>(
        indicator,
        gla::Locator::Get<gla::SceneManager>().GetPersistentScene().GetRoot()->GetComponent<GameState>());
}

void LoadLoadingScene(gla::Scene const& scene)
{
    auto const* gameState = gla::Locator::Get<gla::SceneManager>().GetPersistentScene().GetRoot()->GetComponent<GameState>();
    auto& resourceManager = gla::Locator::Get<gla::ResourceManager>();
    auto const font = resourceManager.LoadFont("Fonts/nes.ttf", 8);

    auto* loading = scene.GetRoot()->CreateChild(128, 110, "Loading text");

    std::string const loadingText = [&] -> auto
    {
        switch (gameState->GetGameMode())
        {
            case GameMode::Singleplayer:
                return "SINGLEPLAYER";
            case GameMode::Coop:
                return "CO-OP";
            case GameMode::Versus:
                return "VERSUS";
        }
    }();

    loading->AddComponent<gla::TextComponent>(loadingText, font, layers::text, gla::TextComponent::Align::Center, colors::LoadingTextColor);

    loading->CreateChild(0, 10, "Ready text")
        ->AddComponent<gla::TextComponent>("READY", font, layers::text, gla::TextComponent::Align::Center, colors::LoadingTextColor);
}

void LoadGameScene(gla::Scene const& scene, GameState const* gameState)
{
    auto& resourceManager{ gla::Locator::Get<gla::ResourceManager>() };

    auto const font = resourceManager.LoadFont("Fonts/nes.ttf", 8);

    auto* score = scene.GetRoot()->CreateChild(100, 15, "Score");
    score->AddComponent<Score>(font, gameState->GetScore());

    auto* highScore = scene.GetRoot()->CreateChild(110, 15, "HighScore");
    highScore->AddComponent<HighScore>(font, gameState->highScore);

    auto* pepperDisplay = scene.GetRoot()->CreateChild(208, 15, "PepperDisplay");
    pepperDisplay->AddComponent<PepperDisplay>(gameState->GetPepper());

    auto* healthDisplay = scene.GetRoot()->CreateChild(240, 15, "Health");
    healthDisplay->AddComponent<HealthDisplay>(gameState->GetHealth());
}

void LoadSinglePlayerGameScene(gla::Scene const& scene)
{
    auto const* persistentRoot = gla::Locator::Get<gla::SceneManager>().GetPersistentScene().GetRoot();
    auto const* gameState = persistentRoot->GetComponent<GameState>();

    auto [firstPos, secondPos] = gameState->GetSpawnPositions();

    LoadGameScene(scene, gameState);

    auto* entitiesContainer = scene.GetRoot()->CreateChild(32, 32, "Entities");

    // Peter Pepper
    auto* playerObject = entitiesContainer->CreateChild(firstPos, std::format("Enemy {}", *gameState->peterPepperPlayerIndex));
    Entity::CreatePlayer(playerObject, *gameState->peterPepperPlayerIndex, Entity::Type::Pepper);

    entitiesContainer->AddComponent<EnemySpawner>(playerObject, gameState->GetEnemyCounts());
}

void LoadCoopGameScene(gla::Scene const& scene)
{
    auto const* persistentRoot = gla::Locator::Get<gla::SceneManager>().GetPersistentScene().GetRoot();
    auto const* gameState = persistentRoot->GetComponent<GameState>();

    LoadGameScene(scene, gameState);

    auto* entitiesContainer = scene.GetRoot()->CreateChild(32, 32, "Entities");
    auto [firstPos, secondPos] = gameState->GetSpawnPositions();

    // Peter Pepper
    auto* player1Object = entitiesContainer->CreateChild(firstPos, std::format("Peter Pepper: {}", *gameState->peterPepperPlayerIndex));
    Entity::CreatePlayer(player1Object, *(gameState->peterPepperPlayerIndex), Entity::Type::Pepper);

    // Sally Salt
    auto* player2Object = entitiesContainer->CreateChild(secondPos, std::format("Sally Salt: {}", *gameState->sallySaltPlayerIndex));
    Entity::CreatePlayer(player2Object, *(gameState->sallySaltPlayerIndex), Entity::Type::Salt);

    entitiesContainer->AddComponent<EnemySpawner>(player1Object, player2Object, gameState->GetEnemyCounts());
}

void LoadVersusGameScene(gla::Scene const& scene)
{
    auto const* persistentRoot = gla::Locator::Get<gla::SceneManager>().GetPersistentScene().GetRoot();
    auto const* gameState = persistentRoot->GetComponent<GameState>();

    LoadGameScene(scene, gameState);

    auto* entitiesContainer = scene.GetRoot()->CreateChild(32, 32, "Entities");
    auto [firstPos, secondPos] = gameState->GetSpawnPositions();

    // Peter Pepper
    auto* player1Object = entitiesContainer->CreateChild(firstPos, std::format("Peter Pepper: {}", *gameState->peterPepperPlayerIndex));
    Entity::CreatePlayer(player1Object, *(gameState->peterPepperPlayerIndex), Entity::Type::Pepper);

    // Mr HotDog
    auto* enemyObject = entitiesContainer->CreateChild(secondPos, std::format("Mr HotDog: {}", *gameState->enemyPlayerIndex));
    Entity::CreateEnemy(enemyObject, *gameState->enemyPlayerIndex, Entity::Type::HotDog);

    entitiesContainer->AddComponent<EnemySpawner>(player1Object, *gameState->enemyPlayerIndex, gameState->GetEnemyCounts());
}

void LoadGameOverScene(gla::Scene const& scene)
{
    auto const* persistentRoot = gla::Locator::Get<gla::SceneManager>().GetPersistentScene().GetRoot();
    auto const* gameState = persistentRoot->GetComponent<GameState>();
    auto const font = gla::Locator::Get<gla::ResourceManager>().LoadFont("Fonts/nes.ttf", 8);

    auto* gameOver = scene.GetRoot()->CreateChild(128, 40, "Game Over text");
    gameOver->AddComponent<gla::TextComponent>("GAME OVER", font, layers::text, gla::TextComponent::Align::Center, colors::Red);

    auto* leaderBoardObject = scene.GetRoot()->CreateChild(88, 65, "Leaderboard object");
    auto const newHighScore = gameState->GetScore() >= gameState->highScore ? gameState->highScore : 0;
    auto* leaderBoard = leaderBoardObject->AddComponent<LeaderBoard>(font, newHighScore);

    auto* newScore = scene.GetRoot()->CreateChild(128, 160, "New score");
    newScore->AddComponent<gla::TextComponent>(
        std::format("YOUR SCORE: {}", gameState->GetScore()),
        font,
        layers::text,
        gla::TextComponent::Align::Center,
        colors::Green);

    auto* initialSelect = scene.GetRoot()->CreateChild(70, 190, "Initials select");

    auto* firstInitial = initialSelect->CreateChild(50, 24, "First initial");
    auto* lastInitial = firstInitial->CreateChild(8, 0, "Last initial");
    auto* firstText = firstInitial->AddComponent<gla::TextComponent>("A", font, layers::text);
    auto* lastText = lastInitial->AddComponent<gla::TextComponent>("B", font, layers::text);
    auto* indicatorObject = firstInitial->CreateChild(0, -8, "Indicator");
    indicatorObject->AddComponent<gla::TextComponent>("v", font, layers::text);

    initialSelect->AddComponent<InitialsInput>(firstText, lastText, indicatorObject, leaderBoard);
}

}  // namespace bt
