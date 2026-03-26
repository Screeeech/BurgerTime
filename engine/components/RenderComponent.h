#ifndef ENGINE_SPRITECOMPONENT_H
#define ENGINE_SPRITECOMPONENT_H
#include <SDL3/SDL_rect.h>

#include <memory>

#include "Renderable.h"

namespace dae
{
class Texture2D;
class GameObject;

class RenderComponent : public Renderable
{
public:
    explicit RenderComponent(GameObject* pOwner, std::shared_ptr<Texture2D> texture, int zIndex = 0);
    explicit RenderComponent(GameObject* pOwner, int zIndex = 0);
    ~RenderComponent() noexcept override;

    void Update(float deltaTime) override;
    void Render() override;

    void SetTexture(std::shared_ptr<Texture2D> texture);
    void SetSourceRect(SDL_FRect sourceRect);
    void SetSourceRect(float x, float y, float w, float h);
    void SetSourceRectPos(float x, float y);
    void SetSourceRectSize(float w, float h);

    [[nodiscard]] SDL_FRect GetSourceRect() const;

    bool m_Visible{ true };
private:
    std::shared_ptr<Texture2D> m_texture;
    SDL_FRect m_sourceRect;
};

}  // namespace dae

#endif  // ENGINE_SPRITECOMPONENT_H
