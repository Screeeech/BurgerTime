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
    explicit FpsComponent(GameObject* pOwner, std::shared_ptr<Font> font, SDL_Color = { .r=255, .g=255, .b=255, .a=255 });
    void Update(float deltaTime) override;
private:
    int m_frameCount{};
    float m_elapsedTime{};
    TextComponent* m_pTextComponent{};
};

}

#endif  // ENGINE_FPSCOMPONENT_H
