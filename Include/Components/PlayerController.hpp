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

class PlayerController : public gla::Renderable
{
public:
    explicit PlayerController(gla::GameObject* pPlayer, Stage* stage, int playerIndex);
    ~PlayerController() noexcept override;

    PlayerController(PlayerController const&) = delete;
    auto operator=(PlayerController const&) -> PlayerController& = delete;
    PlayerController(PlayerController&&) = delete;
    auto operator=(PlayerController&&) -> PlayerController& = delete;

    void Update(float deltaTime) override;
    void SetDirection(glm::vec3 direction);

    void Move(glm::vec3 displacement) const;
    void OnDeath(const gla::Event& event) const;
    void Render() override;

private:
    playerstates::PlayerStateMachine m_finiteStateMachine;

    int m_playerIndex;
    Stage* m_stage;

    glm::vec3 m_direction{};
    static float constexpr m_speed{ 30.f };
};

}  // namespace bt

#endif  // BURGERTIME_PLAYERCONTROLLER_H
