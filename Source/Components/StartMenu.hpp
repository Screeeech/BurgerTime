#ifndef BURGERTIME_STARTMENU_HPP
#define BURGERTIME_STARTMENU_HPP
#include "Component.hpp"

namespace bt
{
class GameState;
enum class GameMode : std::uint8_t;

class StartMenu final : public gla::Component
{
public:
    explicit StartMenu(gla::GameObject* pOwner, gla::GameObject* indicatorObject, GameState* gameState);

protected:
    void OnActivate() override;
    void OnDeactivate() override;

    void OnSelect();
    void OnGameStart() const;

private:
    GameMode m_selectedMode{};
    gla::GameObject* m_pIndicatorObject;
    GameState* m_pGameState;
};

}  // namespace bt

#endif  // BURGERTIME_STARTMENU_HPP
