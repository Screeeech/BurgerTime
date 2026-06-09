#include "GameScenes.hpp"

#include "Commands/CallbackCommand.hpp"
#include "Components/Entity.hpp"
#include "Components/HealthDisplay.hpp"
#include "Components/HighScore.hpp"
#include "Components/PepperDisplay.hpp"
#include "Components/Score.hpp"
#include "Components/Sprite.hpp"
#include "Components/Stage.hpp"
#include "Components/StartMenu.hpp"
#include "Components/TextComponent.hpp"
#include "Locator.hpp"
#include "Scene.hpp"
#include "Services/InputManager.hpp"
#include "Services/GameState.hpp"
#include "Services/ResourceManager.hpp"
#include "Utils.hpp"


namespace bt
{

void LoadStartScene(gla::Scene const& scene)
{
    auto& resourceManager{ gla::Locator::Get<gla::ResourceManager>() };

    auto const font = resourceManager.LoadFont("Fonts/nes.ttf", 8);
    auto const logoTexture = resourceManager.LoadTexture("Textures/logo.png");
    auto const spritesheetTexture = resourceManager.LoadTexture("Textures/spritesheet.png");

    auto* startMenu = scene.GetRoot()->CreateChild(0, 0, "Start menu");
    startMenu->CreateChild((256.f - 152.f) / 2, 50, "Logo")->AddComponent<gla::Sprite>(logoTexture);

    auto* singlePlayerTextObject = startMenu->CreateChild(100, 140, "Singleplayer");
    singlePlayerTextObject->AddComponent<gla::TextComponent>("1 PLAYER", font);

    auto* coopTextObject = singlePlayerTextObject->CreateChild(0, 16, "Co-op");
    coopTextObject->AddComponent<gla::TextComponent>("CO-OP", font);

    auto* versusTextObject = coopTextObject->CreateChild(0, 16, "Versus");
    versusTextObject->AddComponent<gla::TextComponent>("VERSUS", font);

    auto* indicator = singlePlayerTextObject->CreateChild(-20, 0, "Indicator");
    auto* indicatorSprite = indicator->AddComponent<gla::Sprite>(spritesheetTexture);
    indicatorSprite->SetSourceRect({ .x = 48, .y = 128, .w = 16, .h = 16 });
    indicatorSprite->m_offset.y = -4;

    startMenu->AddComponent<StartMenu>(indicator);
}

void UnloadStartScene() {}
void LoadGameScene(gla::Scene const& scene)
{
    auto& resourceManager{ gla::Locator::Get<gla::ResourceManager>() };

    auto const font = resourceManager.LoadFont("Fonts/nes.ttf", 8);

    auto* score = scene.GetRoot()->CreateChild(100, 15, "Score");
    score->AddComponent<Score>(font, 0);

    auto* highScore = scene.GetRoot()->CreateChild(110, 15, "HighScore");
    highScore->AddComponent<HighScore>(font, "Jane Doe", 20'000);

    auto* pepperDisplay = scene.GetRoot()->CreateChild(208, 15, "PepperDisplay");
    pepperDisplay->AddComponent<PepperDisplay>(5);

    auto* healthDisplay = scene.GetRoot()->CreateChild(240, 15, "Health");
    healthDisplay->AddComponent<HealthDisplay>(5);
}

void LoadSinglePlayerGameScene(gla::Scene const& scene)
{
    LoadGameScene(scene);
    auto* stageObject = scene.GetRoot()->CreateChild(32, 32, "Stage");
    auto* stage = stageObject->AddComponent<Stage>("Stages/stage1.json");

    // Peter Pepper
    auto const& gameState{ gla::Locator::Get<GameState>() };
    Entity::CreatePlayer(*stage, *gameState.peterPepperPlayerIndex, { 95, -2 });
}
void UnloadGameScene() {}

void LoadCoopGameScene(gla::Scene const& scene)
{
    LoadGameScene(scene);
    auto* stageObject = scene.GetRoot()->CreateChild(32, 32, "Stage");
    auto* stage = stageObject->AddComponent<Stage>("Stages/stage1.json");
    auto const& gameState{ gla::Locator::Get<GameState>() };

    // Peter Pepper
    Entity::CreatePlayer(*stage, *gameState.peterPepperPlayerIndex, { 95, -2 });

    // Sally Salt
    Entity::CreatePlayer(*stage, *gameState.sallySaltPlayerIndex, { 127, -2 });
}
void UnloadCoopGameScene() {}

void LoadVersusGameScene(gla::Scene const& scene)
{
    LoadGameScene(scene);
    auto* stageObject = scene.GetRoot()->CreateChild(32, 32, "Stage");
    auto* stage = stageObject->AddComponent<Stage>("Stages/stage1.json");
    auto const& gameState{ gla::Locator::Get<GameState>() };

    // Peter Pepper
    Entity::CreatePlayer(*stage, *gameState.peterPepperPlayerIndex, { 95, -2 });

    // Mr HotDog
    Entity::CreateEnemy(*stage, *gameState.enemyPlayerIndex, { 127, -2 }, Entity::Type::HotDog);
}

void UnloadVersusGameScene() {}

}  // namespace bt
