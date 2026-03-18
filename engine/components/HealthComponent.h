#ifndef ENGINE_HEALTHCOMPONENT_H
#define ENGINE_HEALTHCOMPONENT_H
#include "Component.h"

namespace dae
{

class HealthComponent : public Component
{
public:
    explicit HealthComponent(GameObject* pOwner, int startingHealth = 5);
    ~HealthComponent() noexcept override = default;
    void Update(float deltaTime) override;

    bool ChangeHealth(int change);
    void SetHealth(int newHealth);
    [[nodiscard]] int GetHealth() const;
private:
    int m_health;

};
}; // namespace dae

#endif  // ENGINE_HEALTHCOMPONENT_H
