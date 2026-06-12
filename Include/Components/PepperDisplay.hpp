#ifndef BURGERTIME_PEPPERDISPLAY_HPP
#define BURGERTIME_PEPPERDISPLAY_HPP
#include <any>

#include "Component.hpp"

namespace gla
{
class Timer;
class Sprite;
class TextComponent;
}  // namespace gla
namespace bt
{

class PepperDisplay final : public gla::Component
{
public:
    explicit PepperDisplay(gla::GameObject* pOwner, int startingPeppers);

    [[nodiscard]] auto GetPepperCount() const -> int;

protected:
    void OnActivate() override;
    void OnDeactivate() override;
    void Update() override;

private:
    void OnTryPepper(std::any const& pepperEvent) const;
    void OnPepperAttack(std::any const& pepperEvent);
    void OnBonusPickup(std::any const& eventArgs);
    void OnDisable(std::any const& eventArgs) const;

    int m_pepperCount;
    gla::Sprite* m_pPepperSprite;
    gla::TextComponent* m_pPepperText;
    gla::Timer* m_pBonusAppearTimer;
    bool m_bonusAvailable{};
};

}  // namespace bt

#endif  // BURGERTIME_PEPPERDISPLAY_HPP
