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
class GameObject;

class RenderComponent : public Component
{
public:
    RenderComponent(GameObject* pOwner, std::shared_ptr<Texture2D> texture, const Transform& transform);
    void Update(float deltaTime) override;
    void Render() const override;

    void SetTexture(std::shared_ptr<Texture2D> texture);

protected:

private:
    Transform m_Transform;
    std::shared_ptr<Texture2D> m_Texture;
};

}  // namespace dae

#endif  // ENGINE_SPRITECOMPONENT_H
