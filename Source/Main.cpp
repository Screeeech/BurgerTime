
#include <cstddef>
#include <filesystem>
#include <print>

#include "AchievementManager.hpp"
#include "Components/Animation.hpp"
#include "Components/FpsComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/PlayerController.hpp"
#include "Components/ScoreComponent.hpp"
#include "Components/Sprite.hpp"
#include "Components/Stage.hpp"
#include "Components/TextComponent.hpp"
#include "EventManager.hpp"
#include "Events.hpp"
#include "Galena.hpp"
#include "InputManager.hpp"
#include "Renderer.hpp"
#include "ResourceManager.hpp"
#include "Scene.hpp"
#include "SceneManager.hpp"
#include "Utils.hpp"


#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

namespace fs = std::filesystem;
namespace vw = std::ranges::views;

namespace
{
void load()
{
    auto& resourceManager = gla::ResourceManager::Get();
    auto& input = gla::InputManager::Get();

    auto& scene = gla::SceneManager::Get().CreateScene();
    scene.Load();

    // Set logical resolution to be NES size
    gla::Renderer::Get().SetLogicalResolution(256, 240);

    auto* go = scene.GetRoot()->CreateChild(0, 0, 0, "Stage");
    go->AddComponent<bt::Stage>("Stages/stage1.json");

    auto const spriteSheetTexture{ gla::ResourceManager::Get().LoadTexture("spritesheet.png") };
    auto const font = resourceManager.LoadFont("Fonts/nes.ttf", 8);

    // FPS display
    go = scene.GetRoot()->CreateChild(10, 10, 0, "FPS Counter");
    go->AddComponent<gla::FpsComponent>(font);

    // Player 0
    {
        // GameObject
        auto* player0{ scene.GetRoot()->CreateChild(100, 100, 0, "Player 0") };

        // Animations
        player0->AddComponent<bt::PlayerController>(0);
        auto* animation{ player0->AddComponent<gla::Animation>(2) };

        auto const size{ spriteSheetTexture->GetSize() };
        auto const cols{ static_cast<int>(size.x / 16.f) };
        auto const rows{ static_cast<int>(size.y / 16.f) };
        auto& spriteSheet{ animation->AddSpriteSheet(spriteSheetTexture, cols, rows) };

        animation->AddAnimation(
            "walkDown"_h,
            spriteSheet,
            {
                { .colIdx = 0, .rowIdx = 0, .duration = 0.1f },
                { .colIdx = 1, .rowIdx = 0, .duration = 0.1f },
                { .colIdx = 2, .rowIdx = 0, .duration = 0.1f },
            });
        animation->AddAnimation(
            "walkUp"_h,
            spriteSheet,
            {
                { .colIdx = 6, .rowIdx = 0, .duration = 0.1f },
                { .colIdx = 7, .rowIdx = 0, .duration = 0.1f },
                { .colIdx = 8, .rowIdx = 0, .duration = 0.1f },
            });
        animation->AddAnimation(
            "walkLeft"_h,
            spriteSheet,
            {
                { .colIdx = 3, .rowIdx = 0, .duration = 0.1f },
                { .colIdx = 4, .rowIdx = 0, .duration = 0.1f },
                { .colIdx = 5, .rowIdx = 0, .duration = 0.1f },
            });
        animation->AddAnimation(
            "walkRight"_h,
            spriteSheet,
            {
                { .colIdx = 3, .rowIdx = 0, .duration = 0.1f, .flipX = true },
                { .colIdx = 4, .rowIdx = 0, .duration = 0.1f, .flipX = true },
                { .colIdx = 5, .rowIdx = 0, .duration = 0.1f, .flipX = true },
            });

        animation->SetActiveAnimation("walkRight"_h, true);

        input.RegisterInput(SDL_SCANCODE_W, gla::Input::Type::held, "moveUp"_h, 0);
        input.RegisterInput(SDL_SCANCODE_A, gla::Input::Type::held, "moveLeft"_h, 0);
        input.RegisterInput(SDL_SCANCODE_S, gla::Input::Type::held, "moveDown"_h, 0);
        input.RegisterInput(SDL_SCANCODE_D, gla::Input::Type::held, "moveRight"_h, 0);

        input.RegisterInput(SDL_SCANCODE_Q, gla::Input::Type::released, "damage"_h, 0);
        input.RegisterInput(SDL_SCANCODE_E, gla::Input::Type::released, "attack"_h, 0);
    }

    // Mr Hotdog
    {
        auto* enemy = scene.GetRoot()->CreateChild(30, 30, 0, "Mr. Hotdog");

        auto* animation = enemy->AddComponent<gla::Animation>(2);
        auto const size{ spriteSheetTexture->GetSize() };
        auto const cols{ static_cast<int>(size.x / 16.f) };
        auto const rows{ static_cast<int>(size.y / 16.f) };
        auto& spriteSheet = animation->AddSpriteSheet(spriteSheetTexture, cols, rows);

        animation->AddAnimation(
            "walkUp"_h,
            spriteSheet,
            {
                { .colIdx = 4, .rowIdx = 2, .duration = 0.1f },
                { .colIdx = 5, .rowIdx = 2, .duration = 0.1f },
            });
        animation->AddAnimation(
            "walkDown"_h,
            spriteSheet,
            {
                { .colIdx = 0, .rowIdx = 2, .duration = 0.1f },
                { .colIdx = 1, .rowIdx = 2, .duration = 0.1f },
            });
        animation->AddAnimation(
            "walkLeft"_h,
            spriteSheet,
            {
                { .colIdx = 2, .rowIdx = 2, .duration = 0.1f },
                { .colIdx = 3, .rowIdx = 2, .duration = 0.1f },
            });
        animation->AddAnimation(
            "walkRight"_h,
            spriteSheet,
            {
                { .colIdx = 2, .rowIdx = 2, .duration = 0.1f, .flipX = true },
                { .colIdx = 3, .rowIdx = 2, .duration = 0.1f, .flipX = true },
            });

        animation->SetActiveAnimation("walkLeft"_h, true);
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
        int constexpr maxSteps{ 5 };
        int counter{ 0 };
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

    gla::Galena engine{ "Burger Time - Galena Engine" };
    engine.Run(load);
    return 0;
}
