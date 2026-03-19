#include "FpsComponent.h"

#include <print>

#include "GameObject.h"
#include "TextComponent.h"

dae::FpsComponent::FpsComponent(GameObject* pOwner, std::shared_ptr<Font> font, SDL_Color color)
    : Component(pOwner)
    , m_pTextComponent(pOwner->AddComponent<TextComponent>("FPS: ", std::move(font), 999, color))
{
}

void dae::FpsComponent::Update(float deltaTime)
{
    m_elapsedTime += deltaTime;
    ++m_frameCount;

    if(m_elapsedTime >= 1.f)
    {
        const float fps{ static_cast<float>(m_frameCount) / m_elapsedTime };

        m_pTextComponent->SetText(std::format("FPS: {:.2f}", fps));
        m_frameCount = 0;
        m_elapsedTime = 0.0f;
    }
}