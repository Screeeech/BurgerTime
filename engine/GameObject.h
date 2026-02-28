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
    void Update(float deltaTime);

    explicit GameObject() = default;
    ~GameObject() = default;
    GameObject(GameObject&& other) = delete;
    GameObject(const GameObject& other) = delete;
    GameObject& operator=(GameObject&& other) = delete;
    GameObject& operator=(const GameObject& other) = delete;

    template<ComponentConcept T, typename... Args>
    void AddComponent(Args&&... args) noexcept
    {
        m_components.push_back(std::make_unique<T>(this, std::forward<Args>(args)...));
    }
private:
    std::vector<std::unique_ptr<Component>> m_components;
};
}  // namespace dae
