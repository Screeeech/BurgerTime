//
// Created by lily-laptop on 11/03/2026.
//

#ifndef ENGINE_PLAYERCONTROLLER_H
#define ENGINE_PLAYERCONTROLLER_H

#include <glm/vec3.hpp>

#include "Component.h"

namespace dae
{

class PlayerController : public Component
{
public:
    explicit PlayerController(GameObject* pPlayer, int playerIndex);
    ~PlayerController() override;

    void Update(float deltaTime) override;
    void SetDirection(glm::vec3 direction);

private:
    glm::vec3 m_direction{};
    float m_speed{ 100.f };
};

}  // namespace dae

#endif  // ENGINE_PLAYERCONTROLLER_H
