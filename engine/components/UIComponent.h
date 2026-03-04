//
// Created by lily-laptop on 04/03/2026.
//

#ifndef ENGINE_UICOMPONENT_H
#define ENGINE_UICOMPONENT_H
#include <functional>

#include "Component.h"

namespace dae
{

class UIComponent : public Component
{
public:
    UIComponent(GameObject* pOwner, std::function<void()> drawFunc);
    ~UIComponent() override;

    void Update(float deltaTime) override;
    void DrawUI() const;

private:
    std::function<void()> m_drawFunc;
};


}
#endif  // ENGINE_UICOMPONENT_H
