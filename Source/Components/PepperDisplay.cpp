#include "Components/PepperDisplay.hpp"

#include "Components/Sprite.hpp"
#include "Components/TextComponent.hpp"
#include "Components/Timer.hpp"
#include "Constants.hpp"
#include "GameEvents.hpp"
#include "GameObject.hpp"
#include "Locator.hpp"
#include "Services/EventManager.hpp"
#include "Services/ResourceManager.hpp"
#include "Services/Sound.hpp"
#include "Utils.hpp"

namespace bt
{


PepperDisplay::PepperDisplay(gla::GameObject* pOwner, int startingPeppers)
    : Component(pOwner)
    , m_pepperCount(startingPeppers)
    , m_pPepperSprite(pOwner->AddComponent<gla::Sprite>())
    , m_pPepperText(pOwner->AddComponent<gla::TextComponent>(
          std::to_string(startingPeppers), gla::Locator::Get<gla::ResourceManager>().LoadFont("Fonts/nes.ttf", 8)))
    , m_pBonusAppearTimer(pOwner->AddComponent<gla::Timer>())
{
}

void PepperDisplay::OnTryPepper(std::any const& pepperEvent) const
{
    if (m_pepperCount <= 0)
        return;

    auto args = std::any_cast<PepperEvent>(pepperEvent);
    args.eventID = "Pepper"_h;

    gla::Locator::Get<gla::EventManager>().InvokeEvent(args);
}

void PepperDisplay::OnPepperAttack(std::any const& /*pepperEvent*/)
{
    m_pepperCount--;
    m_pPepperText->SetText(std::to_string(m_pepperCount));
}

void PepperDisplay::OnBonusPickup(std::any const& /*eventArgs*/)
{
    m_bonusAvailable = false;
    m_pepperCount++;
    m_pPepperText->SetText(std::to_string(m_pepperCount));
    gla::Locator::Get<gla::Sound>().PlayAudio("bonus_obtained"_h);
}

void PepperDisplay::OnDisable(std::any const& /*eventArgs*/) const
{
    m_pBonusAppearTimer->Reset();
}

auto PepperDisplay::GetPepperCount() const -> int
{
    return m_pepperCount;
}

void PepperDisplay::OnActivate()
{
    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.BindEvent("TryPepper"_h, this, &PepperDisplay::OnTryPepper);
    eventManager.BindEvent("PepperAttack"_h, this, &PepperDisplay::OnPepperAttack);
    eventManager.BindEvent("BonusPickup"_h, this, &PepperDisplay::OnBonusPickup);
    eventManager.BindEvent("DisableEntities"_h, this, &PepperDisplay::OnDisable);

    m_pPepperSprite->SetTexture(gla::Locator::Get<gla::ResourceManager>().LoadTexture("Textures/spritesheet.png"));
    m_pPepperSprite->SetSourceRect({ .x = 232, .y = 0, .w = 8, .h = 8 });
    m_pPepperSprite->m_offset.x = -16.f;

    m_pPepperText->SetText(std::to_string(m_pepperCount));
}

void PepperDisplay::OnDeactivate()
{
    gla::Locator::Get<gla::EventManager>().UnbindEvents(this);
}

void PepperDisplay::Update()
{
    if (m_pepperCount <= game::startingPepper / 2 and not m_bonusAvailable)
    {
        m_bonusAvailable = true;
        m_pBonusAppearTimer->Start(
            game::bonusAppearTime,
            [] -> void
            {
                gla::Locator::Get<gla::Sound>().PlayAudio("bonus_appear"_h);
                gla::Locator::Get<gla::EventManager>().QueueEvent(gla::Event{ "BonusAppear"_h });
            });
    }
}
}  // namespace bt