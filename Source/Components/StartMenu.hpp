#ifndef BURGERTIME_STARTMENU_HPP
#define BURGERTIME_STARTMENU_HPP
#include "Component.hpp"

namespace bt
{
enum class GameMode : std::uint8_t;

class StartMenu final : public gla::Component
{
public:
    explicit StartMenu(gla::GameObject* pOwner, gla::GameObject* indicatorObject);
    ~StartMenu() override;

protected:
    void OnActivate() override;

    void OnSelect();
    void OnGameStart() const;

private:
    GameMode m_selectedMode{};
    gla::GameObject* m_pIndicatorObject;
};

}  // namespace bt

#endif  // BURGERTIME_STARTMENU_HPP
