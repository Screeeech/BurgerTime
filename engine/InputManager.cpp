#include "InputManager.h"

#include <imgui_impl_sdl3.h>
#include <SDL3/SDL.h>

dae::InputManager::InputManager()
{
    // Move this elsewhere later
    RegisterInput( Input{ SDL_SCANCODE_W }, { .name = "moveUp", .playerIndex = 0 });
    RegisterInput( Input{ SDL_SCANCODE_A }, { .name = "moveLeft", .playerIndex = 0 });
    RegisterInput( Input{ SDL_SCANCODE_S }, { .name = "moveDown", .playerIndex = 0 });
    RegisterInput( Input{ SDL_SCANCODE_D }, { .name = "moveRight", .playerIndex = 0 });
}

bool dae::InputManager::ProcessInput()
{
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
                if(input.key == pEvent.key.scancode)
                {
                    auto range = m_commands.equal_range(action);
                    for(auto& [key, command] : std::ranges::subrange(range.first, range.second))
                    {
                        command->Execute();
                        return true;
                    }
                }
            }
            break;
        default:;
    }
    return false;
}

void dae::InputManager::RegisterInput(const Input& input, const Action& action)
{
    m_registeredInputs.insert({ input, action });
}
