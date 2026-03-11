#include <SDL3/SDL_render.h>

#include <filesystem>
#include <glm/gtc/constants.hpp>
#include <print>

#include "commands/CallbackCommand.h"
#include "components/CacheComponent.h"
#include "components/FpsComponent.h"
#include "components/PlayerController.h"
#include "components/RenderComponent.h"
#include "components/RotatorComponent.h"
#include "components/TextComponent.h"
#include "InputManager.h"
#include "Minigin.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Texture2D.h"

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


    // Input testing
    auto& input = dae::InputManager::GetInstance();
    input.BindAction<dae::CallbackCommand>("moveUp", 0, []() { std::println("Moving up!"); });
    input.BindAction<dae::CallbackCommand>("moveLeft", 0, []() { std::println("Moving left!"); });
    input.BindAction<dae::CallbackCommand>("moveDown", 0, []() { std::println("Moving down!"); });
    input.BindAction<dae::CallbackCommand>("moveRight", 0, []() { std::println("Moving right!"); });

    {
        auto* test = new dae::GameObject(10, 10, 0, "Test");
        auto earthTexture = dae::ResourceManager::GetInstance().LoadTexture("earth.png");

        test->AddComponent<dae::RenderComponent>(earthTexture);
        test->AddComponent<dae::PlayerController>(0);
        scene.Add(test);
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
