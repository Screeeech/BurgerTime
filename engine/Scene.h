#pragma once
#include <memory>
#include <vector>

#include "GameObject.h"

namespace dae
{
class UIComponent;
class Renderable;

class Scene final
{
public:
    void RemoveGameObject(GameObject* pObject);
    void Update(float deltaTime);
    void Render();
    void DrawUI();
    void Load();
    void RegisterRenderComponent(Renderable* renderable);
    void UnregisterRenderComponent(Renderable* component);
    void RegisterUIComponent(UIComponent* component);
    void UnregisterUIComponent(UIComponent* component);
    void SortCachedRenderComponents();

    ~Scene() = default;
    Scene(const Scene& other) = delete;
    Scene(Scene&& other) = delete;
    Scene& operator=(const Scene& other) = delete;
    Scene& operator=(Scene&& other) = delete;

    [[nodiscard]] GameObject* GetRoot();
private:
    friend class SceneManager;
    explicit Scene();

    std::unique_ptr<GameObject> m_pRootObject;
    std::vector<Renderable*> m_renderComponents;
    std::vector<UIComponent*> m_uiComponents;
};

}  // namespace dae
