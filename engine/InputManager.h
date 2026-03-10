#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Command.h"
#include "Singleton.h"

// I decided to word vomit all of my thought processes out here, so if you're bored and looking for a headache, here ya go

// Input handling is split across 3 different types
// Input -> The button that's pressed
// Action -> The game action that has to happen and can be bound to an input (ex. "Jump")
// Command -> The function(s) that is/are called when a certain Action is fired

// Input:
// Will be handled by the corresponding input library, but will just contain an id for the key/input
// and then perhaps an enum to determine whether the input is held, pressed or released

// Action:
// This will probably just be a basic string or some other identifier
// All this will be is some way to bind inputs to commands
// So that when an input is pressed that is bound to the action: "playerJump",
// it will then look for all the commands with that action as it's trigger.

// Command:
// An object-oriented version of a callback that's really just going to execute a function.

// PIPELINE:
// 1.) Input pressed
// 2.) InputManager gets scancode
// 3.) InputManager checks which scancode corresponds to which PlayerIndex
// 4.) InputManager checks which scancode corresponds to which Action
// 5.) InputManager checks which Action corresponds to which Command(s)
// 6.) Execute Command(s)

// One consideration is if an Action object is even necessary, because I can just give a Command a "name" and then use that
// to bind it to an input.

// Another question is whether a component should be responsible for listening to input and doing stuff,
// or if the Command should just have a lambda that calls the right function, maybe all of this should be done with Events?

// How would we differentiate inputs for different players?
// Would I hold an index/id for each player, probably
// Now where would I store that index, probably in the Action, then I can register actions for each player
// So what would mean an Action would be:
// - Triggering Input(s) (in case of multiple bindings being allows)
// - Player Index
// Ok but now how would I do Actions for UI and other non player specific things
// Maybe specific indices can be tied to specific receivers
// Or maybe Player1 just does all the UI input, I think I will go with this

// What would binding an Action to a Command look like then?
// I would probably store a (multi)map of Action -> Command

// How would I handle PolyMorphic types though?
// Maybe I should just hold a pointer to the Command instead, yeah that seems good

namespace dae
{
struct Input
{
    SDL_Scancode button{};
    enum class InputType : uint8_t
    {
        pressed,
        held,
        released
    } type{};

    // Is this maybe a little overkill?
    // Who knows, but it's fun :D
    struct Hash
    {
        size_t operator()(const Input& input) const noexcept
        {
            const size_t h1{ std::hash<SDL_Scancode>{}(input.button) };
            const size_t h2{ std::hash<uint8_t>{}(static_cast<uint8_t>(input.type)) };

            // Combine the 2 hashes, maybe I should find a better way of doing this
            return h1 ^ (h2 << 1);
        }
    };
};

struct Action
{
    std::string name;
    std::vector<Input> inputs;
    int playerIndex{};

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
    bool ProcessInput();

    template<typename T>
        requires std::derived_from<T, Command>
    void BindAction()
    {
    }

private:
    std::unordered_map<Input, Action, Input::Hash> m_actions;
    std::unordered_multimap<Action, std::unique_ptr<Command>, Input::Hash> m_commands;
};

}  // namespace dae
