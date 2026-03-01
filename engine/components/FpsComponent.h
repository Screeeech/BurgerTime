//
// Created by lily-laptop on 18/02/2026.
//

#ifndef ENGINE_FPSCOMPONENT_H
#define ENGINE_FPSCOMPONENT_H
#include <SDL3/SDL_pixels.h>

#include <memory>

#include "Component.h"

namespace dae
{
class TextComponent;
class Transform;
class Font;

class FpsComponent : public Component
{
public:
    FpsComponent(GameObject* pOwner, std::shared_ptr<Font> font, const Transform& transform, SDL_Color = { 255, 255, 255, 255 });
    void Update(float deltaTime) override;
private:
    int m_frameCount{};
    float m_elapsedTime{};
    TextComponent* m_pTextComponent{};
};

}

#endif  // ENGINE_FPSCOMPONENT_H
