//
// Created by lily-laptop on 02/03/2026.
//

#include "RotatorComponent.h"

#include "GameObject.h"

namespace dae
{
RotatorComponent::RotatorComponent(GameObject* pOwner, float rotationSpeed)
    : Component(pOwner)
    , m_rotationSpeed(rotationSpeed)
{
}

void RotatorComponent::Update(float /*deltaTime*/)
{

}
}  // namespace dae