#pragma once
#include <SDL3/SDL_surface.h>

#include <glm/vec2.hpp>
#include <string>

struct SDL_Texture;

namespace dae
{
/**
 * Simple RAII wrapper for an SDL_Texture
 */
class Texture2D final
{
public:
    [[nodiscard]] SDL_Texture* GetSDLTexture() const;
    explicit Texture2D(SDL_Texture* texture, SDL_ScaleMode scaleMode = SDL_SCALEMODE_LINEAR);

    explicit Texture2D(const std::string& fullPath, SDL_ScaleMode scaleMode = SDL_SCALEMODE_LINEAR);

    ~Texture2D() noexcept;

    [[nodiscard]] glm::vec2 GetSize() const;

    Texture2D(const Texture2D&) = delete;
    Texture2D(Texture2D&&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&&) = delete;

private:
    SDL_Texture* m_texture;
};
}  // namespace dae
