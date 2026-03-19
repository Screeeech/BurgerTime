#pragma once
#include <memory>
#include <vector>

#include "components/RenderComponent.h"
#include "GameObject.h"

namespace dae
{
class Scene final
{
public:
    void Add(std::unique_ptr<GameObject>&& object);
    void Remove(GameObject* pGameObject);
    void RemoveAll();

    void Update(float deltaTime);
    void Render();
    void DrawUI();
    void Load();
    void RegisterRenderComponent(RenderComponent* renderComponent);
    void UnregisterRenderComponent(RenderComponent* component);
    void RegisterUIComponent(UIComponent* component);
    void UnregisterUIComponent(UIComponent* component);

    ~Scene() = default;
    Scene(const Scene& other) = delete;
    Scene(Scene&& other) = delete;
    Scene& operator=(const Scene& other) = delete;
    Scene& operator=(Scene&& other) = delete;

private:
    friend class SceneManager;
    explicit Scene() = default;

    std::vector<std::unique_ptr<GameObject>> m_objects;
    std::vector<RenderComponent*> m_renderComponents;
    std::vector<UIComponent*> m_uiComponents;
};

}  // namespace dae
