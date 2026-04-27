#ifndef BURGERTIME_SCORECOMPONENT_H
#define BURGERTIME_SCORECOMPONENT_H
#include "Component.hpp"
#include "ResourceManager.hpp"

namespace gla
{
class TextComponent;
struct Event;
}  // namespace gla

namespace bt
{

class ScoreComponent : public gla::Component
{
public:
    explicit ScoreComponent(gla::GameObject* pOwner, int playerIndex, int initialScore = 0);
    ~ScoreComponent() noexcept override;

    ScoreComponent(ScoreComponent const&) = delete;
    auto operator=(ScoreComponent const&) -> ScoreComponent & = delete;
    ScoreComponent(ScoreComponent&&) = delete;
    auto operator=(ScoreComponent&&) -> ScoreComponent & = delete;

    void Update(float deltaTime) override;
    void OnScoreChange(const gla::Event& event);
    void SetScore(int score);

    [[nodiscard]] auto GetScore() const -> int { return m_score; }

private:
    int m_score;
    int m_playerIndex;
    gla::TextComponent* m_pScoreDisplay;
};

}  // namespace bt

#endif  // BURGERTIME_SCORECOMPONENT_H
