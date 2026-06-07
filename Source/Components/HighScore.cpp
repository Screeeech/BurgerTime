#include "Components/HighScore.hpp"

#include <format>
#include <memory>
#include <utility>

#include "Components/TextComponent.hpp"
#include "Constants.hpp"
#include "GameEvents.hpp"
#include "GameObject.hpp"

namespace gla
{
class Font;
}
namespace bt
{

HighScore::HighScore(gla::GameObject* pOwner, std::shared_ptr<gla::Font> font, std::string playerName, int startingHighScore)
    : Component(pOwner)
    , m_highScore(startingHighScore)
    , m_playerName(std::move(playerName))
    , m_pText(pOwner->AddComponent<gla::TextComponent>(std::format("HI  {}", startingHighScore), std::move(font), layers::text))
{
}

void HighScore::OnHighScoreSet(std::any const& scoreEvent)
{
    auto const& args = std::any_cast<ScoreEvent const&>(scoreEvent);
    m_highScore = args.score;
    m_pText->SetText(std::format("HI  {}", m_highScore));
}

auto HighScore::GetHighScore() const -> int
{
    return m_highScore;
}

}  // namespace bt