#include "GameObject.h"

#include <algorithm>
#include <ranges>
#include <utility>

#include "ResourceManager.h"

dae::GameObject::GameObject(float x, float y, float z, std::string  name)
    : m_name(std::move(name))
    , m_transform(x, y, z, this)
{
}

dae::Transform& dae::GameObject::GetTransform()
{
    return m_transform;
}

glm::mat4 dae::GameObject::GetParentWorldMatrix()
{
    if(not m_pParent)
        return { 1.0f };

    return m_pParent->m_transform.GetWorldMatrix();
}

glm::vec3 dae::GameObject::GetWorldPosition()
{
    return GetTransform().GetWorldPosition();
}

std::string dae::GameObject::GetName()
{
    return m_name;
}

bool dae::GameObject::IsChild(GameObject* pChild)
{
    if(not pChild or pChild == this)
        return false;

    const auto it = std::ranges::find(m_children, pChild, [](const auto& child) { return child.get(); });
    return it != m_children.end();
}

void dae::GameObject::SetParent(GameObject* pParent, bool keepWorldPosition)
{
    // Is the parent valid?
    if(IsChild(pParent) or pParent == this or pParent == m_pParent)
        return;

    // Is parent root?
    if(not pParent->m_pParent)  // Then set local pos to world pos
    {
        m_transform.SetLocalPosition(m_transform.GetWorldPosition());
    }
    else  // If it isn't root, then check if we need to keep the world position
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

void dae::GameObject::AddChild(GameObject* pChild)
{
    if(not pChild or pChild == this or IsChild(pChild))
        return;

    // Is this a good way of moving ownership?
    m_children.emplace_back(std::unique_ptr<GameObject>(pChild));
}

dae::GameObject* dae::GameObject::RemoveChild(GameObject* pParent)
{
    // We have to move ownership of the child pointer from the parent
    // and return it to instantiate a new unique_ptr
    // I think this is a good approach?
    auto it = std::ranges::find_if(m_children, [pParent](const auto& child) { return child.get() == pParent; });

    GameObject* pChild{};
    if(it != m_children.end())
    {
        // I'm not sure if this is a good idea, but we ball lol
        // Releasing here "deactivates" the unique_ptr so erasing it doesn't free the GameObject
        pChild = it->release();

        // Thus we can safely remove it from the list while keeping the pointer and returning it
        m_children.erase(it);
    }

    return pChild;
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

    for(const auto& children : m_children)
    {
        children->Update(deltaTime);
    }
}
