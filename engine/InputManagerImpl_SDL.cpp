//
// Created by lily-laptop on 11/03/2026.
//

#include <print>

#include "InputManager.h"

namespace dae
{

class InputManager::Impl
{
public:
    Impl()
    {
        int gamepadCount{};
        auto* pJoyStickID = SDL_GetGamepads(&gamepadCount);
        m_pGamepad = SDL_OpenGamepad(pJoyStickID[0]);

        SDL_free(pJoyStickID);
    }

    ~Impl()
    {
        if(m_pGamepad)
            SDL_CloseGamepad(m_pGamepad);
    }

    bool ProcessGamepadInput(const SDL_Event& event, InputManager* im)
    {
        switch(event.type)
        {
            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                if(event.key.repeat)
                    return false;
                return CheckInputPressed(Input::Type::pressed, static_cast<SDL_GamepadButton>(event.gbutton.button), im);

            case SDL_EVENT_GAMEPAD_BUTTON_UP:
                return CheckInputPressed(Input::Type::released, static_cast<SDL_GamepadButton>(event.gbutton.button), im);

            case SDL_EVENT_GAMEPAD_ADDED:
                std::println("Gamepad added event");
                if(not m_pGamepad)
                {
                    m_pGamepad = SDL_OpenGamepad(event.gdevice.which);
                    if(m_pGamepad)
                        std::println("Gamepad connected!");
                }
                else
                    std::println("Gamepad already connected");
                return true;

            case SDL_EVENT_GAMEPAD_REMOVED:
                if(m_pGamepad)
                {
                    std::println("Gamepad removed!");
                    m_pGamepad = nullptr;
                }
                return true;
            default:;
        }

        return false;
    }

    void ProcessGamepadState(InputManager* im)
    {
        if(not m_pGamepad)
            return;

        for(auto& [action, input] : im->m_registeredInputs)
        {
            auto button = input.GetButton();
            if(not button.has_value() or input.type != Input::Type::held)
                continue;

            if(not SDL_GetGamepadButton(m_pGamepad, button.value()))
                continue;

            auto [fst, snd] = im->m_commands.equal_range(action);
            for(auto& [k, command] : std::ranges::subrange(fst, snd))
            {
                command->Execute();
            }
        }
    }

private:
    bool CheckInputPressed(Input::Type inputType, SDL_GamepadButton button, InputManager* im)
    {
        for(auto& [action, input] : im->m_registeredInputs)
        {
            if(input.type != inputType or not input.InputDataMatches(button))
                continue;

            auto range = im->m_commands.equal_range(action);
            for(auto it = range.first; it != range.second; it++)
            {
                it->second->Execute();
                return true;
            }
        }
        return false;
    }

    SDL_Gamepad* m_pGamepad{};
};

InputManager::InputManager()
    : m_pImpl(std::make_unique<Impl>())
{
    InitializeInputs();
}

InputManager::~InputManager() {}

bool InputManager::ProcessGamepadInput(const SDL_Event& event, InputManager* im)
{
    return m_pImpl->ProcessGamepadInput(event, im);
}

void InputManager::ProcessGamepadState(InputManager* im)
{
    m_pImpl->ProcessGamepadState(im);
}

}  // namespace dae
