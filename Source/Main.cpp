
#include <filesystem>
#include <print>

#include "AchievementManager.hpp"
#include "Components/Animation.hpp"
#include "Components/FpsComponent.hpp"
#include "Components/PlayerController.hpp"
#include "Components/ScoreComponent.hpp"
#include "Components/Sprite.hpp"
#include "Components/TextComponent.hpp"
#include "EventManager.hpp"
#include "Events.hpp"
#include "InputManager.hpp"
#include "Minigin.hpp"
#include "ResourceManager.hpp"
#include "Scene.hpp"
#include "SceneManager.hpp"
#include "Utils.hpp"


#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

namespace fs = std::filesystem;

static void load()
{
    auto& scene = dae::SceneManager::Get().CreateScene();
    scene.Load();

    auto backgroundTexture = dae::ResourceManager::Get().LoadTexture("background.png");
    auto logoTexture = dae::ResourceManager::Get().LoadTexture("logo.png");

    auto& rm{ dae::ResourceManager::Get() };
    auto font = rm.LoadFont("Lingua.otf", 36);
    auto smallFont = rm.LoadFont("Lingua.otf", 21);
    auto mediumFont = rm.LoadFont("Lingua.otf", 28);

    // Background
    auto* go = scene.GetRoot()->CreateChild(0, 0, 0, "Background");
    go->AddComponent<dae::Sprite>(backgroundTexture, -2);

    // Logo
    go = scene.GetRoot()->CreateChild(358, 150, 0, "Logo");
    go->AddComponent<dae::Sprite>(logoTexture, -1);

    // FPS display
    go = scene.GetRoot()->CreateChild(10, 10, 0, "FPS Counter");
    go->AddComponent<dae::FpsComponent>(font);


    auto& input{ dae::InputManager::Get() };
    // auto& event{ dae::EventManager::Get() };

    auto* infoTextP0{ scene.GetRoot()->CreateChild(10, 60) };
    infoTextP0->AddComponent<dae::TextComponent>("Movement: WASD, Kill enemy: E, Take Damage: Q", smallFont, 1);

    auto* infoTextP1{ scene.GetRoot()->CreateChild(10, 90) };
    infoTextP1->AddComponent<dae::TextComponent>("Movement: DPAD, Kill enemy: X, Take Damage: A", smallFont, 1);

    // Player 0
    {
        auto* player0{ scene.GetRoot()->CreateChild(100, 300, 0, "Player 0") };
        player0->GetTransform().SetScale(4.f, 4.f);

        auto playerTexture{ dae::ResourceManager::Get().LoadTexture("player.png") };

        auto* playerDisplay{ scene.GetRoot()->CreateChild(10, 150, 0, "Player display p0") };
        playerDisplay->AddComponent<dae::TextComponent>("Player 0", mediumFont);

        // NOTE: Ask about how to transfer ownership here
        auto* healthDisplay{ playerDisplay->CreateChild(0, 40, 0, "Health display p0") };
        healthDisplay->AddComponent<dae::HealthComponent>(0);

        auto* scoreDisplay{ healthDisplay->CreateChild(0, 30, 0, "Score display p0") };
        scoreDisplay->AddComponent<dae::ScoreComponent>(0);

        auto spriteSheetTexture{ dae::ResourceManager::Get().LoadTexture("spritesheet.png", SDL_SCALEMODE_PIXELART) };
        const auto size{ spriteSheetTexture->GetSize() };

        player0->AddComponent<dae::PlayerController>(0);
        auto* animation{ player0->AddComponent<dae::Animation>(3) };

        const auto cols{ static_cast<int>(size.x / 16.f) };
        const auto rows{ static_cast<int>(size.y / 16.f) };
        auto& spriteSheet{ animation->AddSpriteSheet(spriteSheetTexture, cols, rows) };

        animation->AddAnimation("walkDown"_h, spriteSheet,
                                {
                                    { 0, 0, 0.1f },
                                    { 1, 0, 0.1f },
                                    { 2, 0, 0.1f },
                                });
        animation->AddAnimation("walkUp"_h, spriteSheet,
                                {
                                    { 6, 0, 0.1f },
                                    { 7, 0, 0.1f },
                                    { 8, 0, 0.1f },
                                });
        animation->AddAnimation("walkLeft"_h, spriteSheet,
                                {
                                    { 3, 0, 0.1f },
                                    { 4, 0, 0.1f },
                                    { 5, 0, 0.1f },
                                });
        animation->AddAnimation("walkRight"_h, spriteSheet,
                                {
                                    { 3, 0, 0.1f, true },
                                    { 4, 0, 0.1f, true },
                                    { 5, 0, 0.1f, true },
                                });

        animation->SetActiveAnimation("walkRight"_h, true);

        input.RegisterInput(SDL_SCANCODE_W, dae::Input::Type::held, "moveUp"_h, 0);
        input.RegisterInput(SDL_SCANCODE_A, dae::Input::Type::held, "moveLeft"_h, 0);
        input.RegisterInput(SDL_SCANCODE_S, dae::Input::Type::held, "moveDown"_h, 0);
        input.RegisterInput(SDL_SCANCODE_D, dae::Input::Type::held, "moveRight"_h, 0);

        input.RegisterInput(SDL_SCANCODE_Q, dae::Input::Type::released, "damage"_h, 0);
        input.RegisterInput(SDL_SCANCODE_E, dae::Input::Type::released, "attack"_h, 0);
    }

    // Player 1
    {
        auto* player1{ scene.GetRoot()->CreateChild(300, 300, 0, "Player 1") };
        auto enemyTexture{ dae::ResourceManager::Get().LoadTexture("enemy.png") };

        auto* playerDisplay{ scene.GetRoot()->CreateChild(10, 280, 0, "Player display p1") };
        playerDisplay->AddComponent<dae::TextComponent>("Player 1", mediumFont);

        auto* healthDisplay{ playerDisplay->CreateChild(0, 40, 0, "Health display p1") };
        healthDisplay->AddComponent<dae::HealthComponent>(1);

        auto* scoreDisplay{ healthDisplay->CreateChild(0, 30, 0, "Score display p1") };
        scoreDisplay->AddComponent<dae::ScoreComponent>(1);

        player1->AddComponent<dae::PlayerController>(1);

        input.RegisterInput(SDL_GAMEPAD_BUTTON_DPAD_UP, dae::Input::Type::held, "moveUp"_h, 1);
        input.RegisterInput(SDL_GAMEPAD_BUTTON_DPAD_LEFT, dae::Input::Type::held, "moveLeft"_h, 1);
        input.RegisterInput(SDL_GAMEPAD_BUTTON_DPAD_DOWN, dae::Input::Type::held, "moveDown"_h, 1);
        input.RegisterInput(SDL_GAMEPAD_BUTTON_DPAD_RIGHT, dae::Input::Type::held, "moveRight"_h, 1);

        input.RegisterInput(SDL_GAMEPAD_BUTTON_SOUTH, dae::Input::Type::released, "damage"_h, 1);
        input.RegisterInput(SDL_GAMEPAD_BUTTON_WEST, dae::Input::Type::released, "attack"_h, 1);
    }

    // Achievement Event
    dae::EventManager::Get().BindEvent("win"_h, &dae::AchievementManager::Get(), &dae::AchievementManager::OnWin);
}

int main()
{
#if __EMSCRIPTEN__
    fs::path data_location = "./resources/";
#else
    const fs::path data_location = "./resources/";
#endif
    try
    {
        constexpr int maxSteps{ 5 };
        int counter{ 0 };
        while(not fs::exists(data_location) and counter < maxSteps)
        {
            fs::current_path("..");
            ++counter;
        }
        fs::current_path(data_location);
    }
    catch(const fs::filesystem_error& e)
    {
        std::println("{}", e.what());
    }

    dae::Minigin engine("");
    engine.Run(load);
    return 0;
}
