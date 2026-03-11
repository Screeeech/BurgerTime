#include "InputManager.h"

#include <imgui_impl_sdl3.h>
#include <SDL3/SDL.h>

#include <algorithm>
#include <print>

#include "commands/CallbackCommand.h"

namespace dae
{
}

void dae::InputManager::InitializeInputs()
{
    // Move this elsewhere later
    RegisterInput(Input{ SDL_SCANCODE_W, Input::Type::held }, { .name = "moveUp", .playerIndex = 0 });
    RegisterInput(Input{ SDL_SCANCODE_A, Input::Type::held }, { .name = "moveLeft", .playerIndex = 0 });
    RegisterInput(Input{ SDL_SCANCODE_S, Input::Type::held }, { .name = "moveDown", .playerIndex = 0 });
    RegisterInput(Input{ SDL_SCANCODE_D, Input::Type::held }, { .name = "moveRight", .playerIndex = 0 });
    RegisterInput(Input{ SDL_SCANCODE_SPACE, Input::Type::released }, { .name = "jump", .playerIndex = 0 });

    RegisterInput(Input{ SDL_GAMEPAD_BUTTON_DPAD_UP, Input::Type::held }, { .name = "moveUp", .playerIndex = 0 });
    RegisterInput(Input{ SDL_GAMEPAD_BUTTON_DPAD_LEFT, Input::Type::held }, { .name = "moveLeft", .playerIndex = 0 });
    RegisterInput(Input{ SDL_GAMEPAD_BUTTON_DPAD_DOWN, Input::Type::held }, { .name = "moveDown", .playerIndex = 0 });
    RegisterInput(Input{ SDL_GAMEPAD_BUTTON_DPAD_RIGHT, Input::Type::held }, { .name = "moveRight", .playerIndex = 0 });

    BindAction<dae::CallbackCommand>("jump", 0, []() { std::println("Jumping!"); });
}

bool dae::InputManager::ProcessInput()
{
    ProcessGamepadState(this);
    ProcessKeyBoardState();

    SDL_Event e;
    while(SDL_PollEvent(&e))
    {
        ImGui_ImplSDL3_ProcessEvent(&e);

        if(e.type == SDL_EVENT_QUIT)
            return false;

        if(ProcessGamepadInput(e, this))
            continue;

        ProcessKeyBoardInput(e);
    }

    return true;
}

bool dae::InputManager::ProcessKeyBoardInput(const SDL_Event& event)
{
    switch(event.type)
    {
        case SDL_EVENT_KEY_UP:
            return CheckInputPressed(Input::Type::released, event.key.scancode);

        case SDL_EVENT_KEY_DOWN:
            if(event.key.repeat)
                return false;
            return CheckInputPressed(Input::Type::pressed, event.key.scancode);
        default:;
    }
    return false;
}

void dae::InputManager::ProcessKeyBoardState()
{
    const auto* pKeyboard = SDL_GetKeyboardState(nullptr);
    for(auto& [action, input] : m_registeredInputs)
    {
        auto inputKey = input.GetKey();
        if(not inputKey.has_value())
            continue;

        if(input.type != Input::Type::held or not pKeyboard[inputKey.value()])
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
    m_registeredInputs.insert({ action, input });
}

bool dae::InputManager::CheckInputPressed(Input::Type inputType, SDL_Scancode key)
{
    for(auto& [action, input] : m_registeredInputs)
    {
        if(input.type != inputType)
            continue;

        if(not input.InputDataMatches(key))
            continue;

        auto range = m_commands.equal_range(action);
        for(auto it = range.first; it != range.second; it++)
        {
            it->second->Execute();
            return true;
        }
    }
    return false;
}

bool dae::Input::InputDataMatches(SDL_Scancode key) const
{
    if(not std::holds_alternative<SDL_Scancode>(data))
        return false;

    const auto scancode = std::get<SDL_Scancode>(data);
    return scancode == key;
}

bool dae::Input::InputDataMatches(SDL_GamepadButton button) const
{
    if(not std::holds_alternative<SDL_GamepadButton>(data))
        return false;

    return std::get<SDL_GamepadButton>(data) == button;
}

std::optional<SDL_Scancode> dae::Input::GetKey() const
{
    if(std::holds_alternative<SDL_Scancode>(data))
        return std::get<SDL_Scancode>(data);

    return std::nullopt;
}

std::optional<SDL_GamepadButton> dae::Input::GetButton() const
{
    if(std::holds_alternative<SDL_GamepadButton>(data))
        return std::get<SDL_GamepadButton>(data);

    return std::nullopt;
}
