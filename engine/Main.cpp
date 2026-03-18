
#include <filesystem>
#include <print>

#include "components/FpsComponent.h"
#include "components/PlayerController.h"
#include "components/RenderComponent.h"
#include "components/TextComponent.h"
#include "EventManager.h"
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
    auto& scene = dae::SceneManager::GetInstance().CreateScene();
    scene.Load();

    auto backgroundTexture = dae::ResourceManager::GetInstance().LoadTexture("background.png");
    auto logoTexture = dae::ResourceManager::GetInstance().LoadTexture("logo.png");

    auto font = std::make_shared<dae::Font>("Lingua.otf", 36.f);
    auto smallFont = std::make_shared<dae::Font>("Lingua.otf", 21.f);
    auto mediumFont = std::make_shared<dae::Font>("Lingua.otf", 28.f);

    // Background
    auto* go = new dae::GameObject(0, 0, 0, "Background");
    go->AddComponent<dae::RenderComponent>(backgroundTexture);
    scene.Add(go);

    // Logo
    go = new dae::GameObject(358, 80, 0, "Logo");
    go->AddComponent<dae::RenderComponent>(logoTexture);
    scene.Add(go);

    // Text display
    go = new dae::GameObject(10, 10, 0, "FPS Counter");
    go->AddComponent<dae::FpsComponent>(font);
    scene.Add(go);


    auto& input{ dae::InputManager::GetInstance() };
    // auto& event{ dae::EventManager::GetInstance() };

    // Player 0
    {
        auto* player0 = new dae::GameObject(100, 300, 0, "Player 0");
        auto earthTexture = dae::ResourceManager::GetInstance().LoadTexture("earth.png");

        auto* playerDisplay = new dae::GameObject(10, 100);
        playerDisplay->AddComponent<dae::TextComponent>("Player 0", mediumFont);
        scene.Add(playerDisplay);

        auto* healthDisplay = new dae::GameObject(0, 40);
        auto* healthDisplayComponent{ healthDisplay->AddComponent<dae::TextComponent>("Lives: ", smallFont) };
        healthDisplay->SetParent(playerDisplay);

        auto* scoreDisplay = new dae::GameObject(0, 30);
        auto* scoreDisplayComponent{ scoreDisplay->AddComponent<dae::TextComponent>("Score: ", smallFont) };
        scoreDisplay->SetParent(healthDisplay);

        player0->AddComponent<dae::RenderComponent>(earthTexture);
        player0->AddComponent<dae::PlayerController>(0, healthDisplayComponent, scoreDisplayComponent);
        scene.Add(player0);

        input.RegisterInput( SDL_SCANCODE_W, dae::Input::Type::held, "moveUp", 0 );
        input.RegisterInput( SDL_SCANCODE_A, dae::Input::Type::held, "moveLeft",0 );
        input.RegisterInput( SDL_SCANCODE_S, dae::Input::Type::held, "moveDown",0 );
        input.RegisterInput( SDL_SCANCODE_D, dae::Input::Type::held, "moveRight",0 );

        input.RegisterInput( SDL_SCANCODE_Q, dae::Input::Type::released, "damage",0 );
        input.RegisterInput( SDL_SCANCODE_E, dae::Input::Type::released, "attack",0 );
    }

    // Player 1
    {
        auto* player1 = new dae::GameObject(300, 300, 0, "Player 1");
        auto earthTexture = dae::ResourceManager::GetInstance().LoadTexture("sun.png");

        auto* playerDisplay = new dae::GameObject(10, 230);
        playerDisplay->AddComponent<dae::TextComponent>("Player 1", mediumFont);
        scene.Add(playerDisplay);

        auto* healthDisplay = new dae::GameObject(0, 40);
        auto* healthDisplayComponent{ healthDisplay->AddComponent<dae::TextComponent>("Lives: ", smallFont) };
        healthDisplay->SetParent(playerDisplay);

        auto* scoreDisplay = new dae::GameObject(0, 30);
        auto* scoreDisplayComponent{ scoreDisplay->AddComponent<dae::TextComponent>("Score: ", smallFont) };
        scoreDisplay->SetParent(healthDisplay);

        player1->AddComponent<dae::RenderComponent>(earthTexture);
        player1->AddComponent<dae::PlayerController>(1, healthDisplayComponent, scoreDisplayComponent);
        scene.Add(player1);


        input.RegisterInput( SDL_GAMEPAD_BUTTON_DPAD_UP, dae::Input::Type::held, "moveUp", 1 );
        input.RegisterInput( SDL_GAMEPAD_BUTTON_DPAD_LEFT, dae::Input::Type::held, "moveLeft",1 );
        input.RegisterInput( SDL_GAMEPAD_BUTTON_DPAD_DOWN, dae::Input::Type::held, "moveDown",1 );
        input.RegisterInput( SDL_GAMEPAD_BUTTON_DPAD_RIGHT, dae::Input::Type::held, "moveRight",1 );

        input.RegisterInput( SDL_GAMEPAD_BUTTON_SOUTH, dae::Input::Type::released, "damage",1 );
        input.RegisterInput( SDL_GAMEPAD_BUTTON_WEST, dae::Input::Type::released, "attack",1 );
    }
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
