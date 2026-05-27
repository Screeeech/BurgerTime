#ifndef BURGERTIME_PEPPER_HPP
#define BURGERTIME_PEPPER_HPP
#include "Renderable.hpp"

namespace bt
{

class Pepper : public gla::Renderable
{
public:
    explicit Pepper(gla::GameObject* pOwner, int zIndex = 0);
    ~Pepper() noexcept override = default;

protected:
    void Render() override;
    void Update(float) override;
    void OnActivate() override;
    void OnDeactivate() override;

private:
    //bool m_active{};
};

}  // namespace bt

#endif  // BURGERTIME_PEPPER_HPP
