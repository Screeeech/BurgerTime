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
    explicit RenderComponent(GameObject* pOwner, std::shared_ptr<Texture2D> texture);
    explicit RenderComponent(GameObject* pOwner);
    ~RenderComponent() override;
    void Update(float deltaTime) override;
    void Render() const;

    void SetTexture(std::shared_ptr<Texture2D> texture);

private:
    std::shared_ptr<Texture2D> m_Texture;
};

}  // namespace dae

#endif  // ENGINE_SPRITECOMPONENT_H
