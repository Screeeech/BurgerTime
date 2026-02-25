//
// Created by lily-laptop on 18/02/2026.
//

#include "SpriteComponent.h"

#include <utility>

#include "Renderer.h"
#include "GameObject.h"

dae::SpriteComponent::SpriteComponent(GameObject* pOwner, std::shared_ptr<Texture2D> texture, const Transform& transform)
    : Component(pOwner)
    , m_Transform(transform)
    , m_Texture(std::move(texture))
{
}

void dae::SpriteComponent::Update(float /*deltaTime*/)
{
}

void dae::SpriteComponent::Render() const
{
    const auto& pos{ m_Transform.GetPosition() };
    Renderer::GetInstance().RenderTexture(*m_Texture, pos.x, pos.y);
}

void dae::SpriteComponent::SetTexture(std::shared_ptr<dae::Texture2D> texture)
{
    m_Texture = std::move(texture);
}