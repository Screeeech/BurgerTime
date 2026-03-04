#pragma once
#include <memory>
#include <vector>

#include "Component.h"
#include "SceneManager.h"

namespace dae
{
class RenderComponent;
class Texture2D;

class GameObject final
{
public:
    void Update(float deltaTime) const;

    explicit GameObject(float x, float y, float z = 0.0f, std::string  name = "new GameObject");
    ~GameObject() = default;
    GameObject(GameObject&& other) = delete;
    GameObject(const GameObject& other) = delete;
    GameObject& operator=(GameObject&& other) = delete;
    GameObject& operator=(const GameObject& other) = delete;

    template<ComponentConcept T, typename... Args>
    T* AddComponent(Args&&... args) noexcept
    {
        // NOTE: I still need to decide whether I want to have max one type of component
        // Or maybe I should be able to have multiple types of components and refer to them with a name or ID
        m_components.push_back(std::make_unique<T>(this, std::forward<Args>(args)...));
        return dynamic_cast<T*>(m_components.back().get());
    }

    template<ComponentConcept T>
    void RemoveComponent() noexcept
    {
        for (auto& component : m_components)
            if (auto* pComponent = dynamic_cast<T*>(component.get()))
                m_components.erase(pComponent);
    }

    template<ComponentConcept T>
    T* GetComponent()
    {
        for (auto& component : m_components)
            if (auto* pComponent = dynamic_cast<T*>(component.get()))
                return pComponent;
        return nullptr;
    }

    Transform& GetTransform();
    glm::mat4 GetParentWorldMatrix();
    glm::vec3 GetWorldPosition();
    std::string GetName();

    bool IsChild(GameObject* pChild);
    void SetParent(GameObject* pParent, bool keepWorldPosition = true);
    void AddChild(GameObject* game_object);
    GameObject* RemoveChild(GameObject* pParent);

    // Recursive function
    void SetDirty();

private:
    // For testing purposes
    std::string m_name;

    GameObject* m_pParent{};
    // NOTE: Should the parent own the children?
    std::vector<std::unique_ptr<GameObject>> m_children;

    std::vector<std::unique_ptr<Component>> m_components;
    Transform m_transform;
};
}  // namespace dae
