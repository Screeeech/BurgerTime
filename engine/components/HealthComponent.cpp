#include "HealthComponent.h"

#include <sdbm.hpp>

#include "EventManager.h"
#include "Events.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include "TextComponent.h"
using namespace sdbm;

namespace dae
{

HealthComponent::HealthComponent(GameObject* pOwner, int playerIndex, int startingHealth)
    : Component(pOwner)
    , m_health(startingHealth)
    , m_playerIndex(playerIndex)
    , m_pTextComponent(pOwner->AddComponent<TextComponent>(std::format("Health: {}", startingHealth),
                                                           ResourceManager::Get().LoadFont("Lingua.otf", 36.f)))
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
    m_pTextComponent->SetText(std::format("Health: {}", m_health));

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