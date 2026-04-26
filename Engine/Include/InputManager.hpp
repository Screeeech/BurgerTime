#pragma once
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_scancode.h>

#include <memory>
#include <optional>
#include <print>
#include <ranges>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "Command.hpp"
#include "Singleton.hpp"

namespace dae
{

using ActionID = unsigned int;

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
    ActionID name;
    int playerIndex{};

    bool operator==(const Action& other) const
    {
        return name == other.name and playerIndex == other.playerIndex;
    }

    struct Hash
    {
        size_t operator()(const Action& action) const noexcept
        {
            return std::hash<ActionID>{}(action.name);
        }
    };
};

class InputManager final : public Singleton<InputManager>
{
public:
    InputManager() = default;
    ~InputManager() noexcept override;

    void Init();
    bool ProcessInput();
    void ProcessInputHeld();

    template<InputConcept T>
    void RegisterInput(T inputData, Input::Type inputType, const ActionID& name, int playerIndex)
    {
        if(m_registeredInputs.contains(Action{ name, playerIndex }))
        {
            if constexpr(std::same_as<T, SDL_Scancode>)
                std::println("Warning! \tAttempting to bind Input: \'{}\' to Action: ({}, {}) which is already bound!",
                             SDL_GetScancodeName(inputData), name, playerIndex);
            else if constexpr(std::same_as<T, SDL_GamepadButton>)
                std::println("Warning! \tAttempting to bind Input: \'{}\' to Action: ({}, {}) which is already bound!",
                             SDL_GetGamepadStringForButton(inputData), name, playerIndex);
        }

        m_registeredInputs.insert(std::pair{ Action{ name, playerIndex }, Input{ inputData, inputType } });
    }

    template<InputConcept T>
    void UnregisterInput(T inputData, const ActionID& name, int playerIndex)
    {
        std::erase_if(m_registeredInputs,
                      [inputData, name, playerIndex](const std::pair<Action, Input>& input)
                      {
                          return input.first.name == name and input.first.playerIndex == playerIndex and
                              input.second.InputDataMatches(inputData);
                      });
    }

    template<typename CommandType, typename... Args>
        requires std::derived_from<CommandType, Command>
    void BindAction(const ActionID& name, int playerIndex, Args... args)
    {
        m_commands.emplace(Action{ .name = name, .playerIndex = playerIndex }, std::make_unique<CommandType>(args...));
    }

    void UnbindAction(const ActionID& name, int playerIndex);

private:
    template<InputConcept T>
    bool HandleInputEvent(Input::Type inputType, T inputData)
    {
        auto actionFilter = [&](const auto& pair)
        {
            const auto& [action, registeredInput] = pair;
            return registeredInput.type != inputType and registeredInput.InputDataMatches(inputData);
        };

        auto getCommands = [this](const Action& action)
        {
            auto range = m_commands.equal_range(action);
            return std::ranges::subrange(range.first, range.second) | std::views::values;
        };

        // clang-format off
        auto allCommands = m_registeredInputs
            | std::views::filter(actionFilter)
            | std::views::keys
            | std::views::transform(getCommands)
            | std::views::join;

        bool executed{};
        for(auto& command : allCommands)
        {
            command->Execute();
            executed = true;
        }

        return executed;
    }

    std::unordered_multimap<Action, Input, Action::Hash> m_registeredInputs;
    std::unordered_multimap<Action, std::unique_ptr<Command>, Action::Hash> m_commands;

    SDL_Gamepad* m_pGamepad{};
};

}  // namespace dae
