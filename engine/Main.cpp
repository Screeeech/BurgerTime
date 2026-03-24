
#include <filesystem>
#include <print>

#include "AchievementManager.h"
#include "components/FpsComponent.h"
#include "components/PlayerController.h"
#include "components/RenderComponent.h"
#include "components/TextComponent.h"
#include "EventManager.h"
#include "Events.h"
#include "InputManager.h"
#include "Minigin.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "SceneManager.h"

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

    auto font = std::make_shared<dae::Font>("Lingua.otf", 36.f);
    auto smallFont = std::make_shared<dae::Font>("Lingua.otf", 21.f);
    auto mediumFont = std::make_shared<dae::Font>("Lingua.otf", 28.f);

    // Background
    auto* go = scene.GetRoot()->CreateChild(0, 0, 0, "Background");
    go->AddComponent<dae::RenderComponent>(backgroundTexture, -2);

    // Logo
    go = scene.GetRoot()->CreateChild(358, 150, 0, "Logo");
    go->AddComponent<dae::RenderComponent>(logoTexture, -1);

    // FPS display
    go = scene.GetRoot()->CreateChild(10, 10, 0, "FPS Counter");
    go->AddComponent<dae::FpsComponent>(font);


    auto& input{ dae::InputManager::Get() };
    // auto& event{ dae::EventManager::GetInstance() };

    auto* infoTextP0{ scene.GetRoot()->CreateChild(10, 60) };
    infoTextP0->AddComponent<dae::TextComponent>("Movement: WASD, Kill enemy: E, Take Damage: Q", smallFont, 1);

    auto* infoTextP1{ scene.GetRoot()->CreateChild(10, 90) };
    infoTextP1->AddComponent<dae::TextComponent>("Movement: DPAD, Kill enemy: X, Take Damage: A", smallFont, 1);

    // Player 0
    {
        auto* player0{ scene.GetRoot()->CreateChild(100, 300, 0, "Player 0") };
        auto playerTexture{ dae::ResourceManager::Get().LoadTexture("player.png") };

        auto* playerDisplay{ scene.GetRoot()->CreateChild(10, 150, 0, "Player display p0") };
        playerDisplay->AddComponent<dae::TextComponent>("Player 0", mediumFont);

        // NOTE: Ask about how to transfer ownership here
        auto* healthDisplay{ playerDisplay->CreateChild(0, 40, 0, "Health display p0") };
        auto* healthDisplayComponent{ healthDisplay->AddComponent<dae::TextComponent>("Lives: ", smallFont, 1) };

        auto* scoreDisplay{ healthDisplay->CreateChild(0, 30, 0, "Score display p0") };
        auto* scoreDisplayComponent{ scoreDisplay->AddComponent<dae::TextComponent>("Score: ", smallFont, 1) };

        player0->AddComponent<dae::RenderComponent>(playerTexture);
        player0->AddComponent<dae::PlayerController>(0, healthDisplayComponent, scoreDisplayComponent);

        input.RegisterInput(SDL_SCANCODE_W, dae::Input::Type::held, dae::sdbm("moveUp"), 0);
        input.RegisterInput(SDL_SCANCODE_A, dae::Input::Type::held, dae::sdbm("moveLeft"), 0);
        input.RegisterInput(SDL_SCANCODE_S, dae::Input::Type::held, dae::sdbm("moveDown"), 0);
        input.RegisterInput(SDL_SCANCODE_D, dae::Input::Type::held, dae::sdbm("moveRight"), 0);

        input.RegisterInput(SDL_SCANCODE_Q, dae::Input::Type::released, dae::sdbm("damage"), 0);
        input.RegisterInput(SDL_SCANCODE_E, dae::Input::Type::released, dae::sdbm("attack"), 0);
    }

    // Player 1
    {
        auto* player1{ scene.GetRoot()->CreateChild(300, 300, 0, "Player 1") };
        auto enemyTexture{ dae::ResourceManager::Get().LoadTexture("enemy.png") };

        auto* playerDisplay{ scene.GetRoot()->CreateChild(10, 280, 0, "Player display p1") };
        playerDisplay->AddComponent<dae::TextComponent>("Player 1", mediumFont);

        auto* healthDisplay{ playerDisplay->CreateChild(0, 40, 0, "Health display p1") };
        auto* healthDisplayComponent{ healthDisplay->AddComponent<dae::TextComponent>("Lives: ", smallFont) };

        auto* scoreDisplay{ healthDisplay->CreateChild(0, 30, 0, "Score display p1") };
        auto* scoreDisplayComponent{ scoreDisplay->AddComponent<dae::TextComponent>("Score: ", smallFont) };

        player1->AddComponent<dae::RenderComponent>(enemyTexture);
        player1->AddComponent<dae::PlayerController>(1, healthDisplayComponent, scoreDisplayComponent);

        input.RegisterInput(SDL_GAMEPAD_BUTTON_DPAD_UP, dae::Input::Type::held, dae::sdbm("moveUp"), 1);
        input.RegisterInput(SDL_GAMEPAD_BUTTON_DPAD_LEFT, dae::Input::Type::held, dae::sdbm("moveLeft"), 1);
        input.RegisterInput(SDL_GAMEPAD_BUTTON_DPAD_DOWN, dae::Input::Type::held, dae::sdbm("moveDown"), 1);
        input.RegisterInput(SDL_GAMEPAD_BUTTON_DPAD_RIGHT, dae::Input::Type::held, dae::sdbm("moveRight"), 1);

        input.RegisterInput(SDL_GAMEPAD_BUTTON_SOUTH, dae::Input::Type::released, dae::sdbm("damage"), 1);
        input.RegisterInput(SDL_GAMEPAD_BUTTON_WEST, dae::Input::Type::released, dae::sdbm("attack"), 1);
    }

    // Achievement Event
    dae::EventManager::Get().BindEvent(dae::sdbm("win"), &dae::AchievementManager::Get(), &dae::AchievementManager::OnWin);
}

int main()
{
#if __EMSCRIPTEN__
    fs::path data_location = "./resources/";
#else
    const fs::path data_location = "./resources/";
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
#endif
    dae::Minigin engine(data_location);
    engine.Run(load);
    return 0;
}
