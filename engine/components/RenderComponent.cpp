//
// Created by lily-laptop on 18/02/2026.
//

#include <utility>

#include "GameObject.h"
#include "RenderComponent.h"
#include "Renderer.h"

dae::RenderComponent::RenderComponent(GameObject* pOwner, std::shared_ptr<Texture2D> texture, const Transform& transform)
    : Component(pOwner)
    , m_Transform(transform)
    , m_Texture(std::move(texture))
{
    SceneManager::GetInstance().RegisterRenderComponent(this);
}

dae::RenderComponent::~RenderComponent()
{
    SceneManager::GetInstance().UnregisterRenderComponent(this);
}

void dae::RenderComponent::Update(float /*deltaTime*/)
{
}

void dae::RenderComponent::Render() const
{
    const auto& pos{ m_Transform.GetPosition() };
    Renderer::GetInstance().RenderTexture(*m_Texture, pos.x, pos.y);
}

void dae::RenderComponent::SetTexture(std::shared_ptr<Texture2D> texture)
{
    m_Texture = std::move(texture);
}