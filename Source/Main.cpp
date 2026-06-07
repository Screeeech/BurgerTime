#include <cstddef>
#include <filesystem>
#include <print>

#include "AchievementManager.hpp"
#include "Commands/VolumeCommand.hpp"
#include "Components/Animation.hpp"
#include "Components/BurgerPart.hpp"
#include "Components/Entity.hpp"
#include "Components/FpsComponent.hpp"
#include "Components/HighScore.hpp"
#include "Components/Pepper.hpp"
#include "Components/Score.hpp"
#include "Components/Sprite.hpp"
#include "Components/Stage.hpp"
#include "Components/TextComponent.hpp"
#include "Constants.hpp"
#include "Events.hpp"
#include "Galena.hpp"
#include "Locator.hpp"
#include "Scene.hpp"
#include "Services/EventManager.hpp"
#include "Services/InputManager.hpp"
#include "Services/ISound.hpp"
#include "Services/Renderer.hpp"
#include "Services/ResourceManager.hpp"
#include "Services/SceneManager.hpp"
#include "Utils.hpp"


#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

namespace fs = std::filesystem;
namespace vw = std::ranges::views;

namespace
{

void LoadSounds()
{
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
    auto& inputManager = gla::Locator::Get<gla::InputManager>();
    inputManager.RegisterInput(SDL_SCANCODE_UP, gla::Input::Type::released, "volumeUp"_h, 0);
    inputManager.RegisterInput(SDL_SCANCODE_DOWN, gla::Input::Type::released, "volumeDown"_h, 0);
    inputManager.BindAction<gla::VolumeCommand>("volumeUp"_h, 0, 0.1f);
    inputManager.BindAction<gla::VolumeCommand>("volumeDown"_h, 0, -0.1f);
}

void load()
{
    auto& scene = gla::Locator::Get<gla::SceneManager>().CreateScene();

    auto& resourceManager{ gla::Locator::Get<gla::ResourceManager>() };
    auto& inputManager{ gla::Locator::Get<gla::InputManager>() };
    // auto& eventManager{ gla::Locator::Get<gla::EventManager>() };
    auto& renderer{ gla::Locator::Get<gla::Renderer>() };

    LoadSounds();

    // Set logical resolution to be NES size
    renderer.SetLogicalResolution(256, 240);
    renderer.SetBackgroundColor(bt::colors::Black);

    auto const font = resourceManager.LoadFont("Fonts/nes.ttf", 8);

    auto* stageObject = scene.GetRoot()->CreateChild(32, 32, "Stage");
    auto* stage = stageObject->AddComponent<bt::Stage>("Stages/stage1.json");

    // FPS display
    //auto* go = scene.GetRoot()->CreateChild(5, 225, "FPS Counter");
    //go->AddComponent<gla::FpsComponent>(font);

    auto* go = scene.GetRoot()->CreateChild(100, 15, "Score");
    go->AddComponent<bt::Score>(font, 0);

    go = scene.GetRoot()->CreateChild(110, 15, "HighScore");
    go->AddComponent<bt::HighScore>(font, "Jane Doe", 20'000);

    // Player 0
    {
        // GameObject
        // auto* pepperObject{ stageObject->CreateChild(0, 0, "Pepper") };
        // auto* pepperComponent{ pepperObject->AddComponent<bt::Pepper>(3) };

        int constexpr playerIndex{ 0 };
        bt::Entity::CreatePlayer(*stage, playerIndex, { 95, -2 });

        inputManager.RegisterInput(SDL_SCANCODE_W, gla::Input::Type::held, "moveUp"_h, playerIndex);
        inputManager.RegisterInput(SDL_SCANCODE_A, gla::Input::Type::held, "moveLeft"_h, playerIndex);
        inputManager.RegisterInput(SDL_SCANCODE_S, gla::Input::Type::held, "moveDown"_h, playerIndex);
        inputManager.RegisterInput(SDL_SCANCODE_D, gla::Input::Type::held, "moveRight"_h, playerIndex);
        inputManager.RegisterInput(SDL_SCANCODE_E, gla::Input::Type::held, "attack"_h, playerIndex);

    }

    // Mr Egg
    {
        int constexpr enemyIndex{ 1 };

        bt::Entity::CreateEnemy(*stage, enemyIndex, {50, -2}, bt::Entity::Type::Egg);

        inputManager.RegisterInput(SDL_SCANCODE_I, gla::Input::Type::held, "moveUp"_h, enemyIndex);
        inputManager.RegisterInput(SDL_SCANCODE_J, gla::Input::Type::held, "moveLeft"_h, enemyIndex);
        inputManager.RegisterInput(SDL_SCANCODE_K, gla::Input::Type::held, "moveDown"_h, enemyIndex);
        inputManager.RegisterInput(SDL_SCANCODE_L, gla::Input::Type::held, "moveRight"_h, enemyIndex);
    }

    // Mr Hotdog
    {
        int constexpr enemyIndex{ 2 };

        bt::Entity::CreateEnemy(*stage, enemyIndex, {30, -2}, bt::Entity::Type::HotDog);

        inputManager.RegisterInput(SDL_SCANCODE_UP, gla::Input::Type::held, "moveUp"_h, enemyIndex);
        inputManager.RegisterInput(SDL_SCANCODE_LEFT, gla::Input::Type::held, "moveLeft"_h, enemyIndex);
        inputManager.RegisterInput(SDL_SCANCODE_DOWN, gla::Input::Type::held, "moveDown"_h, enemyIndex);
        inputManager.RegisterInput(SDL_SCANCODE_RIGHT, gla::Input::Type::held, "moveRight"_h, enemyIndex);
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
