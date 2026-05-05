#ifndef BURGERTIME_STATEMACHINE_HPP
#define BURGERTIME_STATEMACHINE_HPP

#include <variant>

namespace bt
{

// Fold expression to check if a pack contains a type
template<typename T, typename... Pack>
inline constexpr bool IsInPack = (std::is_same_v<T, Pack> or ...);

// Recursive type trait to make sure all states are unique
template<typename...>
struct PackIsUnique : std::true_type
{
};
template<typename T, typename... Rest>
struct PackIsUnique<T, Rest...> : std::bool_constant<(not std::is_same_v<T, Rest> && ...) && PackIsUnique<Rest...>::value>
{
};

// Not sure if this is_trivially_copyable will stay but for now I think it makes sense
// And we need to make sure that the state has an Update() function
template<typename State, typename StateMachine>
concept IsValidState = requires(State state, StateMachine& machine) {
    { state.Update(machine) } -> std::same_as<void>;
};

template<typename T>
concept HasOnEnter = requires(T state) {
    { state.OnEnter() } -> std::same_as<void>;
};

template<typename T>
concept HasOnExit = requires(T state) {
    { state.OnExit() } -> std::same_as<void>;
};

template<typename... States>
class StateMachine final
{
    static_assert((IsValidState<States, StateMachine> && ...), "Every state should have a valid Update function");
    static_assert(PackIsUnique<States...>::value, "Duplicate states are not allowed");

public:
    template<typename State>
        requires IsValidState<State, StateMachine>
    explicit StateMachine(State initialState)
        : m_currentState(initialState)
    {
    }
    ~StateMachine() noexcept = default;

    StateMachine(StateMachine const&) = delete;
    auto operator=(StateMachine const&) -> StateMachine& = delete;
    StateMachine(StateMachine&&) = delete;
    auto operator=(StateMachine&&) -> StateMachine& = delete;

    void Update()
    {
        std::visit([this](auto& state) -> void { state.Update(*this); }, m_currentState);
    }

    template<typename NewState>
    void TransitionTo()
    {
        // Find a way to model all valid states at compile time
        if constexpr (HasOnEnter<NewState>)
            std::visit([&](auto& state) -> void { state.OnExit(); }, m_currentState);

        m_currentState.template emplace<NewState>();

        if constexpr (HasOnExit<NewState>)
            std::visit([&](auto& state) -> void { state.OnEnter(); }, m_currentState);
    }

private:
    std::variant<States...> m_currentState;
};

}  // namespace bt

#endif  // BURGERTIME_STATEMACHINE_HPP
