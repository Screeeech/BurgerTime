//
// Created by lily-laptop on 02/03/2026.
//

#ifndef ENGINE_ROTATORCOMPONENT_H
#define ENGINE_ROTATORCOMPONENT_H
#include "Component.h"

namespace dae
{

class RotatorComponent : public Component
{
public:
    RotatorComponent(GameObject* pOwner, float rotationSpeed);

    void Update(float deltaTime) override;
    ~RotatorComponent() override = default;

private:
    float m_rotationSpeed;
    float m_angle{};
};

}  // namespace dae

#endif  // ENGINE_ROTATORCOMPONENT_H
