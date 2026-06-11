#ifndef BURGERTIME_STATEMACHINE_HPP
#define BURGERTIME_STATEMACHINE_HPP

#include <variant>

#include "Component.hpp"

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

template<typename State>
concept HasValidUpdateFunction = requires(State state) {
    { state.Update() } -> std::same_as<void>;
};

template<typename State, typename StateMachine>
concept HasStateMachinePointer = requires(State state) { requires std::same_as<decltype(state.machine), StateMachine*>; };

template<typename State, typename Context>
concept HasContextPointer = requires(State state) { requires std::same_as<decltype(state.ctx), Context*>; };

template<typename T>
concept HasOnEnter = requires(T state) {
    { state.OnEnter() } -> std::same_as<void>;
};

template<typename T>
concept HasOnExit = requires(T state) {
    { state.OnExit() } -> std::same_as<void>;
};

template<typename Context, typename... States>
class StateMachine final : public gla::Component
{
    static_assert((HasValidUpdateFunction<States> and ...), "States must have a valid Update function");
    static_assert((HasStateMachinePointer<States, StateMachine> and ...), "States must have a valid StateMachine pointer as a member");
    static_assert((HasContextPointer<States, Context> and ...), "States must have a valid Context pointer as a member");
    static_assert(PackIsUnique<States...>::value, "Duplicate states are not allowed");

public:
    explicit StateMachine(gla::GameObject* pOwner, Context const& context)
        : Component(pOwner)
        , m_context(context)
    {
        InitNewState<std::tuple_element_t<0, std::tuple<States...>>>();
    }
    ~StateMachine() noexcept override = default;

    template<typename NewState>
    void TransitionTo()
    {
        static_assert(IsInPack<NewState, States...> and "NewState must be a valid state of this State Machine");
        // Find a way to model all valid states at compile time

        CallOnExit();
        InitNewState<NewState>();
        CallOnEnter();
    }

    template<typename State>
    auto IsActive() -> bool
    {
        static_assert(IsInPack<State, States...> and "State must be a valid state of this State Machine");
        return std::holds_alternative<State>(m_currentState);
    }

    template<typename NewState>
    void InitNewState()
    {
        // What is this syntax C++??
        m_currentState.template emplace<NewState>();
        auto& newState = std::get<NewState>(m_currentState);
        newState.machine = this;
        newState.ctx = &m_context;
    }

protected:
    void FixedUpdate() override
    {
        std::visit([&](auto& state) -> void { state.Update(); }, m_currentState);
    }
    void OnActivate() override
    {
        CallOnEnter();
    }
    void OnDeactivate() override
    {
        CallOnExit();
    }

private:
    std::variant<States...> m_currentState;
    Context m_context;

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
    Context* ctx;

    virtual void Update() = 0;
};


}  // namespace bt

#endif  // BURGERTIME_STATEMACHINE_HPP
