//
// Created by lily-laptop on 18/03/2026.
//

#include <format>
#include <sdbm.hpp>
using namespace sdbm;

#include "GameObject.h"
#include "ResourceManager.h"

#include "ScoreComponent.h"
#include "TextComponent.h"

#include "EventManager.h"
#include "Events.h"

namespace dae
{
ScoreComponent::ScoreComponent(GameObject* pOwner, int playerIndex, int initialScore)
    : Component(pOwner)
    , m_score(initialScore)
    , m_playerIndex(playerIndex)
    , m_pScoreDisplay(pOwner->AddComponent<TextComponent>(std::format("Score: {}", initialScore),
                                                           ResourceManager::Get().LoadFont("Lingua.otf", 21.f)))
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