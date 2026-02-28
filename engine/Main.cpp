
#include <filesystem>
#include <print>

#include "components/RenderComponent.h"
#include "components/TextComponent.h"
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

    auto font = std::make_shared<dae::Font>("Lingua.otf", 36);
    auto logoTexture = dae::ResourceManager::GetInstance().LoadTexture("logo.png");
    // auto backgroundComponent{};

    // Background
    auto go = std::make_unique<dae::GameObject>();
    auto backgroundTexture = dae::ResourceManager::GetInstance().LoadTexture("background.png");
    go->AddComponent<dae::RenderComponent>(std::move(backgroundTexture), dae::Transform{ 0, 0 });
    scene.Add(std::move(go));
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
        std::println("{}", e.what());
    }
#endif
    dae::Minigin engine(data_location);
    engine.Run(load);
    return 0;
}
