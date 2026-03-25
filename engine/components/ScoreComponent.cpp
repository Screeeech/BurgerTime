//
// Created by lily-laptop on 18/03/2026.
//

#include <sdbm.hpp>
using namespace sdbm;

#include "ScoreComponent.h"

#include "EventManager.h"
#include "Events.h"

namespace dae
{
ScoreComponent::ScoreComponent(GameObject* pOwner, int initialScore)
    : Component(pOwner)
    , m_score(initialScore)
{
}

void ScoreComponent::Update(float) {}

void ScoreComponent::ChangeScore(int change)
{
    m_score += change;

    if(m_score >= 500)
        EventManager::Get().InvokeEvent("win"_h);

    // Future logic related to reaching score thresholds could go here
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