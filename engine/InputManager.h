#pragma once
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_scancode.h>

#include <memory>
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
    SDL_Scancode key{};
    enum class Type : uint8_t
    {
        released,
        pressed,
        held
    } type{};

    explicit Input(SDL_Scancode _key, Type _type = Type::released)
        : key{ _key }
        , type{ _type }
    {
    }

    // Is this maybe a little overkill?
    // Who knows, but it's fun :D
    bool operator==(const Input& other) const
    {
        return key == other.key and type == other.type;
    }

    struct Hash
    {
        size_t operator()(const Input& input) const noexcept
        {
            const size_t h1{ std::hash<SDL_Scancode>{}(input.key) };
            const size_t h2{ std::hash<uint8_t>{}(static_cast<uint8_t>(input.type)) };

            // Combine the 2 hashes, maybe I should find a better way of doing this
            return h1 ^ (h2 << 1);
        }
    };
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

    bool ProcessInput();
    bool ProcessKeyBoardInput(const SDL_Event& pEvent);
    void ProcessKeyBoardState();

    void RegisterInput(const Input& input, const Action& action);

    template<typename CommandType, typename... Args>
        requires std::derived_from<CommandType, Command>
    void BindAction(const std::string& name, int playerIndex, Args... args)
    {
        m_commands.emplace(Action{ .name = name, .playerIndex = playerIndex }, std::make_unique<CommandType>(args...));
    }

private:
    std::variant<SDL_Scancode, SDL_GamepadButton> m_Input;
    std::unordered_multimap<Input, Action, Input::Hash> m_registeredInputs;
    std::unordered_multimap<Action, std::unique_ptr<Command>, Action::Hash> m_commands;
};

}  // namespace dae
