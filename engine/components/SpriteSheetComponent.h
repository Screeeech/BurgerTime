#ifndef ENGINE_SPRITESHEET_H
#define ENGINE_SPRITESHEET_H
#include <memory>

#include "Component.h"
#include "Texture2D.h"

namespace dae
{

struct SpriteSheet
{
    int rows;
    int cols;
};

class SpriteSheetComponent : public Component
{
public:
    explicit SpriteSheetComponent(GameObject* pOwner, std::shared_ptr<Texture2D> texture);
    ~SpriteSheetComponent() noexcept override;

    void Update(float deltaTime) override;
};

}  // namespace dae

#endif  // ENGINE_SPRITESHEET_H
