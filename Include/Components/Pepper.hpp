#ifndef BURGERTIME_PEPPER_HPP
#define BURGERTIME_PEPPER_HPP
#include "Renderable.hpp"

namespace gla
{
class Animation;
}
namespace bt
{

class Pepper : public gla::Renderable
{
public:
    explicit Pepper(gla::GameObject* pOwner, int zIndex = 0);
    ~Pepper() noexcept override = default;

    void SpawnPepper(glm::vec2 position, glm::vec2 direction);

protected:
    void Render() override;
    void FixedUpdate(float deltaTime) override;

private:
    float m_duration{};
    gla::Animation* m_pAnimation{};
};

}  // namespace bt

#endif  // BURGERTIME_PEPPER_HPP
