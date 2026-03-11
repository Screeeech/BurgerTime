#include "InputManager.h"

#include <imgui_impl_sdl3.h>
#include <SDL3/SDL.h>

dae::InputManager::InputManager()
{
    // Move this elsewhere later
    RegisterInput(Input{ SDL_SCANCODE_W, Input::Type::held }, { .name = "moveUp", .playerIndex = 0 });
    RegisterInput(Input{ SDL_SCANCODE_A, Input::Type::held }, { .name = "moveLeft", .playerIndex = 0 });
    RegisterInput(Input{ SDL_SCANCODE_S, Input::Type::held }, { .name = "moveDown", .playerIndex = 0 });
    RegisterInput(Input{ SDL_SCANCODE_D, Input::Type::held }, { .name = "moveRight", .playerIndex = 0 });

    // RegisterInput(Input{ SDL_SCANCODE_W, Input::Type::released }, { .name = "moveReleased", .playerIndex = 0 });
    // RegisterInput(Input{ SDL_SCANCODE_A, Input::Type::released }, { .name = "moveReleased", .playerIndex = 0 });
    // RegisterInput(Input{ SDL_SCANCODE_S, Input::Type::released }, { .name = "moveReleased", .playerIndex = 0 });
    // RegisterInput(Input{ SDL_SCANCODE_D, Input::Type::released }, { .name = "moveReleased", .playerIndex = 0 });
}

bool dae::InputManager::ProcessInput()
{
    ProcessKeyBoardState();

    SDL_Event e;
    while(SDL_PollEvent(&e))
    {
        ImGui_ImplSDL3_ProcessEvent(&e);

        ProcessKeyBoardInput(e);
    }

    return true;
}

bool dae::InputManager::ProcessKeyBoardInput(const SDL_Event& pEvent)
{
    switch(pEvent.type)
    {
        case SDL_EVENT_KEY_UP:
            for(auto& [input, action] : m_registeredInputs)
            {
                if(input.type != Input::Type::released or input.key != pEvent.key.scancode)
                    continue;

                auto [fst, snd] = m_commands.equal_range(action);
                for(auto& [k, command] : std::ranges::subrange(fst, snd))
                {
                    command->Execute();
                    return true;
                }
            }
            break;


        case SDL_EVENT_KEY_DOWN:
            if(pEvent.key.repeat)
                return false;

            for(auto& [input, action] : m_registeredInputs)
            {
                if(input.type != Input::Type::pressed or input.key != pEvent.key.scancode)
                    continue;

                auto [fst, snd] = m_commands.equal_range(action);
                for(auto& [k, command] : std::ranges::subrange(fst, snd))
                {
                    command->Execute();
                    return true;
                }
            }
            break;
        default:;
    }
    return false;
}

void dae::InputManager::ProcessKeyBoardState()
{
    const auto* pKeyboard = SDL_GetKeyboardState(nullptr);
    for(auto& [input, action] : m_registeredInputs)
    {
        if(input.type != Input::Type::held or not pKeyboard[input.key])
            continue;

        auto [fst, snd] = m_commands.equal_range(action);
        for(auto& [k, command] : std::ranges::subrange(fst, snd))
        {
            command->Execute();
        }
    }
}

void dae::InputManager::RegisterInput(const Input& input, const Action& action)
{
    m_registeredInputs.insert({ input, action });
}
