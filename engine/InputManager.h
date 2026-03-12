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

    [[nodiscard]] bool InputDataMatches(SDL_Scancode) const;
    [[nodiscard]] bool InputDataMatches(SDL_GamepadButton) const;

    [[nodiscard]] std::optional<SDL_Scancode> GetKey() const;
    [[nodiscard]] std::optional<SDL_GamepadButton> GetButton() const;
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

    bool ProcessInput();
    bool ProcessKeyBoardInput(const SDL_Event& event);
    bool ProcessGamepadInput(const SDL_Event& sdl_event, InputManager* im);
    void ProcessKeyBoardState();
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
    bool CheckInputPressed(Input::Type inputType, SDL_Scancode key);

    std::unordered_multimap<Action, Input, Action::Hash> m_registeredInputs;
    std::unordered_multimap<Action, std::unique_ptr<Command>, Action::Hash> m_commands;

    class Impl;
    std::unique_ptr<Impl> m_pImpl;
};

}  // namespace dae
