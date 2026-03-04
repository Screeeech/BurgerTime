
#include <SDL3/SDL_render.h>

#include <filesystem>
#include <glm/gtc/constants.hpp>
#include <print>

#include "components/FpsComponent.h"
#include "components/RenderComponent.h"
#include "components/RotatorComponent.h"
#include "components/TextComponent.h"
#include "imgui.h"
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
    // Temporary
    constexpr int width{ 1024 };
    constexpr int height{ 576 };

    auto& scene = dae::SceneManager::GetInstance().CreateScene();
    scene.Load();

    auto backgroundTexture = dae::ResourceManager::GetInstance().LoadTexture("background.png");
    auto logoTexture = dae::ResourceManager::GetInstance().LoadTexture("logo.png");
    auto font = std::make_shared<dae::Font>("Lingua.otf", 36);

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

    // UI component
    // If I were to pass a lambda to it to define the window layout.
    //

    // Orbiters
    auto sunTexture = dae::ResourceManager::GetInstance().LoadTexture("sun.png");
    auto earthTexture = dae::ResourceManager::GetInstance().LoadTexture("earth.png");
    auto moonTexture = dae::ResourceManager::GetInstance().LoadTexture("moon.png");

    // Creating children here would instantiate a new unique_ptr under the parent
    // So we don't have to make a unique_ptr here

    auto getCenterPos = [width, height](std::shared_ptr<dae::Texture2D>& texture) -> glm::vec2
    {
        return glm::vec2{ (static_cast<float>(width) / 2) - (static_cast<float>(texture->GetSDLTexture()->w) / 2),
                          (static_cast<float>(height) / 2) - (static_cast<float>(texture->GetSDLTexture()->h) / 2) };
    };

    // Sun
    const auto sunPos{ getCenterPos(sunTexture) + glm::vec2{ 0, 20 } };
    auto* sun = new dae::GameObject(sunPos.x, sunPos.y, 0, "Sun");
    sun->AddComponent<dae::RenderComponent>(sunTexture);

    // Earth
    auto* earth = new dae::GameObject(180, 30, 0, "Earth");
    earth->AddComponent<dae::RenderComponent>(earthTexture);
    earth->AddComponent<dae::RotatorComponent>(glm::pi<float>() / 4);

    auto* moon = new dae::GameObject(100, 10, 0, "Moon");
    moon->AddComponent<dae::RenderComponent>(moonTexture);
    moon->AddComponent<dae::RotatorComponent>(glm::pi<float>() / 2);

    scene.Add(sun);
    earth->SetParent(sun, false);
    moon->SetParent(earth, false);

    // UI
    auto* ui = new dae::GameObject(0, 0, 0, "UI");
    ui->AddComponent<dae::UIComponent>(
        [](dae::GameObject* pCaller) mutable
        {
            ImGui::Begin("Test test");

            ImGui::Text(std::format("Drawing from GameObject: {}", pCaller->GetName()).c_str());

            ImGui::End();
        });
    scene.Add(ui);
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
