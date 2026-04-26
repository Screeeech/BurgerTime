#ifndef ENGINE_HEALTHCOMPONENT_H
#define ENGINE_HEALTHCOMPONENT_H
#include "Component.h"

namespace dae
{
class TextComponent;
struct Event;

class HealthComponent : public Component
{
public:
    explicit HealthComponent(GameObject* pOwner, int playerIndex, int startingHealth = 5);
    ~HealthComponent() noexcept override;
    void Update(float deltaTime) override;

    void OnHealthChange(const Event& event);
    void SetHealth(int newHealth);
    [[nodiscard]] int GetHealth() const;
private:
    int m_health;
    int m_playerIndex;
    TextComponent* m_pHealthDisplay;
};
}; // namespace dae

#endif  // ENGINE_HEALTHCOMPONENT_H
