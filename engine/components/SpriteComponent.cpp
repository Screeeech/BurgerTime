//
// Created by lily-laptop on 18/02/2026.
//

#include "SpriteComponent.h"

#include <utility>

#include "Renderer.h"

SpriteComponent::SpriteComponent(std::shared_ptr<dae::Texture2D> texture, const dae::Transform& transform)
    : m_Transform(transform)
    , m_Texture(std::move(texture))
{
}

void SpriteComponent::Update(float /*deltaTime*/)
{
}

void SpriteComponent::Render() const
{
    const auto& pos{ m_Transform.GetPosition() };
    dae::Renderer::GetInstance().RenderTexture(*m_Texture, pos.x, pos.y);
}

void SpriteComponent::SetTexture(std::shared_ptr<dae::Texture2D> texture)
{
    m_Texture = std::move(texture);
}