#ifndef BURGERTIME_INITIALSINPUT_HPP
#define BURGERTIME_INITIALSINPUT_HPP
#include <memory>

#include "Component.hpp"

namespace gla
{
class Font;
class TextComponent;
}  // namespace gla
namespace bt
{
class LeaderBoard;

class InitialsInput final : public gla::Component
{
public:
    explicit InitialsInput(
        gla::GameObject* pOwner, gla::TextComponent* first, gla::TextComponent* last, gla::GameObject* indicator, LeaderBoard* leaderBoard);

protected:
    void OnActivate() override;
    void OnDeactivate() override;

private:
    gla::TextComponent* m_pInfoText;
    gla::TextComponent* m_pFirstText;
    gla::TextComponent* m_pLastText;
    gla::GameObject* m_pIndicator;
    LeaderBoard* m_pLeaderBoard;

    char m_first{ 'A' };
    char m_last{ 'B' };
    bool m_firstSelected{ true };

    void CycleInitial(int count);
    void SelectInitial();
    void UpdateText() const;
    void SaveScoreAndRestart() const;
};

}  // namespace bt

#endif  // BURGERTIME_INITIALSINPUT_HPP
