#ifndef BURGERTIME_PLAYERCONTROLLER_H
#define BURGERTIME_PLAYERCONTROLLER_H

#include <glm/vec3.hpp>

#include "Component.hpp"
#include "StateMachine.hpp"
#include "States/PlayerStates.hpp"

namespace gla
{
struct Event;
}

namespace bt
{
class Pepper;

class PlayerController final : public gla::Renderable
{
public:
    explicit PlayerController(gla::GameObject* pPlayer, Stage* stage, Pepper* pepper, int playerIndex);

    void SetDirection(glm::vec2 direction);
    [[nodiscard]] glm::vec2 GetDirection() const;

    void Move(glm::vec2 displacement) const;
    void OnDeath(std::any const& eventArgs) const;
    // void OnPepper(std::any const& eventArgs) const;

    int const m_playerIndex;

protected:
    void Render() override;
    void FixedUpdate(float deltaTime) override;

    void OnActivate() override;
    void OnDeactivate() override;

private:
    static constexpr glm::vec2 spriteFeetOffset{ 8.f, 15.f };

    playerstates::PlayerStateMachine m_finiteStateMachine;

    Stage* m_pStage;
    Pepper* m_pPepper;

    glm::vec2 m_direction{};
    // static float constexpr m_speed{ 1.f };
};

}  // namespace bt

#endif  // BURGERTIME_PLAYERCONTROLLER_H
