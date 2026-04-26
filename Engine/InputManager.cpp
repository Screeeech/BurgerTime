#include "InputManager.h"

#include <imgui_impl_sdl3.h>
#include <SDL3/SDL.h>

#include <algorithm>
#include <print>

#include "Commands/CallbackCommand.h"

dae::InputManager::~InputManager() noexcept
{
    if(m_pGamepad)
        SDL_CloseGamepad(m_pGamepad);
}

void dae::InputManager::Init()
{
    int count = 0;
    SDL_JoystickID* ids = SDL_GetGamepads(&count);
    const SDL_Gamepad* gamepad{};

    for(int i{}; i < count; i++)
    {
        SDL_Gamepad* gp = SDL_OpenGamepad(ids[i]);
        if(not gamepad)
            gamepad = gp;

        std::println("Gamepad connected: {}", SDL_GetGamepadName(gp));

        // Close the other gamepads
        if(i > 0)
        {
            SDL_CloseGamepad(gp);
        }
    }

    SDL_free(ids);
}

bool dae::InputManager::ProcessInput()
{
    ProcessInputHeld();

    SDL_Event e;
    while(SDL_PollEvent(&e))
    {
        ImGui_ImplSDL3_ProcessEvent(&e);

        if(e.type == SDL_EVENT_QUIT)
            return false;

        switch(e.type)
        {
            case SDL_EVENT_KEY_UP:
                HandleInputEvent(Input::Type::released, e.key.scancode);
                break;

            case SDL_EVENT_KEY_DOWN:
                if(e.key.repeat)
                    break;
                HandleInputEvent(Input::Type::pressed, e.key.scancode);
                break;

            case SDL_EVENT_GAMEPAD_ADDED:
                std::println("Gamepad added event");
                if(not m_pGamepad)
                {
                    m_pGamepad = SDL_OpenGamepad(e.gdevice.which);
                    if(m_pGamepad)
                        std::println("Gamepad connected!");
                }
                else
                    std::println("Gamepad already connected");
                break;
            case SDL_EVENT_GAMEPAD_REMOVED:
                if(m_pGamepad)
                {
                    std::println("Gamepad removed!");
                    m_pGamepad = nullptr;
                }
                break;

            case SDL_EVENT_GAMEPAD_BUTTON_UP:
                HandleInputEvent(Input::Type::released, static_cast<SDL_GamepadButton>(e.gbutton.button));
                break;

            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                if(e.key.repeat)
                    break;
                HandleInputEvent(Input::Type::pressed, static_cast<SDL_GamepadButton>(e.gbutton.button));
                break;

            default:;
        }
    }

    return true;
}

void dae::InputManager::ProcessInputHeld()
{
    for(auto& [action, input] : m_registeredInputs)
    {
        if(input.type != Input::Type::held)
            continue;

        if(std::holds_alternative<SDL_Scancode>(input.data))
        {
            const auto* pKeyboard = SDL_GetKeyboardState(nullptr);
            const auto key = std::get<SDL_Scancode>(input.data);
            if(input.type != Input::Type::held or not pKeyboard[key])
                continue;
        }
        else
        {
            const auto button = std::get<SDL_GamepadButton>(input.data);
            if(not SDL_GetGamepadButton(m_pGamepad, button))
                continue;
        }

        auto [fst, snd] = m_commands.equal_range(action);
        for(auto& [k, command] : std::ranges::subrange(fst, snd))
        {
            command->Execute();
        }
    }
}

void dae::InputManager::UnbindAction(const ActionID& name, int playerIndex)
{
    m_commands.erase(Action{name, playerIndex});
}
