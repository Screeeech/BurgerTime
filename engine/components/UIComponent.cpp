//
// Created by lily-laptop on 04/03/2026.
//

#include "UIComponent.h"

#include <utility>

#include "SceneManager.h"

dae::UIComponent::UIComponent(GameObject* pOwner, std::function<void(GameObject* pCaller)> drawFunc)
    : Component(pOwner)
    , m_drawFunc(std::move(drawFunc))
{
    SceneManager::GetInstance().RegisterUIComponent(this);
}

dae::UIComponent::~UIComponent() noexcept
{
    SceneManager::GetInstance().UnregisterUIComponent(this);
}

void dae::UIComponent::Update(float /*deltaTime*/)
{
}

void dae::UIComponent::DrawUI() const
{
    m_drawFunc(m_pOwner);
}