//
// Created by lily-laptop on 18/02/2026.
//

#ifndef ENGINE_FPSCOMPONENT_H
#define ENGINE_FPSCOMPONENT_H
#include "Component.h"
#include "TextComponent.h"

class FpsComponent : public TextComponent
{
public:
    FpsComponent(std::shared_ptr<dae::Font> font, const dae::Transform& transform, SDL_Color = { 255, 255, 255, 255 });
    void Update(float deltaTime) override;
private:
    int frameCount{};
    float elapsedTime{};
};


#endif  // ENGINE_FPSCOMPONENT_H
