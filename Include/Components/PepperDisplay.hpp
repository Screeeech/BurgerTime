#ifndef BURGERTIME_PEPPERDISPLAY_HPP
#define BURGERTIME_PEPPERDISPLAY_HPP
#include <any>

#include "Component.hpp"

namespace gla
{
class Sprite;
class TextComponent;
}
namespace bt
{

class PepperDisplay final : public gla::Component
{
public:
    explicit PepperDisplay(gla::GameObject* pOwner, int startingPeppers);

    void OnTryPepper(std::any const& pepperEvent);
    void OnPepperAttack(std::any const& pepperEvent);

    auto GetPepperCount() const -> int;

protected:
    void OnActivate() override;
    void OnDeactivate() override;

private:
    int const m_startingPepperCount;
    int m_pepperCount;
   gla::Sprite* m_pPepperSprite;
    gla::TextComponent* m_pPepperText;

};

}  // namespace bt

#endif  // BURGERTIME_PEPPERDISPLAY_HPP
