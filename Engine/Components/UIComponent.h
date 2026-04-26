#ifndef ENGINE_UICOMPONENT_H
#define ENGINE_UICOMPONENT_H
#include <functional>

#include "Component.h"

namespace dae
{


class UIComponent : public Component
{
public:
    UIComponent(GameObject* pOwner, std::function<void(GameObject* pCaller)> drawFunc);
    ~UIComponent() noexcept override;

    void Update(float deltaTime) override;
    void DrawUI() const;

private:
    std::function<void(GameObject* pCaller)> m_drawFunc;
};


}

#endif  // ENGINE_UICOMPONENT_H
