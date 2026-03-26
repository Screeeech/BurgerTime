#pragma once
#include <memory>
#include <vector>

#include "Scene.h"
#include "Singleton.h"

namespace dae
{
class UIComponent;
class Scene;
class Renderable;

class SceneManager final : public Singleton<SceneManager>
{
public:
    Scene& CreateScene();

    void Update(float deltaTime);
    void Render() const;
    void DrawUI() const;

    void LoadScene(Scene* scene);
    [[nodiscard]] Scene* GetActiveScene() const;
    void RegisterRenderComponent(Renderable* component) const;
    void UnregisterRenderComponent(Renderable* component) const;
    void RegisterUIComponent(UIComponent* component) const;
    void UnregisterUIComponent(UIComponent* component) const;

    void Cleanup();

    void SortCachedRenderComponents();

private:
    friend class Singleton<SceneManager>;
    std::vector<std::unique_ptr<Scene>> m_scenes;
    Scene* m_currentScene{};
};
}  // namespace dae
