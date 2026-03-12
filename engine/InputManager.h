#pragma once
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_scancode.h>

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "Command.h"
#include "Singleton.h"

namespace dae
{

template<typename InputDataType>
concept InputConcept = std::same_as<InputDataType, SDL_Scancode> || std::same_as<InputDataType, SDL_GamepadButton>;

struct Input
{
    using InputData = std::variant<SDL_Scancode, SDL_GamepadButton>;
    InputData data;
    enum class Type : uint8_t
    {
        released,
        pressed,
        held
    } type{};

    explicit Input(SDL_Scancode _key, Type _type = Type::released)
        : data{ _key }
        , type{ _type }
    {
    }

    explicit Input(SDL_GamepadButton _key, Type _type = Type::released)
        : data{ _key }
        , type{ _type }
    {
    }

    template<InputConcept T>
    bool InputDataMatches(T otherData) const
    {
        if constexpr(std::same_as<T, SDL_Scancode>)
        {
            if(not std::holds_alternative<SDL_Scancode>(data))
                return false;

            const auto ourKey = std::get<SDL_Scancode>(data);
            return ourKey == otherData;
        }
        else
        {
            if(not std::holds_alternative<SDL_GamepadButton>(data))
                return false;

            const auto ourButton = std::get<SDL_GamepadButton>(data);
            return ourButton == otherData;
        }
    }
};

struct Action
{
    std::string name;
    int playerIndex{};

    bool operator==(const Action& other) const
    {
        return name == other.name and playerIndex == other.playerIndex;
    }

    struct Hash
    {
        size_t operator()(const Action& action) const noexcept
        {
            return std::hash<std::string>{}(action.name);
        }
    };
};

class InputManager final : public Singleton<InputManager>
{
public:
    InputManager();
    ~InputManager() override;

    void Init();
    bool ProcessInput();
    bool ProcessGamepadInput(const SDL_Event& sdl_event, InputManager* im);
    void ProcessInputHeld();
    void ProcessGamepadState(InputManager* im);

    template<typename T>
        requires(std::same_as<T, SDL_Scancode> || std::same_as<T, SDL_GamepadButton>)
    void RegisterInput(T inputData, Input::Type inputType, const std::string& name, int playerIndex)
    {
        m_registeredInputs.insert(std::pair{ Action{ name, playerIndex }, Input{ inputData, inputType } });
    }

    template<typename CommandType, typename... Args>
        requires std::derived_from<CommandType, Command>
    void BindAction(const std::string& name, int playerIndex, Args... args)
    {
        m_commands.emplace(Action{ .name = name, .playerIndex = playerIndex }, std::make_unique<CommandType>(args...));
    }

private:
    template<InputConcept T>
    bool HandleInputEvent(Input::Type inputType, T inputData)
    {
        for(auto& [action, registeredInput] : m_registeredInputs)
        {
            if(registeredInput.type != inputType)
                continue;

            if(not registeredInput.InputDataMatches(inputData))
                continue;

            bool executed{};
            auto range = m_commands.equal_range(action);
            for(auto& [k, command] : std::ranges::subrange(range.first, range.second))
            {
                command->Execute();
                executed = true;
            }
            return executed;
        }
        return false;
    }

    std::unordered_multimap<Action, Input, Action::Hash> m_registeredInputs;
    std::unordered_multimap<Action, std::unique_ptr<Command>, Action::Hash> m_commands;

    SDL_Gamepad* m_pGamepad{};
};

}  // namespace dae
