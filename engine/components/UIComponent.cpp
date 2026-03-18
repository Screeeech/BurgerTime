#include "UIComponent.h"

#include <utility>

#include "SceneManager.h"

dae::UIComponent::UIComponent(GameObject* pOwner, std::function<void(GameObject* pCaller)> drawFunc)
    : Component(pOwner)
    , m_drawFunc(std::move(drawFunc))
{
    SceneManager::Get().RegisterUIComponent(this);
}

dae::UIComponent::~UIComponent() noexcept
{
    SceneManager::Get().UnregisterUIComponent(this);
}

void dae::UIComponent::Update(float /*deltaTime*/)
{
}

void dae::UIComponent::DrawUI() const
{
    m_drawFunc(m_pOwner);
}