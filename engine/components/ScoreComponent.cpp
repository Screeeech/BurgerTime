//
// Created by lily-laptop on 18/03/2026.
//

#include "ScoreComponent.h"

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