#ifndef ENGINE_ANIMATION_H
#define ENGINE_ANIMATION_H
#include <SDL3/SDL_rect.h>

#include <memory>
#include <unordered_map>

#include "Renderable.h"
#include "Texture2D.h"

namespace dae
{

struct SpriteSheet
{
    std::shared_ptr<Texture2D> texture;
    int cols;
    int rows;
};

struct Frame
{
    SpriteSheet& spriteSheet;
    float duration;
    SDL_FRect srcRect;
    bool flipX{};
    bool flipY{};
};

struct FrameData
{
    int colIdx;
    int rowIdx;
    float duration;
    bool flipX{};
    bool flipY{};
};

class Animation : public Renderable
{
public:
    explicit Animation(GameObject* pOwner, int zIndex = 0);
    ~Animation() noexcept override = default;

    void Update(float deltaTime) override;
    void Render() override;

    void SetPlaying(bool playing);

    SpriteSheet& AddSpriteSheet(const std::shared_ptr<Texture2D>& texture, int cols, int rows);
    void AddAnimation(unsigned int animationID, SpriteSheet& spriteSheet, std::initializer_list<FrameData> frameData);

    void SetActiveAnimation(unsigned int animationID, bool startPlaying = false);
private:
    const Frame* GetActiveFrame() const;
    void AdvanceFrame();

    std::vector<SpriteSheet> m_spriteSheets;
    std::unordered_map<unsigned int, std::vector<Frame>> m_animations;

    float m_elapsedTime{};
    unsigned int m_currentAnimation{};
    size_t m_frameIndex{};
    bool m_playing{};
};

}  // namespace dae

#endif  // ENGINE_ANIMATION_H
