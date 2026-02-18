#pragma once
#include <memory>
#include <string>

#include "Component.h"
#include "Transform.h"

namespace dae
{
class Texture2D;

class GameObject final
{
public:
    void Update(float deltaTime);
    void Render() const;

    GameObject(std::vector<std::shared_ptr<Component>>);
    ~GameObject();
    GameObject(const GameObject& other) = delete;
    GameObject(GameObject&& other) = delete;
    GameObject& operator=(const GameObject& other) = delete;
    GameObject& operator=(GameObject&& other) = delete;

private:
    std::vector<std::shared_ptr<Component>> m_Components;
};
}  // namespace dae
