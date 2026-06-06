#ifndef BURGERTIME_PLATE_HPP
#define BURGERTIME_PLATE_HPP
#include "Renderable.hpp"

namespace gla
{
class CollisionRect;
}
namespace bt
{

class Plate final : public gla::Renderable
{
public:
    explicit Plate(gla::GameObject* pOwner, int maxStackSize, int plateIndex);

protected:
    void Render() override;

private:
    int const m_maxStackSize;
    int const m_plateIndex;
    int m_pieceCount{};
    gla::CollisionRect* m_pHitBox;

};

}  // namespace bt

#endif  // BURGERTIME_PLATE_HPP
