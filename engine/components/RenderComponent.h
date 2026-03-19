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
    explicit RenderComponent(GameObject* pOwner, std::shared_ptr<Texture2D> texture, int zIndex = 0);
    explicit RenderComponent(GameObject* pOwner, int zIndex = 0);
    ~RenderComponent() noexcept override;

    void Update(float deltaTime) override;
    void Render() const;

    void SetTexture(std::shared_ptr<Texture2D> texture);

    void SetZIndex(int zIndex);
    [[nodiscard]] int GetZIndex() const;
private:
    std::shared_ptr<Texture2D> m_Texture;
    int m_zIndex;
};

}  // namespace dae

#endif  // ENGINE_SPRITECOMPONENT_H
