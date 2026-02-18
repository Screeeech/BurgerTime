
#include <iostream>
#include <print>

#include "components/FpsComponent.h"
#include "components/SpriteComponent.h"
#include "components/TextComponent.h"
#include "Texture2D.h"
#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#include <filesystem>

#include "Minigin.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "SceneManager.h"
namespace fs = std::filesystem;

static void load()
{
    auto& scene = dae::SceneManager::GetInstance().CreateScene();

    auto font = std::make_shared<dae::Font>("Lingua.otf", 36);
    auto backgroundTexture = dae::ResourceManager::GetInstance().LoadTexture("background.png");
    auto logoTexture = dae::ResourceManager::GetInstance().LoadTexture("logo.png");

    auto fpsComponent{ std::make_shared<FpsComponent>( std::move(font), dae::Transform{ 10, 10 } ) };
    auto backgroundComponent{ std::make_shared<SpriteComponent>( std::move(backgroundTexture), dae::Transform{ 0, 0 } ) };
    auto logoComponent{ std::make_shared<SpriteComponent>(std::move(logoTexture), dae::Transform{ 358, 180 } ) };

    // Background
    auto go = std::make_unique<dae::GameObject>(std::vector<std::shared_ptr<Component>>{ std::move(backgroundComponent) });
    scene.Add(std::move(go));

    // Logo
    go = std::make_unique<dae::GameObject>(std::vector<std::shared_ptr<Component>>{ std::move(logoComponent) });
    scene.Add(std::move(go));

    // FPS counter
    go = std::make_unique<dae::GameObject>(std::vector<std::shared_ptr<Component>>{ fpsComponent });
    scene.Add(std::move(go));

    // auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
    // auto to = std::make_unique<dae::TextObject>("Programming 4 Assignment", font);
    // to->SetColor({ 255, 255, 0, 255 });
    // to->SetPosition(292, 20);
    // scene.Add(std::move(to));
}

int main(int, char*[])
{
#if __EMSCRIPTEN__
    fs::path data_location = "";
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
        std::println("{}",e.what());
    }
#endif
    dae::Minigin engine(data_location);
    engine.Run(load);
    return 0;
}
