//
// Created by lily-laptop on 18/02/2026.
//

#ifndef ENGINE_SPRITECOMPONENT_H
#define ENGINE_SPRITECOMPONENT_H
#include <memory>

#include "Component.h"
#include "Transform.h"

namespace dae
{
class Texture2D;
}

class SpriteComponent : public Component
{
public:
    SpriteComponent(std::shared_ptr<dae::Texture2D> texture, const dae::Transform& transform);

    void Update(float deltaTime) override;
    void Render() const override;

    void SetTexture(std::shared_ptr<dae::Texture2D> texture);

private:
    dae::Transform m_Transform;
    std::shared_ptr<dae::Texture2D> m_Texture;
};


#endif  // ENGINE_SPRITECOMPONENT_H
