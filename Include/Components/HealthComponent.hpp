#ifndef BURGERTIME_HEALTHCOMPONENT_H
#define BURGERTIME_HEALTHCOMPONENT_H
#include "Component.hpp"

namespace gla
{
class TextComponent;
struct Event;
}  // namespace gla

namespace bt
{

class HealthComponent : public gla::Component
{
public:
    explicit HealthComponent(gla::GameObject* pOwner, int playerIndex, int startingHealth = 5);
    ~HealthComponent() noexcept override;
    void Update(float deltaTime) override;

    void OnHealthChange(const gla::Event& event);
    void SetHealth(int newHealth);
    [[nodiscard]] int GetHealth() const;

private:
    int m_health;
    int m_playerIndex;
    gla::TextComponent* m_pHealthDisplay;
};

}  // namespace bt

#endif  // BURGERTIME_HEALTHCOMPONENT_H
