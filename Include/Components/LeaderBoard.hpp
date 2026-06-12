
#ifndef BURGERTIME_LEADERBOARD_HPP
#define BURGERTIME_LEADERBOARD_HPP
#include <map>
#include <memory>

#include "Component.hpp"

namespace gla
{
class Font;
class TextComponent;
}  // namespace gla


namespace bt
{
struct Initials
{
    char first;
    char last;

    //auto operator==(const Initials& other) const -> bool { return first == other.first && last == other.last; }
};
};  // namespace bt


//template<>
//struct std::hash<bt::Initials>
//{
//    auto operator()(bt::Initials const& init) const noexcept -> std::size_t
//    {
//        return (static_cast<std::size_t>(init.first) << 8) | static_cast<std::size_t>(init.last);
//    }
//};


namespace bt
{
class LeaderBoard final : public gla::Component
{
public:
    explicit LeaderBoard(gla::GameObject* pOwner, std::shared_ptr<gla::Font> const& font, int newScoreEntry);

    void SaveHighScoreData();

protected:
    void OnActivate() override;
    void OnDeactivate() override;

private:
    void LoadHighScoreData();
    void SetLeaderBoardText();

    int m_newScore;
    std::multimap<int, Initials> m_highScores;
    Initials m_currentInitials{ .first = 'P', .last = '1' };

    gla::TextComponent* m_pHeader;
    gla::TextComponent* m_pRank1Text;
    gla::TextComponent* m_pRank2Text;
    gla::TextComponent* m_pRank3Text;
};

}  // namespace bt

#endif  // BURGERTIME_LEADERBOARD_HPP
