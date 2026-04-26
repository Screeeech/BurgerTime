//
// Created by lily-laptop on 18/03/2026.
//

#include "ScoreComponent.hpp"

#include <format>
#include <Utils.hpp>

#include "EventManager.hpp"
#include "Events.hpp"
#include "GameObject.hpp"
#include "ResourceManager.hpp"
#include "TextComponent.hpp"

namespace dae
{
ScoreComponent::ScoreComponent(GameObject* pOwner, int playerIndex, int initialScore)
    : Component(pOwner)
    , m_score(initialScore)
    , m_playerIndex(playerIndex)
    , m_pScoreDisplay(pOwner->AddComponent<TextComponent>(std::format("Score: {}", initialScore),
                                                           ResourceManager::Get().LoadFont("Lingua.otf", 21)))
{
    EventManager::Get().BindEvent("scoreChange"_h, this, &ScoreComponent::OnScoreChange);
}

ScoreComponent::~ScoreComponent() noexcept
{
    EventManager::Get().UnbindEvents(this);
}

void ScoreComponent::Update(float) {}

void ScoreComponent::OnScoreChange(const Event& event)
{
    const auto* scoreEvent{ dynamic_cast<const ScoreEvent*>(&event) };
    if(not scoreEvent or scoreEvent->playerIndex != m_playerIndex)
        return;

    m_score += scoreEvent->scoreChange;
    m_pScoreDisplay->SetText(std::format("Score: {}", m_score));

    if(m_score >= 500)
        EventManager::Get().InvokeEvent("win"_h);

}

void ScoreComponent::SetScore(int score)
{
    m_score = score;
}

int ScoreComponent::GetScore() const
{
    return m_score;
}

}  // namespace dae