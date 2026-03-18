//
// Created by lily-laptop on 18/03/2026.
//

#ifndef ENGINE_SCORECOMPONENT_H
#define ENGINE_SCORECOMPONENT_H
#include "Component.h"

namespace dae
{

class ScoreComponent : public Component
{
public:
    explicit ScoreComponent(GameObject* pOwner, int initialScore = 0);
    ~ScoreComponent() noexcept override = default;

    void Update(float deltaTime) override;
    void ChangeScore(int change);
    void SetScore(int score);
    [[nodiscard]] int GetScore() const;

private:
    int m_score;
};

}  // namespace dae

#endif  // ENGINE_SCORECOMPONENT_H
