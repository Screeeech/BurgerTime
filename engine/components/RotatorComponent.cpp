#include "RotatorComponent.h"

#include <glm/ext/scalar_constants.hpp>
#include <print>

#include "GameObject.h"

namespace dae
{
RotatorComponent::RotatorComponent(GameObject* pOwner, float rotationSpeed)
    : Component(pOwner)
    , m_rotationSpeed(rotationSpeed)
{
}

void RotatorComponent::Update(float deltaTime)
{
    const auto localPos{ m_pOwner->GetTransform().GetLocalPosition() };
    const float radius{ glm::length(localPos) };

    m_angle += m_rotationSpeed * deltaTime;

    // Clamp angle to avoid overflow
    while(m_angle < 0.f)
        m_angle += 2 * glm::pi<float>();
    while(m_angle > 2 * glm::pi<float>())
        m_angle -= 2 * glm::pi<float>();

    const auto x = glm::cos(m_angle) * radius;
    const auto y = glm::sin(m_angle) * radius;

    m_pOwner->GetTransform().SetLocalPosition(x, y);
}
}  // namespace dae