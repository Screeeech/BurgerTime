#include "RenderComponent.h"

#include <utility>

#include "GameObject.h"
#include "Renderer.h"
#include "Texture2D.h"
#include "Transform.h"

dae::RenderComponent::RenderComponent(GameObject* pOwner, std::shared_ptr<Texture2D> texture, int zIndex)
    : Renderable(pOwner, zIndex)
    , m_texture(std::move(texture))
    , m_sourceRect(0, 0, m_texture->GetSize().x, m_texture->GetSize().y)
{
    SceneManager::Get().RegisterRenderComponent(this);
}

dae::RenderComponent::RenderComponent(GameObject* pOwner, int zIndex)
    : Renderable(pOwner, zIndex)
    , m_sourceRect()
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

void dae::RenderComponent::Render()
{
    if(not m_texture or not m_Visible)
        return;

    const auto& pos{ m_pOwner->GetWorldPosition() };
    Renderer::Get().RenderTexture(*m_texture, pos.x, pos.y);
}

void dae::RenderComponent::SetTexture(std::shared_ptr<Texture2D> texture)
{
    m_texture = std::move(texture);
}

void dae::RenderComponent::SetSourceRect(SDL_FRect sourceRect)
{
    if(sourceRect.x < 0 or sourceRect.y < 0 or sourceRect.w < 0 or sourceRect.h < 0)
        throw std::invalid_argument("arguments must be greater than 0");

    m_sourceRect = sourceRect;
}

void dae::RenderComponent::SetSourceRect(float x, float y, float w, float h)
{
    if(x < 0 or y < 0 or w < 0 or h < 0)
        throw std::invalid_argument("arguments must be greater than 0");

    m_sourceRect.x = x;
    m_sourceRect.y = y;
    m_sourceRect.w = w;
    m_sourceRect.h = h;
}

void dae::RenderComponent::SetSourceRectPos(float x, float y)
{
    if(x < 0 or y < 0)
        throw std::invalid_argument("arguments must be greater than 0");

    m_sourceRect.x = x;
    m_sourceRect.y = y;
}

void dae::RenderComponent::SetSourceRectSize(float w, float h)
{
    if(w < 0 or h < 0)
        throw std::invalid_argument("arguments must be greater than 0");

    m_sourceRect.w = w;
    m_sourceRect.h = h;
}

SDL_FRect dae::RenderComponent::GetSourceRect() const
{
    return m_sourceRect;
}
