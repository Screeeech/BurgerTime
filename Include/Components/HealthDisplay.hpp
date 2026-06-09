#ifndef BURGERTIME_HEALTHDISPLAY_HPP
#define BURGERTIME_HEALTHDISPLAY_HPP
#include <any>

#include "Component.hpp"

namespace gla
{
class TextComponent;
class Sprite;
}
namespace bt
{

class HealthDisplay final : public gla::Component
{
public:
    explicit HealthDisplay(gla::GameObject* pOwner, int startingHealth);

    void OnHealthChange(std::any const& healthEvent);
protected:
    void OnActivate() override;

private:
    int const m_startingHealth;
    int m_healthPoints;

    gla::Sprite* m_pHealthSprite;
    gla::TextComponent* m_pHealthText;
};

}  // namespace bt

#endif  // BURGERTIME_HEALTHDISPLAY_HPP
