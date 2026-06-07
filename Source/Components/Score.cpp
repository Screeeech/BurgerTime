#include "Components/Score.hpp"

#include <format>

#include "Utils.hpp"
#include "Components/TextComponent.hpp"
#include "Constants.hpp"
#include "GameEvents.hpp"
#include "GameObject.hpp"
#include "Locator.hpp"
#include "Services/EventManager.hpp"

namespace gla
{
class EventManager;
}
namespace bt
{

Score::Score(gla::GameObject* pOwner, std::shared_ptr<gla::Font> font, int startingScore)
    : Component(pOwner)
    , m_score(startingScore)
    , m_pText(pOwner->AddComponent<gla::TextComponent>(std::to_string(m_score), std::move(font), layers::text, gla::TextComponent::Align::Right))
{
}

void Score::OnScoreIncrease(std::any const& scoreEvent)
{
    auto const& args = std::any_cast<ScoreEvent const&>(scoreEvent);
    m_score += args.score;
    m_pText->SetText(std::to_string(m_score));
}

auto Score::GetScore() const -> int
{
    return m_score;
}

void Score::OnActivate()
{
    gla::Locator::Get<gla::EventManager>().BindEvent("ScoreChange"_h, this, &Score::OnScoreIncrease);
}

void Score::OnDeactivate()
{
    gla::Locator::Get<gla::EventManager>().UnbindEvent("ScoreChange"_h, this);
}

}  // namespace bt