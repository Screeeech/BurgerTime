#ifndef BURGERTIME_STATEMACHINE_HPP
#define BURGERTIME_STATEMACHINE_HPP

#include <optional>
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
struct PackIsUnique<T, Rest...> : std::bool_constant<(not std::is_same_v<T, Rest> and ...) and PackIsUnique<Rest...>::value>
{
};

template<typename State, typename Context>
concept HasValidUpdateFunction = requires(State state, Context& context) {
    { state.Update(context) } -> std::same_as<void>;
};

template<typename State, typename StateMachine>
concept HasStateMachinePointer = requires(State state) { requires std::same_as<decltype(state.machine), StateMachine*>; };

template<typename T, typename Context>
concept HasOnEnter = requires(T state, Context context) {
    { state.OnEnter(context) } -> std::same_as<void>;
};

template<typename T, typename Context>
concept HasOnExit = requires(T state, Context context) {
    { state.OnExit(context) } -> std::same_as<void>;
};

template<typename Context, typename... States>
class StateMachine final
{
    static_assert((HasValidUpdateFunction<States, Context> and ...), "States must have a valid Update function");
    static_assert((HasStateMachinePointer<States, StateMachine> and ...), "States must have a valid StateMachine pointer as a member");
    static_assert(PackIsUnique<States...>::value, "Duplicate states are not allowed");

public:
    explicit StateMachine(Context const& context = {})
        : m_currentState(std::tuple_element_t<0, std::tuple<States...>>{})
    {
        std::get<std::tuple_element_t<0, std::tuple<States...>>>(m_currentState).machine = this;
        CallOnEnter(context);
    }
    ~StateMachine() noexcept = default;

    StateMachine(StateMachine const&) = delete;
    auto operator=(StateMachine const&) -> StateMachine& = delete;
    StateMachine(StateMachine&&) = delete;
    auto operator=(StateMachine&&) -> StateMachine& = delete;

    void Update(std::decay_t<Context> context)
    {
        std::visit([&](auto& state) -> void { state.Update(context); }, m_currentState);
    }

    template<typename NewState>
    void TransitionTo(Context context = {})
    {
        static_assert(IsInPack<NewState, States...> and "NewState must be a valid state of this State Machine");
        // Find a way to model all valid states at compile time

        CallOnExit(context);
        // What is this syntax C++??
        m_currentState.template emplace<NewState>();
        std::get<NewState>(m_currentState).machine = this;
        CallOnEnter(context);
    }

    template<typename State>
    auto IsActive() -> bool
    {
        static_assert(IsInPack<State, States...> and "State must be a valid state of this State Machine");
        return std::holds_alternative<State>(m_currentState);
    }

private:
    std::variant<States...> m_currentState;

    // I could use this tuple in the future if I needed my states to be stored instead of just constructed
    // For now my states don't need to hold any state (haha) so this is not necessary
    // std::tuple<States...> m_allStates;

    void CallOnEnter(Context context = {})
    {
        std::visit(
            [&]<typename T>(T& state) -> void
            {
                if constexpr (HasOnEnter<std::decay_t<T>, Context>)
                    state.OnEnter(context);
            },
            m_currentState);
    }

    void CallOnExit(Context context = {})
    {
        std::visit(
            [&]<typename T>(T& state) -> void
            {
                if constexpr (HasOnExit<std::decay_t<T>, Context>)
                    state.OnExit(context);
            },
            m_currentState);
    }
};

template<typename Context, typename StateMachine>
struct HelperState
{
    HelperState() = default;
    HelperState(HelperState const&) = default;
    HelperState(HelperState&&) = default;
    auto operator=(HelperState const&) -> HelperState& = default;
    auto operator=(HelperState&&) -> HelperState& = default;
    virtual ~HelperState() = default;

    StateMachine* machine;

    virtual void Update(Context const& ctx) = 0;
};


}  // namespace bt

#endif  // BURGERTIME_STATEMACHINE_HPP
