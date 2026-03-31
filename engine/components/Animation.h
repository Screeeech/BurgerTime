#ifndef ENGINE_ANIMATION_H
#define ENGINE_ANIMATION_H
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

class Animation : public Component
{
public:
    explicit Animation(GameObject* pOwner, std::shared_ptr<Texture2D> texture);
    ~Animation() noexcept override;

    void Update(float deltaTime) override;
};

}  // namespace dae

#endif  // ENGINE_ANIMATION_H
