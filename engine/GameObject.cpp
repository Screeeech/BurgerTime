#include "GameObject.h"

#include "ResourceManager.h"

void dae::GameObject::Update(float deltaTime)
{
    for (auto& component : m_components)
    {
        component->Update(deltaTime);
    }
}