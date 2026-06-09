#include "Components/PepperDisplay.hpp"

#include "Components/Sprite.hpp"
#include "Components/TextComponent.hpp"
#include "GameEvents.hpp"
#include "GameObject.hpp"
#include "Locator.hpp"
#include "Services/EventManager.hpp"
#include "Services/ResourceManager.hpp"
#include "Utils.hpp"

namespace bt
{


PepperDisplay::PepperDisplay(gla::GameObject* pOwner, int startingPeppers)
    : Component(pOwner)
    , m_startingPepperCount(startingPeppers)
    , m_pepperCount(startingPeppers)
    , m_pPepperSprite(pOwner->AddComponent<gla::Sprite>())
    , m_pPepperText(pOwner->AddComponent<gla::TextComponent>(
          std::to_string(startingPeppers), gla::Locator::Get<gla::ResourceManager>().LoadFont("Fonts/nes.ttf", 8)))
{
}

PepperDisplay::~PepperDisplay()
{
    gla::Locator::Get<gla::EventManager>().UnbindEvent("TryPepper"_h, this);
}

void PepperDisplay::OnTryPepper(std::any const& pepperEvent)
{
    if (m_pepperCount <= 0)
        return;

    m_pepperCount--;
    m_pPepperText->SetText(std::to_string(m_pepperCount));

    auto args = std::any_cast<PepperEvent>(pepperEvent);
    args.eventID = "Pepper"_h;

    gla::Locator::Get<gla::EventManager>().InvokeEvent(args);
}

auto PepperDisplay::GetPepperCount() const -> int
{
    return m_pepperCount;
}

void PepperDisplay::OnActivate()
{
    gla::Locator::Get<gla::EventManager>().BindEvent("TryPepper"_h, this, &PepperDisplay::OnTryPepper);
    m_pepperCount = m_startingPepperCount;

    m_pPepperSprite->SetTexture(gla::Locator::Get<gla::ResourceManager>().LoadTexture("Textures/spritesheet.png"));
    m_pPepperSprite->SetSourceRect({ .x = 232, .y = 0, .w = 8, .h = 8 });
    m_pPepperSprite->m_offset.x = -16.f;

    m_pPepperText->SetText(std::to_string(m_startingPepperCount));
}

}  // namespace bt