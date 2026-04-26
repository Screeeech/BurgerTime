#include "TextComponent.hpp"

#include <SDL3_ttf/SDL_ttf.h>

#include <stdexcept>
#include <utility>

#include "GameObject.hpp"
#include "Renderer.hpp"
#include "Sprite.hpp"
#include "Texture2D.hpp"

dae::TextComponent::TextComponent(GameObject* pOwner, std::string text, std::shared_ptr<Font> font, int zIndex, SDL_Color color)
    : Component(pOwner)
    , m_Font(std::move(font))
    , m_Color(color)
    , m_Text(std::move(text))
    , m_pRenderComponent(pOwner->AddComponent<Sprite>(zIndex))
{
    m_TextTexture = UpdateTexture();
    m_pRenderComponent->SetTexture(m_TextTexture);
}

void dae::TextComponent::Update(float /*deltaTime*/)
{
    if(not m_Text.empty() and not m_NeedsUpdate)
        return;

    m_TextTexture = UpdateTexture();
    m_NeedsUpdate = false;
    m_pRenderComponent->SetTexture(m_TextTexture);
}

void dae::TextComponent::SetText(const std::string& text)
{
    m_Text = text;
    m_NeedsUpdate = true;
}

const std::string& dae::TextComponent::GetText() const
{
    return m_Text;
}

std::shared_ptr<dae::Texture2D> dae::TextComponent::UpdateTexture() const
{
    auto* const surf = TTF_RenderText_Blended(m_Font->GetFont(), m_Text.c_str(), m_Text.length(), m_Color);
    if(surf == nullptr)
    {
        throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());
    }
    auto* texture = SDL_CreateTextureFromSurface(Renderer::Get().GetSDLRenderer(), surf);
    if(texture == nullptr)
    {
        throw std::runtime_error(std::string("Create text texture from surface failed: ") + SDL_GetError());
    }
    SDL_DestroySurface(surf);

    return std::make_shared<Texture2D>(texture);
}
