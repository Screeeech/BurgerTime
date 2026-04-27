#ifndef BURGERTIME_PLAYERCONTROLLER_H
#define BURGERTIME_PLAYERCONTROLLER_H

#include <glm/vec3.hpp>

#include "Component.hpp"
#include "HealthComponent.hpp"

namespace bt
{

class PlayerController : public gla::Component
{
public:
    explicit PlayerController(gla::GameObject* pPlayer, int playerIndex);
    ~PlayerController() noexcept override;

    void Update(float deltaTime) override;
    void SetDirection(glm::vec3 direction);

    void OnDeath(const gla::Event& event);

private:
    int m_playerIndex;

    glm::vec3 m_direction{};
    float m_speed{ 100.f };
};

}  // namespace bt

#endif  // BURGERTIME_PLAYERCONTROLLER_H
