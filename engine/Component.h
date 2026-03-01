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
    Component(Component&&) = delete;
    Component(Component const&) = delete;
    Component& operator=(Component&&) = delete;
    Component& operator=(Component const&) = delete;

    virtual void Update(float deltaTime) = 0;

protected:
    explicit Component(GameObject* pOwner)
        : m_pOwner(pOwner) {}
private:
    GameObject* m_pOwner;
};

template<typename ComponentType>
concept ComponentConcept = std::derived_from<ComponentType, Component>;

}  // namespace dae
#endif  // MINIGIN_COMPONENT_H
