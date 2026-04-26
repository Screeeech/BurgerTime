#ifndef ENGINE_PLAYERCONTROLLER_H
#define ENGINE_PLAYERCONTROLLER_H

#include <glm/vec3.hpp>

#include "Component.h"
#include "HealthComponent.h"

namespace dae
{
class ScoreComponent;
class TextComponent;
struct Event;
class Font;

class PlayerController : public Component
{
public:
    explicit PlayerController(GameObject* pPlayer, int playerIndex);
    ~PlayerController() noexcept override;

    void Update(float deltaTime) override;
    void SetDirection(glm::vec3 direction);

    void OnDeath(const Event& event);
private:
    int m_playerIndex;

    glm::vec3 m_direction{};
    float m_speed{ 100.f };
};

}  // namespace dae

#endif  // ENGINE_PLAYERCONTROLLER_H
