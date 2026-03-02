#include "GameObject.h"

#include <algorithm>
#include <ranges>

#include "ResourceManager.h"

dae::GameObject::GameObject(const Transform& transform)
    : m_transform(transform)
{
}

dae::Transform dae::GameObject::GetTransform()
{
    if(m_transform.isDirty)
    {
        if(m_pParent)
            m_transform.UpdateWorldMatrix(m_pParent->GetTransform());
        else
            m_transform.UpdateWorldMatrix();
    }

    return m_transform;
}

glm::vec3 dae::GameObject::GetPosition()
{
    return GetTransform().GetPosition();
}

bool dae::GameObject::IsChild(GameObject* pChild)
{
    if(not pChild or pChild == this)
        return false;

    const auto it = std::ranges::find(m_children, pChild, [](const auto& child) { return child.get(); });
    return it == m_children.end();
}

void dae::GameObject::SetParent(GameObject* pParent, bool keepWorldPosition)
{
    // Is the parent valid?
    if(IsChild(pParent) or pParent == this or pParent == m_pParent)
        return;

    // Is parent root?
    if(not pParent) // Then set local pos to world pos
    {
        m_transform.SetLocalPosition(m_transform.GetWorldPosition());
    }
    else // If it isn't root, then check if we need to keep the world position
    {
        if(keepWorldPosition)
            m_transform.ApplyInverseTransform(pParent->GetTransform());
        else
            SetDirty();
    }

    // Remove the object itself from the current parent's list of children
    if(m_pParent)
        m_pParent->RemoveChild(this);

    // Set new parent
    m_pParent = pParent;

    // Add this as a new child to parent's list of children
    if(m_pParent)
        m_pParent->AddChild(this);
}

void dae::GameObject::RemoveChild(GameObject* pParent)
{
    std::erase_if(m_children, [pParent](const auto& child) { return child.get() == pParent; });
}

void dae::GameObject::AddChild(GameObject* pChild)
{
    if(not pChild or pChild == this or IsChild(pChild))
        return;

    // NOTE: Any way of moving ownership?
    m_children.emplace_back(std::unique_ptr<GameObject>(pChild));
}

void dae::GameObject::SetDirty()
{
    if(m_transform.isDirty)
        return;

    m_transform.isDirty = true;
    for(const auto& child : m_children)
        child->SetDirty();
}

void dae::GameObject::Update(float deltaTime) const
{
    for(const auto& component : m_components)
    {
        component->Update(deltaTime);
    }
}
