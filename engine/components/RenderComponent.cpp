#include <utility>

#include "GameObject.h"
#include "RenderComponent.h"
#include "Renderer.h"

dae::RenderComponent::RenderComponent(GameObject* pOwner, std::shared_ptr<Texture2D> texture, int zIndex)
    : Component(pOwner)
    , m_Texture(std::move(texture))
    , m_zIndex(zIndex)
{
    SceneManager::Get().RegisterRenderComponent(this);
}

dae::RenderComponent::RenderComponent(GameObject* pOwner, int zIndex)
    : Component(pOwner)
    , m_zIndex(zIndex)
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

void dae::RenderComponent::SetZIndex(int zIndex)
{
    m_zIndex = zIndex;
    SceneManager::Get().SortCachedRenderComponents();
}

int dae::RenderComponent::GetZIndex() const
{
    return m_zIndex;
}