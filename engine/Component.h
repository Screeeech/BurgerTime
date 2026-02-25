//
// Created by lily-laptop on 18/02/2026.
//

#ifndef MINIGIN_COMPONENT_H
#define MINIGIN_COMPONENT_H

#include <concepts>

namespace dae
{
class GameObject;

class Component
{
public:
    virtual ~Component() = default;
    virtual void Update(float deltaTime) = 0;
    virtual void Render() const = 0;

protected:
    GameObject* m_pOwner;
    explicit Component(GameObject* pOwner)
        : m_pOwner(pOwner) {}
};

template<typename ComponentType>
concept ComponentConcept = std::derived_from<ComponentType, Component>;

}  // namespace dae
#endif  // MINIGIN_COMPONENT_H
