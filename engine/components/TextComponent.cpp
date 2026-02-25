//
// Created by lily-laptop on 18/02/2026.
//

#include "TextComponent.h"

#include <utility>

#include "Renderer.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "Texture2D.h"

dae::TextComponent::TextComponent(GameObject* pOwner, std::string text, std::shared_ptr<dae::Font> font, const dae::Transform& transform, SDL_Color color)
    : Component(pOwner)
    , m_Transform(transform)
    , m_Font(std::move(font))
    , m_Color(color)
    , m_Text(std::move(text))
{
}

dae::TextComponent::~TextComponent() {}

void dae::TextComponent::Update(float /*deltaTime*/)
{
    if(not m_Text.empty() and not m_NeedsUpdate)
        return;

    auto* const surf = TTF_RenderText_Blended(m_Font->GetFont(), m_Text.c_str(), m_Text.length(), m_Color);
    if(surf == nullptr)
    {
        throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());
    }
    auto* texture = SDL_CreateTextureFromSurface(dae::Renderer::GetInstance().GetSDLRenderer(), surf);
    if(texture == nullptr)
    {
        throw std::runtime_error(std::string("Create text texture from surface failed: ") + SDL_GetError());
    }
    SDL_DestroySurface(surf);
    m_TextTexture = std::make_shared<dae::Texture2D>(texture);
    m_NeedsUpdate = false;
}

void dae::TextComponent::Render() const
{
    if(m_TextTexture == nullptr)
        return;

    const auto& pos{ m_Transform.GetPosition() };
    Renderer::GetInstance().RenderTexture(*m_TextTexture, pos.x, pos.y);
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
