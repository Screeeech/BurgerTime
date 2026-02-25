//
// Created by lily-laptop on 18/02/2026.
//

#include "FpsComponent.h"

#include <print>

dae::FpsComponent::FpsComponent(GameObject* pOwner, std::shared_ptr<dae::Font> font, const dae::Transform& transform, SDL_Color)
    : TextComponent(pOwner, "FPS: bleeehh", std::move(font), transform)
{
}

void dae::FpsComponent::Update(float deltaTime)
{
    TextComponent::Update(deltaTime);

    std::println("Delta time: {}", deltaTime);
    elapsedTime += deltaTime;
    ++frameCount;

    if (elapsedTime >= 1.f)
    {
        const float fps{ frameCount / elapsedTime };

        this->SetText(std::format("FPS: {:.2f}", fps));
        frameCount = 0;
        elapsedTime = 0.0f;
    }
}