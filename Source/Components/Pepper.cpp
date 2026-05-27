
#include "Components/Pepper.hpp"

namespace bt
{

Pepper::Pepper(gla::GameObject* pOwner, int zIndex)
    : Renderable(pOwner, zIndex)
{
}

void Pepper::Render() {}

void Pepper::Update(float x)
{
    Renderable::Update(x);
}

void Pepper::OnActivate()
{
    Renderable::OnActivate();
}

void Pepper::OnDeactivate()
{
    Renderable::OnDeactivate();
}

}  // namespace bt