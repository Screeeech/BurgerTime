#include "Components/ScoreComponent.hpp"

#include <format>

#include "Components/TextComponent.hpp"
#include "EventManager.hpp"
#include "Events.hpp"
#include "GameObject.hpp"
#include "ResourceManager.hpp"
#include "Utils.hpp"

namespace bt
{
ScoreComponent::ScoreComponent(gla::GameObject* pOwner, int playerIndex, int initialScore)
    : Component(pOwner)
    , m_score(initialScore)
    , m_playerIndex(playerIndex)
    , m_pScoreDisplay(pOwner->AddComponent<gla::TextComponent>(std::format("Score: {}", initialScore),
                                                          gla::ResourceManager::Get().LoadFont("Lingua.otf", 21)))
{
    gla::EventManager::Get().BindEvent("scoreChange"_h, this, &ScoreComponent::OnScoreChange);
}

ScoreComponent::~ScoreComponent() noexcept
{
    gla::EventManager::Get().UnbindEvents(this);
}

void ScoreComponent::Update(float) {}

void ScoreComponent::OnScoreChange(const gla::Event& event)
{
    const auto* scoreEvent{ dynamic_cast<const gla::ScoreEvent*>(&event) };
    if(not scoreEvent or scoreEvent->playerIndex != m_playerIndex)
        return;

    m_score += scoreEvent->scoreChange;
    m_pScoreDisplay->SetText(std::format("Score: {}", m_score));

    if(m_score >= 500)
        gla::EventManager::Get().InvokeEvent("win"_h);
}

void ScoreComponent::SetScore(int score)
{
    m_score = score;
}

int ScoreComponent::GetScore() const
{
    return m_score;
}

}  // namespace bt