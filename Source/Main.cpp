#include <cstddef>
#include <filesystem>
#include <print>

#include "../Include/Services/GameState.hpp"
#include "Constants.hpp"
#include "Galena.hpp"
#include "GameScenes.hpp"
#include "Locator.hpp"
#include "Services/InputManager.hpp"
#include "Services/Renderer.hpp"
#include "Services/SceneManager.hpp"
#include "Services/Sound.hpp"
#include "Utils.hpp"


#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

namespace fs = std::filesystem;

namespace
{

void LoadSounds()
{
    auto& sound{ gla::Locator::Get<gla::Sound>() };
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
}

void load()
{
    LoadSounds();

    // Set logical resolution to be NES size
    auto& renderer{ gla::Locator::Get<gla::Renderer>() };
    renderer.SetLogicalResolution(256, 240);
    renderer.SetBackgroundColor(bt::colors::Black);

    auto& sceneManager = gla::Locator::Get<gla::SceneManager>();

    // Initialise persistent GameState component that will hold info over the wider game
    sceneManager.GetPersistentScene().GetRoot()->AddComponent<bt::GameState>();

    auto& startScene = sceneManager.CreateScene("Start", bt::LoadStartScene);
    sceneManager.CreateScene("Loading", bt::LoadLoadingScene);
    sceneManager.CreateScene("Singleplayer", bt::LoadSinglePlayerGameScene);
    sceneManager.CreateScene("Coop", bt::LoadCoopGameScene);
    sceneManager.CreateScene("Versus", bt::LoadVersusGameScene);

    sceneManager.LoadScene(startScene);
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
