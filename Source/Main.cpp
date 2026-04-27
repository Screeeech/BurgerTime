
#include <filesystem>
#include <print>

#include "AchievementManager.hpp"
#include "Components/Animation.hpp"
#include "Components/FpsComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/PlayerController.hpp"
#include "Components/ScoreComponent.hpp"
#include "Components/Sprite.hpp"
#include "Components/TextComponent.hpp"
#include "EventManager.hpp"
#include "Events.hpp"
#include "Galena.hpp"
#include "InputManager.hpp"
#include "ResourceManager.hpp"
#include "Scene.hpp"
#include "SceneManager.hpp"
#include "Utils.hpp"


#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

namespace fs = std::filesystem;

namespace
{
void load()
{
    auto& scene = gla::SceneManager::Get().CreateScene();
    scene.Load();

    auto backgroundTexture = gla::ResourceManager::Get().LoadTexture("background.png");
    auto logoTexture = gla::ResourceManager::Get().LoadTexture("logo.png");

    auto& rm{gla::ResourceManager::Get()};
    auto font = rm.LoadFont("Lingua.otf", 36);
    auto smallFont = rm.LoadFont("Lingua.otf", 21);
    auto mediumFont = rm.LoadFont("Lingua.otf", 28);

    // Background
    auto* go = scene.GetRoot()->CreateChild(0, 0, 0, "Background");
    go->AddComponent<gla::Sprite>(backgroundTexture, -2);

    // Logo
    go = scene.GetRoot()->CreateChild(358, 150, 0, "Logo");
    go->AddComponent<gla::Sprite>(logoTexture, -1);

    // FPS display
    go = scene.GetRoot()->CreateChild(10, 10, 0, "FPS Counter");
    go->AddComponent<gla::FpsComponent>(font);


    auto& input{gla::InputManager::Get()};
    // auto& event{ dae::EventManager::Get() };

    auto* infoTextP0{scene.GetRoot()->CreateChild(10, 60)};
    infoTextP0->AddComponent<gla::TextComponent>("Movement: WASD, Kill enemy: E, Take Damage: Q", smallFont, 1);

    auto* infoTextP1{scene.GetRoot()->CreateChild(10, 90)};
    infoTextP1->AddComponent<gla::TextComponent>("Movement: DPAD, Kill enemy: X, Take Damage: A", smallFont, 1);

    // Player 0
    {
        auto* player0{scene.GetRoot()->CreateChild(100, 300, 0, "Player 0")};
        player0->GetTransform().SetScale(4.f, 4.f);

        auto playerTexture{gla::ResourceManager::Get().LoadTexture("player.png")};

        auto* playerDisplay{scene.GetRoot()->CreateChild(10, 150, 0, "Player display p0")};
        playerDisplay->AddComponent<gla::TextComponent>("Player 0", mediumFont);

        // NOTE: Ask about how to transfer ownership here
        auto* healthDisplay{playerDisplay->CreateChild(0, 40, 0, "Health display p0")};
        healthDisplay->AddComponent<bt::HealthComponent>(0);

        auto* scoreDisplay{healthDisplay->CreateChild(0, 30, 0, "Score display p0")};
        scoreDisplay->AddComponent<bt::ScoreComponent>(0);

        auto spriteSheetTexture{gla::ResourceManager::Get().LoadTexture("spritesheet.png", SDL_SCALEMODE_PIXELART)};
        auto const size{spriteSheetTexture->GetSize()};

        player0->AddComponent<bt::PlayerController>(0);
        auto* animation{player0->AddComponent<gla::Animation>(3)};

        auto const cols{static_cast<int>(size.x / 16.f)};
        auto const rows{static_cast<int>(size.y / 16.f)};
        auto& spriteSheet{animation->AddSpriteSheet(spriteSheetTexture, cols, rows)};

        animation->AddAnimation(
            "walkDown"_h,
            spriteSheet,
            {
                {.colIdx = 0, .rowIdx = 0, .duration = 0.1f},
                {.colIdx = 1, .rowIdx = 0, .duration = 0.1f},
                {.colIdx = 2, .rowIdx = 0, .duration = 0.1f},
            });
        animation->AddAnimation(
            "walkUp"_h,
            spriteSheet,
            {
                {.colIdx = 6, .rowIdx = 0, .duration = 0.1f},
                {.colIdx = 7, .rowIdx = 0, .duration = 0.1f},
                {.colIdx = 8, .rowIdx = 0, .duration = 0.1f},
            });
        animation->AddAnimation(
            "walkLeft"_h,
            spriteSheet,
            {
                {.colIdx = 3, .rowIdx = 0, .duration = 0.1f},
                {.colIdx = 4, .rowIdx = 0, .duration = 0.1f},
                {.colIdx = 5, .rowIdx = 0, .duration = 0.1f},
            });
        animation->AddAnimation(
            "walkRight"_h,
            spriteSheet,
            {
                {.colIdx = 3, .rowIdx = 0, .duration = 0.1f, .flipX = true},
                {.colIdx = 4, .rowIdx = 0, .duration = 0.1f, .flipX = true},
                {.colIdx = 5, .rowIdx = 0, .duration = 0.1f, .flipX = true},
            });

        animation->SetActiveAnimation("walkRight"_h, true);

        input.RegisterInput(SDL_SCANCODE_W, gla::Input::Type::held, "moveUp"_h, 0);
        input.RegisterInput(SDL_SCANCODE_A, gla::Input::Type::held, "moveLeft"_h, 0);
        input.RegisterInput(SDL_SCANCODE_S, gla::Input::Type::held, "moveDown"_h, 0);
        input.RegisterInput(SDL_SCANCODE_D, gla::Input::Type::held, "moveRight"_h, 0);

        input.RegisterInput(SDL_SCANCODE_Q, gla::Input::Type::released, "damage"_h, 0);
        input.RegisterInput(SDL_SCANCODE_E, gla::Input::Type::released, "attack"_h, 0);
    }

    // Player 1
    {
        auto* player1{scene.GetRoot()->CreateChild(300, 300, 0, "Player 1")};
        auto enemyTexture{gla::ResourceManager::Get().LoadTexture("enemy.png")};

        auto* playerDisplay{scene.GetRoot()->CreateChild(10, 280, 0, "Player display p1")};
        playerDisplay->AddComponent<gla::TextComponent>("Player 1", mediumFont);

        auto* healthDisplay{playerDisplay->CreateChild(0, 40, 0, "Health display p1")};
        healthDisplay->AddComponent<bt::HealthComponent>(1);

        auto* scoreDisplay{healthDisplay->CreateChild(0, 30, 0, "Score display p1")};
        scoreDisplay->AddComponent<bt::ScoreComponent>(1);

        player1->AddComponent<bt::PlayerController>(1);

        input.RegisterInput(SDL_GAMEPAD_BUTTON_DPAD_UP, gla::Input::Type::held, "moveUp"_h, 1);
        input.RegisterInput(SDL_GAMEPAD_BUTTON_DPAD_LEFT, gla::Input::Type::held, "moveLeft"_h, 1);
        input.RegisterInput(SDL_GAMEPAD_BUTTON_DPAD_DOWN, gla::Input::Type::held, "moveDown"_h, 1);
        input.RegisterInput(SDL_GAMEPAD_BUTTON_DPAD_RIGHT, gla::Input::Type::held, "moveRight"_h, 1);

        input.RegisterInput(SDL_GAMEPAD_BUTTON_SOUTH, gla::Input::Type::released, "damage"_h, 1);
        input.RegisterInput(SDL_GAMEPAD_BUTTON_WEST, gla::Input::Type::released, "attack"_h, 1);
    }

    // Achievement Event
    gla::EventManager::Get().BindEvent("win"_h, &bt::AchievementManager::Get(), &bt::AchievementManager::OnWin);
}
}  // namespace

auto main() -> int
{
    fs::path const data_location = "./Resources/";
    try
    {
        int constexpr maxSteps{5};
        int counter{0};
        while (not fs::exists(data_location) and counter < maxSteps)
        {
            fs::current_path("..");
            ++counter;
        }
        fs::current_path(data_location);
    }
    catch (fs::filesystem_error const& e)
    {
        std::println("{}", e.what());
    }

    gla::Galena engine("");
    engine.Run(load);
    return 0;
}
