//
// Created by lily-laptop on 18/03/2026.
//

#ifndef ENGINE_SCORECOMPONENT_H
#define ENGINE_SCORECOMPONENT_H
#include "Component.h"

namespace dae
{
struct Event;
class TextComponent;

class ScoreComponent : public Component
{
public:
    explicit ScoreComponent(GameObject* pOwner, int playerIndex, int initialScore = 0);
    ~ScoreComponent() noexcept override;

    void Update(float deltaTime) override;
    void OnScoreChange(const Event& event);
    void SetScore(int score);
    [[nodiscard]] int GetScore() const;

private:
    int m_score;
    int m_playerIndex;
    TextComponent* m_pScoreDisplay;
};

}  // namespace dae

#endif  // ENGINE_SCORECOMPONENT_H
