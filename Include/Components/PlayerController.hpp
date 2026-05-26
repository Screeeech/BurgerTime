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

class PlayerController final : public gla::Renderable
{
public:
    explicit PlayerController(gla::GameObject* pPlayer, Stage* stage, int playerIndex);

    void SetDirection(glm::vec2 direction);

    void Move(glm::vec2 displacement) const;
    void Walk(float xDisplacement) const;
    void Climb(float yDisplacement) const;
    void OnDeath(const gla::Event& event) const;

protected:
    void Render() override;
    void FixedUpdate(float deltaTime) override;

    void OnActivate() override;
    void OnDeactivate() override;

private:
    playerstates::PlayerStateMachine m_finiteStateMachine;

    int m_playerIndex;
    Stage* m_stage;

    glm::vec2 m_direction{};
    //static float constexpr m_speed{ 1.f };
};

}  // namespace bt

#endif  // BURGERTIME_PLAYERCONTROLLER_H
