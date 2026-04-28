#include "Components/HealthComponent.hpp"

#include <format>

#include "Components/TextComponent.hpp"
#include "EventManager.hpp"
#include "Events.hpp"
#include "GameObject.hpp"
#include "ResourceManager.hpp"
#include "Utils.hpp"

namespace bt
{

HealthComponent::HealthComponent(gla::GameObject* pOwner, int playerIndex, int startingHealth)
    : Component(pOwner)
    , m_health(startingHealth)
    , m_playerIndex(playerIndex)
    , m_pHealthDisplay(pOwner->AddComponent<gla::TextComponent>(std::format("Health: {}", startingHealth),
                                                                gla::ResourceManager::Get().LoadFont("Fonts/nes.ttf", 8)))
{
    gla::EventManager::Get().BindEvent("healthChange"_h, this, &HealthComponent::OnHealthChange);
}

HealthComponent::~HealthComponent() noexcept
{
    gla::EventManager::Get().UnbindEvents(this);
}

void HealthComponent::Update(float) {}

void HealthComponent::OnHealthChange(const gla::Event& event)
{
    const auto* healthEvent{ dynamic_cast<const gla::HealthEvent*>(&event) };
    if(not healthEvent or healthEvent->playerIndex != m_playerIndex)
        return;

    m_health += healthEvent->healthChange;
    m_pHealthDisplay->SetText(std::format("Health: {}", m_health));

    if(m_health <= 0)
        gla::EventManager::Get().QueueEvent(gla::PlayerEvent{ "die"_h, m_playerIndex });
}

void HealthComponent::SetHealth(int newHealth)
{
    m_health = newHealth;
}

auto HealthComponent::GetHealth() const -> int
{
    return m_health;
}

}  // namespace bt