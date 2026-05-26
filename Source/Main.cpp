
#include <cstddef>
#include <filesystem>
#include <print>

#include "AchievementManager.hpp"
#include "Colors.hpp"
#include "Commands/VolumeCommand.hpp"
#include "Components/Animation.hpp"
#include "Components/FpsComponent.hpp"
#include "Components/PlayerController.hpp"
#include "Components/Sprite.hpp"
#include "Components/Stage.hpp"
#include "Components/TextComponent.hpp"
#include "Events.hpp"
#include "Galena.hpp"
#include "Locator.hpp"
#include "Scene.hpp"
#include "SceneManager.hpp"
#include "Services/EventManager.hpp"
#include "Services/InputManager.hpp"
#include "Services/ISound.hpp"
#include "Services/Renderer.hpp"
#include "Services/ResourceManager.hpp"
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
    auto& scene = gla::Locator::Get<gla::SceneManager>().CreateScene();

    // Getting value without checking for nullopt cause if there's no service here I want the program to fail
    auto& resourceManager{ gla::Locator::Get<gla::ResourceManager>() };
    auto& inputManager{ gla::Locator::Get<gla::InputManager>() };
    auto& eventManager{ gla::Locator::Get<gla::EventManager>() };
    auto& renderer{ gla::Locator::Get<gla::Renderer>() };

    auto& sound{ gla::Locator::Get<gla::ISound>() };
    sound.LoadAudio("Sounds/bonus_appear.wav", "bonus_appear"_h);
    sound.LoadAudio("Sounds/bonus_obtained.wav", "bonus_obtained"_h);
    sound.LoadAudio("Sounds/burger_fall.wav", "burger_fall"_h);
    sound.LoadAudio("Sounds/burger_land.wav", "burger_land"_h);
    sound.LoadAudio("Sounds/burger_step.wav", "burger_step"_h);
    sound.LoadAudio("Sounds/coin.wav", "coin"_h);
    sound.LoadAudio("Sounds/death.wav", "death"_h);
    sound.LoadAudio("Sounds/enemy_fall.wav", "enemy_fall.wav"_h);
    sound.LoadAudio("Sounds/enemy_sprayed.wav", "enemy_sprayed.wav"_h);
    sound.LoadAudio("Sounds/enemy_squashed.wav", "enemy_squashed.wav"_h);
    sound.LoadAudio("Sounds/game_start.wav", "game_start"_h);
    sound.LoadAudio("Sounds/pepper_shake.wav", "pepper_shake"_h);
    sound.LoadAudio("Sounds/round_clear.wav", "round_clear"_h);
    sound.LoadAudio("Sounds/system_sound.wav", "system_sound"_h);
    sound.LoadPersistentAudioTrack("Sounds/bgm.wav", "background");

    // Bind commands for global volume control
    inputManager.RegisterInput(SDL_SCANCODE_UP, gla::Input::Type::released, "volumeUp"_h, 0);
    inputManager.RegisterInput(SDL_SCANCODE_DOWN, gla::Input::Type::released, "volumeDown"_h, 0);
    inputManager.BindAction<gla::VolumeCommand>("volumeUp"_h, 0, 0.1f);
    inputManager.BindAction<gla::VolumeCommand>("volumeDown"_h, 0, -0.1f);


    // Set logical resolution to be NES size
    renderer.SetLogicalResolution(256, 240);
    renderer.SetBackgroundColor(bt::colors::Black);

    auto* go = scene.GetRoot()->CreateChild(0, 0, 0, "Stage");
    auto* stage = go->AddComponent<bt::Stage>("Stages/stage1.json");

    auto const spriteSheetTexture{ resourceManager.LoadTexture("Textures/spritesheet.png") };
    auto const font = resourceManager.LoadFont("Fonts/nes.ttf", 8);
    auto const smallFont = resourceManager.LoadFont("Fonts/nes.ttf", 8);

    // FPS display
    go = scene.GetRoot()->CreateChild(10, 10, 0, "FPS Counter");
    go->AddComponent<gla::FpsComponent>(font);

    // Control display
    go = scene.GetRoot()->CreateChild(10, 205);
    go->AddComponent<gla::TextComponent>("PLAY SOUND: Q", smallFont, 3);

    go = scene.GetRoot()->CreateChild(10, 215);
    go->AddComponent<gla::TextComponent>("VOLUME: UP DOWN", smallFont, 3);

    // Player 0
    {
        // GameObject
        auto* player0{ scene.GetRoot()->CreateChild(100, 174, 0, "Player 0") };

        // Animations
        auto* animation{ player0->AddComponent<gla::Animation>(2) };

        auto const size{ spriteSheetTexture->GetSize() };
        auto const cols{ static_cast<int>(size.x / 16.f) };
        auto const rows{ static_cast<int>(size.y / 16.f) };
        auto& spriteSheet{ animation->AddSpriteSheet(spriteSheetTexture, cols, rows) };

        animation->AddAnimation(
            "idle"_h,
            spriteSheet,
            {
                { .colIdx = 1, .rowIdx = 0 },
            });
        animation->AddAnimation(
            "walkDown"_h,
            spriteSheet,
            {
                { .colIdx = 0, .rowIdx = 0, .duration = 4.f / 60.f },
                { .colIdx = 1, .rowIdx = 0, .duration = 4.f / 60.f },
                { .colIdx = 2, .rowIdx = 0, .duration = 4.f / 60.f },
            });
        animation->AddAnimation(
            "walkUp"_h,
            spriteSheet,
            {
                { .colIdx = 6, .rowIdx = 0, .duration = 4.f / 60.f },
                { .colIdx = 7, .rowIdx = 0, .duration = 4.f / 60.f },
                { .colIdx = 8, .rowIdx = 0, .duration = 4.f / 60.f },
            });
        animation->AddAnimation(
            "walkLeft"_h,
            spriteSheet,
            {
                { .colIdx = 3, .rowIdx = 0, .duration = 4.f / 60.f },
                { .colIdx = 4, .rowIdx = 0, .duration = 4.f / 60.f },
                { .colIdx = 5, .rowIdx = 0, .duration = 4.f / 60.f },
            });
        animation->AddAnimation(
            "walkRight"_h,
            spriteSheet,
            {
                { .colIdx = 3, .rowIdx = 0, .duration = 4.f / 60.f, .flipX = true },
                { .colIdx = 4, .rowIdx = 0, .duration = 4.f / 60.f, .flipX = true },
                { .colIdx = 5, .rowIdx = 0, .duration = 4.f / 60.f, .flipX = true },
            });

        animation->SetAnimation("walkRight"_h, true);

        player0->AddComponent<bt::PlayerController>(stage, 0);

        inputManager.RegisterInput(SDL_SCANCODE_W, gla::Input::Type::held, "moveUp"_h, 0);
        inputManager.RegisterInput(SDL_SCANCODE_A, gla::Input::Type::held, "moveLeft"_h, 0);
        inputManager.RegisterInput(SDL_SCANCODE_S, gla::Input::Type::held, "moveDown"_h, 0);
        inputManager.RegisterInput(SDL_SCANCODE_D, gla::Input::Type::held, "moveRight"_h, 0);

        inputManager.RegisterInput(SDL_SCANCODE_Q, gla::Input::Type::released, "damage"_h, 0);
        inputManager.RegisterInput(SDL_SCANCODE_E, gla::Input::Type::released, "attack"_h, 0);
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
                { .colIdx = 4, .rowIdx = 2, .duration = 3.f / 60.f },
                { .colIdx = 5, .rowIdx = 2, .duration = 3.f / 60.f },
            });
        animation->AddAnimation(
            "walkDown"_h,
            spriteSheet,
            {
                { .colIdx = 0, .rowIdx = 2, .duration = 3.f / 60.f },
                { .colIdx = 1, .rowIdx = 2, .duration = 3.f / 60.f },
            });
        animation->AddAnimation(
            "walkLeft"_h,
            spriteSheet,
            {
                { .colIdx = 2, .rowIdx = 2, .duration = 3.f / 60.f },
                { .colIdx = 3, .rowIdx = 2, .duration = 3.f / 60.f },
            });
        animation->AddAnimation(
            "walkRight"_h,
            spriteSheet,
            {
                { .colIdx = 2, .rowIdx = 2, .duration = 3.f / 60.f , .flipX = true },
                { .colIdx = 3, .rowIdx = 2, .duration = 3.f / 60.f , .flipX = true },
            });

        animation->SetAnimation("walkLeft"_h, true);
    }

    // Achievement Event
    eventManager.BindEvent("win"_h, &bt::AchievementManager::Get(), &bt::AchievementManager::OnWin);


    scene.Load();
}

// void listFiles(const fs::path& path = fs::current_path(), size_t depth = 0)
//{
//     for (auto const& entry : fs::directory_iterator(path))
//     {
//         std::println("{}{:5} {}", std::string(depth * 2, ' '), entry.is_directory() ? "[DIR]" : "[FILE]", entry.path().filename().string());
//
//         if (entry.is_directory())
//         {
//             listFiles(entry.path(), depth + 1);
//         }
//     }
// }
}  // namespace


auto main() -> int
{
#ifndef __EMSCRIPTEN__
    fs::path const data_location = "./Resources/";
#else
    fs::path const data_location = "./";
#endif
    try
    {
        // listFiles();
        int constexpr maxSteps{ 5 };
        int counter{ 0 };
        while (not fs::exists(data_location) and counter < maxSteps)
        {
            fs::current_path("..");
            ++counter;
        }
        fs::current_path(data_location);
        gla::Galena engine{ "Burger Time - Galena Engine", 60 };
        engine.Run(load);
    }
    catch (fs::filesystem_error const& e)
    {
        std::println("{}", e.what());
    }

    return 0;
}
