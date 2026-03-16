#ifndef ENGINE_PLAYERCONTROLLER_H
#define ENGINE_PLAYERCONTROLLER_H

#include <glm/vec3.hpp>

#include "Component.h"

namespace dae
{
struct Event;

class PlayerController : public Component
{
public:
    explicit PlayerController(GameObject* pPlayer, int playerIndex);
    ~PlayerController() noexcept override = default;

    void Update(float deltaTime) override;
    void SetDirection(glm::vec3 direction);

    void Test(const Event& event);
private:

    glm::vec3 m_direction{};
    float m_speed{ 100.f };
};

}  // namespace dae

#endif  // ENGINE_PLAYERCONTROLLER_H
