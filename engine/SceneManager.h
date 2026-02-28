#pragma once
#include <memory>
#include <string>
#include <vector>

#include "Scene.h"
#include "Singleton.h"

namespace dae
{
class Scene;

class SceneManager final : public Singleton<SceneManager>
{
public:
    Scene& CreateScene();

    void Update(float deltaTime);
    void Render();

    void LoadScene(Scene* scene);
    void RegisterRenderComponent(RenderComponent* component) const;
    void UnregisterRenderComponent(RenderComponent* component) const;

private:
    friend class Singleton<SceneManager>;
    SceneManager() = default;
    std::vector<std::unique_ptr<Scene>> m_scenes;
    Scene* m_currentScene{};
};
}  // namespace dae
