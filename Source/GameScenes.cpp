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

void UnloadStartScene()
{
    // auto& inputManager{ gla::Locator::Get<gla::InputManager>() };
    // inputManager.UnbindAction("select"_h, 0);
}

void LoadGameScene(gla::Scene const& scene)
{
    auto& resourceManager{ gla::Locator::Get<gla::ResourceManager>() };
    auto& inputManager{ gla::Locator::Get<gla::InputManager>() };

    auto const font = resourceManager.LoadFont("Fonts/nes.ttf", 8);

    auto* stageObject = scene.GetRoot()->CreateChild(32, 32, "Stage");
    auto* stage = stageObject->AddComponent<bt::Stage>("Stages/stage1.json");

    auto* score = scene.GetRoot()->CreateChild(100, 15, "Score");
    score->AddComponent<bt::Score>(font, 0);

    auto* highScore = scene.GetRoot()->CreateChild(110, 15, "HighScore");
    highScore->AddComponent<bt::HighScore>(font, "Jane Doe", 20'000);

    auto* pepperDisplay = scene.GetRoot()->CreateChild(208, 15, "PepperDisplay");
    pepperDisplay->AddComponent<bt::PepperDisplay>(5);

    auto* healthDisplay = scene.GetRoot()->CreateChild(240, 15, "Health");
    healthDisplay->AddComponent<bt::HealthDisplay>(5);

    // Player 0
    {
        // GameObject
        // auto* pepperObject{ stageObject->CreateChild(0, 0, "Pepper") };
        // auto* pepperComponent{ pepperObject->AddComponent<bt::Pepper>(3) };

        int constexpr playerIndex{ 0 };
        bt::Entity::CreatePlayer(*stage, playerIndex, { 95, -2 });

        inputManager.RegisterInput(SDL_SCANCODE_W, gla::Input::Type::held, "moveUp"_h, playerIndex);
        inputManager.RegisterInput(SDL_SCANCODE_A, gla::Input::Type::held, "moveLeft"_h, playerIndex);
        inputManager.RegisterInput(SDL_SCANCODE_S, gla::Input::Type::held, "moveDown"_h, playerIndex);
        inputManager.RegisterInput(SDL_SCANCODE_D, gla::Input::Type::held, "moveRight"_h, playerIndex);
        inputManager.RegisterInput(SDL_SCANCODE_E, gla::Input::Type::held, "attack"_h, playerIndex);
    }

    // Mr Egg
    {
        int constexpr enemyIndex{ 1 };

        bt::Entity::CreateEnemy(*stage, enemyIndex, { 50, -2 }, bt::Entity::Type::Egg);

        inputManager.RegisterInput(SDL_SCANCODE_I, gla::Input::Type::held, "moveUp"_h, enemyIndex);
        inputManager.RegisterInput(SDL_SCANCODE_J, gla::Input::Type::held, "moveLeft"_h, enemyIndex);
        inputManager.RegisterInput(SDL_SCANCODE_K, gla::Input::Type::held, "moveDown"_h, enemyIndex);
        inputManager.RegisterInput(SDL_SCANCODE_L, gla::Input::Type::held, "moveRight"_h, enemyIndex);
    }

    // Mr Hotdog
    {
        int constexpr enemyIndex{ 2 };

        bt::Entity::CreateEnemy(*stage, enemyIndex, { 30, -2 }, bt::Entity::Type::HotDog);

        inputManager.RegisterInput(SDL_SCANCODE_UP, gla::Input::Type::held, "moveUp"_h, enemyIndex);
        inputManager.RegisterInput(SDL_SCANCODE_LEFT, gla::Input::Type::held, "moveLeft"_h, enemyIndex);
        inputManager.RegisterInput(SDL_SCANCODE_DOWN, gla::Input::Type::held, "moveDown"_h, enemyIndex);
        inputManager.RegisterInput(SDL_SCANCODE_RIGHT, gla::Input::Type::held, "moveRight"_h, enemyIndex);
    }
}

void UnloadGameScene() {}

}  // namespace bt
