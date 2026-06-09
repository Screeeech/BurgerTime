#include "Components/HealthDisplay.hpp"

#include "Components/Sprite.hpp"
#include "Components/TextComponent.hpp"
#include "GameEvents.hpp"
#include "GameObject.hpp"
#include "Services/EventManager.hpp"
#include "Services/ResourceManager.hpp"
#include "Utils.hpp"

namespace gla
{
class EventManager;
}
namespace bt
{

HealthDisplay::HealthDisplay(gla::GameObject* pOwner, int startingHealth)
    : Component(pOwner)
    , m_startingHealth(startingHealth)
    , m_healthPoints(startingHealth)
    , m_pHealthSprite(pOwner->AddComponent<gla::Sprite>())
    , m_pHealthText(pOwner->AddComponent<gla::TextComponent>(
          std::to_string(startingHealth), gla::Locator::Get<gla::ResourceManager>().LoadFont("Fonts/nes.ttf", 8)))
{
}

void HealthDisplay::OnHealthChange(std::any const& healthEvent)
{
    auto const& args = std::any_cast<HealthEvent const&>(healthEvent);

    m_healthPoints += args.healthChange;
    m_pHealthText->SetText(std::to_string(m_healthPoints));
}

void HealthDisplay::OnActivate()
{
    gla::Locator::Get<gla::EventManager>().BindEvent("HealthChange"_h, this, &HealthDisplay::OnHealthChange);
    m_healthPoints = m_startingHealth;

    m_pHealthSprite->SetTexture(gla::Locator::Get<gla::ResourceManager>().LoadTexture("Textures/spritesheet.png"));
    m_pHealthSprite->SetSourceRect({ .x = 200, .y = 0, .w = 8, .h = 8 });
    m_pHealthSprite->m_offset.x = -16.f;

    m_pHealthText->SetText(std::to_string(m_startingHealth));
}

void HealthDisplay::OnDeactivate()
{
    Component::OnDeactivate();
}

}  // namespace bt