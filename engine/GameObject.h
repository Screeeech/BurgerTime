#pragma once
#include <memory>
#include <string>

#include "Transform.h"

namespace dae
{
class Texture2D;

class GameObject final
{
    Transform m_transform{};
    std::shared_ptr<Texture2D> m_texture{};

public:
    void Update(float deltaTime);
    void Render() const;

    void SetTexture(const std::string& filename);
    void SetPosition(float x, float y);

    GameObject() = default;
    virtual ~GameObject();
    GameObject(const GameObject& other) = delete;
    GameObject(GameObject&& other) = delete;
    GameObject& operator=(const GameObject& other) = delete;
    GameObject& operator=(GameObject&& other) = delete;
};
}  // namespace dae
