#ifndef BURGERTIME_SCORECOMPONENT_HPP
#define BURGERTIME_SCORECOMPONENT_HPP
#include <any>
#include <memory>

#include "Component.hpp"

namespace gla
{
class Font;
class TextComponent;
}
namespace bt
{

class Score final : public gla::Component
{
public:
    explicit Score(gla::GameObject* pOwner, std::shared_ptr<gla::Font> font, int startingScore = 0);

    void OnScoreIncrease(std::any const& scoreEvent);

    [[nodiscard]] auto GetScore() const -> int;

protected:
    void OnActivate() override;
    void OnDeactivate() override;

private:
    int m_score;
    gla::TextComponent* m_pText;
};

}  // namespace bt

#endif  // BURGERTIME_SCORECOMPONENT_HPP
