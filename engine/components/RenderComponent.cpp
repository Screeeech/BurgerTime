#include <utility>

#include "GameObject.h"
#include "RenderComponent.h"
#include "Renderer.h"

dae::RenderComponent::RenderComponent(GameObject* pOwner, std::shared_ptr<Texture2D> texture)
    : Component(pOwner)
    , m_Texture(std::move(texture))
{
    SceneManager::Get().RegisterRenderComponent(this);
}

dae::RenderComponent::RenderComponent(GameObject* pOwner)
    : Component(pOwner)
{
    auto& sm = SceneManager::Get();
    sm.RegisterRenderComponent(this);
}

dae::RenderComponent::~RenderComponent() noexcept
{
    auto& sm = SceneManager::Get();
    sm.UnregisterRenderComponent(this);
}

void dae::RenderComponent::Update(float /*deltaTime*/)
{
}

void dae::RenderComponent::Render() const
{
    if(not m_Texture)
        return;

    const auto& pos{ m_pOwner->GetWorldPosition() };
    Renderer::Get().RenderTexture(*m_Texture, pos.x, pos.y);
}

void dae::RenderComponent::SetTexture(std::shared_ptr<Texture2D> texture)
{
    m_Texture = std::move(texture);
}