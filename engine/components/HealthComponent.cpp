//
// Created by lily-laptop on 17/03/2026.
//

#include "HealthComponent.h"

namespace dae
{

HealthComponent::HealthComponent(GameObject* pOwner, int startingHealth)
    : Component(pOwner)
    , m_health(startingHealth)
{
}

void HealthComponent::Update(float) {}

bool HealthComponent::ChangeHealth(int change)
{
    m_health += change;
    return m_health <= 0;
}

void HealthComponent::SetHealth(int newHealth)
{
    m_health = newHealth;
}

int HealthComponent::GetHealth() const
{
    return m_health;
}

}  // namespace dae