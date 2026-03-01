//
// Created by lily-laptop on 18/02/2026.
//

#ifndef MINIGIN_TEXTCOMPONENT_H
#define MINIGIN_TEXTCOMPONENT_H

#include <SDL3/SDL_pixels.h>

#include <memory>

#include "Component.h"
#include "Font.h"
#include "Transform.h"

namespace dae
{
class RenderComponent;
class Texture2D;

class TextComponent : public Component
{
public:
    explicit TextComponent(GameObject* pOwner, std::string text, std::shared_ptr<Font> font,
                  SDL_Color color = { .r = 255, .g = 255, .b = 255, .a = 255 });
    ~TextComponent() override = default;

    void Update(float deltaTime) override;
    void SetText(const std::string& text);
    [[nodiscard]] const std::string& GetText() const;

private:
    std::shared_ptr<Font> m_Font;
    std::shared_ptr<Texture2D> m_TextTexture;
    SDL_Color m_Color;
    std::string m_Text;

    bool m_NeedsUpdate{ false };

    RenderComponent* m_pRenderComponent{};

    std::shared_ptr<Texture2D> UpdateTexture() const;
};

}  // namespace dae

#endif  // MINIGIN_TEXTCOMPONENT_H
