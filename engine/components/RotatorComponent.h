#ifndef ENGINE_ROTATORCOMPONENT_H
#define ENGINE_ROTATORCOMPONENT_H
#include "Component.h"

namespace dae
{

class RotatorComponent : public Component
{
public:
    RotatorComponent(GameObject* pOwner, float rotationSpeed);
    ~RotatorComponent() override = default;

    void Update(float deltaTime) override;

private:
    float m_rotationSpeed;
    float m_angle{};
};

}  // namespace dae

#endif  // ENGINE_ROTATORCOMPONENT_H
