#include "GameObject.h"

#include <string>

#include "Renderer.h"
#include "ResourceManager.h"

dae::GameObject::GameObject(std::vector<std::shared_ptr<Component>> components)
    : m_Components(std::move(components))
{
}

dae::GameObject::~GameObject() = default;

void dae::GameObject::Update(float deltaTime)
{
    for (auto& component : m_Components)
    {
        component->Update(deltaTime);
    }
}

void dae::GameObject::Render() const
{
    for (const auto& component : m_Components)
    {
        component->Render();
    }
}