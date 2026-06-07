#ifndef BURGERTIME_HIGHSCORE_HPP
#define BURGERTIME_HIGHSCORE_HPP
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

class HighScore final : public gla::Component
{
public:
    explicit HighScore(gla::GameObject* pOwner, std::shared_ptr<gla::Font> font, std::string playerName, int startingHighScore = 0);

    void OnHighScoreSet(std::any const& scoreEvent);

    [[nodiscard]] auto GetHighScore() const -> int;
private:
    int m_highScore;
    std::string m_playerName;
    gla::TextComponent* m_pText;
};

}  // namespace bt

#endif  // BURGERTIME_HIGHSCORE_HPP
