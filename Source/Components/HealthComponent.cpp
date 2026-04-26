#include "Components/HealthComponent.hpp"

#include <format>

#include "Components/TextComponent.hpp"
#include "EventManager.hpp"
#include "Events.hpp"
#include "GameObject.hpp"
#include "ResourceManager.hpp"
#include "Utils.hpp"

namespace dae
{

HealthComponent::HealthComponent(GameObject* pOwner, int playerIndex, int startingHealth)
    : Component(pOwner)
    , m_health(startingHealth)
    , m_playerIndex(playerIndex)
    , m_pHealthDisplay(pOwner->AddComponent<TextComponent>(std::format("Health: {}", startingHealth),
                                                           ResourceManager::Get().LoadFont("Lingua.otf", 21)))
{
    EventManager::Get().BindEvent("healthChange"_h, this, &HealthComponent::OnHealthChange);
}

HealthComponent::~HealthComponent() noexcept
{
    EventManager::Get().UnbindEvents(this);
}

void HealthComponent::Update(float) {}

void HealthComponent::OnHealthChange(const Event& event)
{
    const auto* healthEvent{ dynamic_cast<const HealthEvent*>(&event) };
    if(not healthEvent or healthEvent->playerIndex != m_playerIndex)
        return;

    m_health += healthEvent->healthChange;
    m_pHealthDisplay->SetText(std::format("Health: {}", m_health));

    if(m_health <= 0)
        EventManager::Get().QueueEvent(PlayerEvent{ "die"_h, m_playerIndex });
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