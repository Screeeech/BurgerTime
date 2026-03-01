#include "GameObject.h"

#include "ResourceManager.h"

dae::GameObject::GameObject(const Transform& transform)
    : m_transform(transform)
{
}

dae::Transform dae::GameObject::GetTransform() const
{
    return m_transform;
}

glm::vec3 dae::GameObject::GetPosition() const
{
    return m_transform.GetPosition();
}

void dae::GameObject::Update(float deltaTime) const
{
    for (const auto& component : m_components)
    {
        component->Update(deltaTime);
    }
}
