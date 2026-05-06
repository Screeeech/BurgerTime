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

template<typename State, typename StateMachine, typename Context>
concept IsValidState = requires(State state, StateMachine& machine, Context&& context) {
    { state.Update(machine, std::forward<Context>(context)) } -> std::same_as<void>;
};

template<typename T>
concept HasOnEnter = requires(T state) {
    { state.OnEnter() } -> std::same_as<void>;
};

template<typename T>
concept HasOnExit = requires(T state) {
    { state.OnExit() } -> std::same_as<void>;
};

template<typename InitialState, typename Context, typename... States>
class StateMachine final
{
    static_assert((IsValidState<States, StateMachine, Context> && ...), "Every state should have a valid Update function");
    static_assert(PackIsUnique<States...>::value, "Duplicate states are not allowed");
    static_assert(IsInPack<InitialState, States...>, "Initial state must be a listed possible state");

public:
    explicit StateMachine()
        : m_currentState(InitialState{})
    {
        CallOnEnter();
    }
    ~StateMachine() noexcept = default;

    StateMachine(StateMachine const&) = delete;
    auto operator=(StateMachine const&) -> StateMachine& = delete;
    StateMachine(StateMachine&&) = delete;
    auto operator=(StateMachine&&) -> StateMachine& = delete;

    template<typename... Args>
    void Update(Args&&... args)
    {
        std::visit([&](auto& state) -> void { state.Update(*this, std::forward<Args>(args)...); }, m_currentState);
    }

    template<typename NewState>
    void TransitionTo()
    {
        // Find a way to model all valid states at compile time

        CallOnExit();
        m_currentState.template emplace<NewState>();
        CallOnEnter();
    }

private:
    std::variant<States...> m_currentState;

    // I could use this tuple in the future if I needed my states to be stored instead of just constructed
    // For now my states don't need to hold any state (haha) so this is not necessary
    // std::tuple<States...> m_allStates;

    void CallOnEnter()
    {
        std::visit(
            [&]<typename T>(T& state) -> void
            {
                if constexpr (HasOnEnter<std::decay_t<T>>)
                    state.OnEnter();
            },
            m_currentState);
    }

    void CallOnExit()
    {
        std::visit(
            [&]<typename T>(T& state) -> void
            {
                if constexpr (HasOnExit<std::decay_t<T>>)
                    state.OnExit();
            },
            m_currentState);
    }
};

}  // namespace bt

#endif  // BURGERTIME_STATEMACHINE_HPP
