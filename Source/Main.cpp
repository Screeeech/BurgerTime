#include <cstddef>
#include <filesystem>
#include <print>

#include "AchievementManager.hpp"
#include "Colors.hpp"
#include "Commands/VolumeCommand.hpp"
#include "Components/Animation.hpp"
#include "Components/Enemy.hpp"
#include "Components/FpsComponent.hpp"
#include "Components/Pepper.hpp"
#include "Components/Player.hpp"
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

    auto& resourceManager{ gla::Locator::Get<gla::ResourceManager>() };
    auto& inputManager{ gla::Locator::Get<gla::InputManager>() };
    // auto& eventManager{ gla::Locator::Get<gla::EventManager>() };
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

    auto* go = scene.GetRoot()->CreateChild(0, 0, "Stage");
    auto* stage = go->AddComponent<bt::Stage>("Stages/stage1.json");

    auto const spriteSheetTexture{ resourceManager.LoadTexture("Textures/spritesheet.png") };
    auto const font = resourceManager.LoadFont("Fonts/nes.ttf", 8);
    auto const smallFont = resourceManager.LoadFont("Fonts/nes.ttf", 8);

    // FPS display
    go = scene.GetRoot()->CreateChild(10, 10, "FPS Counter");
    go->AddComponent<gla::FpsComponent>(font);

    // Control display
    go = scene.GetRoot()->CreateChild(10, 205);
    go->AddComponent<gla::TextComponent>("PLAY SOUND: Q", smallFont, 3);

    go = scene.GetRoot()->CreateChild(10, 215);
    go->AddComponent<gla::TextComponent>("VOLUME: UP DOWN", smallFont, 3);

    // Player 0
    {
        // GameObject
        auto* pepperObject{ scene.GetRoot()->CreateChild(0, 0, "Pepper") };
        auto* pepperComponent{ pepperObject->AddComponent<bt::Pepper>(3) };

        auto* player0{ scene.GetRoot()->CreateChild(100, 174, "Player 0") };
        bt::Player::DefineAnimations(*player0->AddComponent<gla::Animation>(2), spriteSheetTexture);

        player0->AddComponent<bt::Player>(stage, pepperComponent, 0);

        inputManager.RegisterInput(SDL_SCANCODE_W, gla::Input::Type::held, "moveUp"_h, 0);
        inputManager.RegisterInput(SDL_SCANCODE_A, gla::Input::Type::held, "moveLeft"_h, 0);
        inputManager.RegisterInput(SDL_SCANCODE_S, gla::Input::Type::held, "moveDown"_h, 0);
        inputManager.RegisterInput(SDL_SCANCODE_D, gla::Input::Type::held, "moveRight"_h, 0);

        inputManager.RegisterInput(SDL_SCANCODE_Q, gla::Input::Type::released, "damage"_h, 0);
        inputManager.RegisterInput(SDL_SCANCODE_E, gla::Input::Type::released, "attack"_h, 0);
    }

    // Mr Hotdog
    {
        auto* enemy = scene.GetRoot()->CreateChild(50, 174, "Mr. Hotdog");
        bt::Enemy::DefineAnimations(*enemy->AddComponent<gla::Animation>(2), spriteSheetTexture);
        enemy->AddComponent<bt::Enemy>(stage);

        inputManager.RegisterInput(SDL_SCANCODE_UP, gla::Input::Type::held, "moveUp"_h, 2);
        inputManager.RegisterInput(SDL_SCANCODE_LEFT, gla::Input::Type::held, "moveLeft"_h, 2);
        inputManager.RegisterInput(SDL_SCANCODE_DOWN, gla::Input::Type::held, "moveDown"_h, 2);
        inputManager.RegisterInput(SDL_SCANCODE_RIGHT, gla::Input::Type::held, "moveRight"_h, 2);
    }


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
