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

private:
    friend class Singleton<SceneManager>;
    SceneManager() = default;
    std::vector<std::unique_ptr<Scene>> m_scenes{};
};
}  // namespace dae
