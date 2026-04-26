#include "../../Include/Components/Animation.hpp"

#include "../../Include/Renderer.hpp"

namespace dae
{
Animation::Animation(GameObject* pOwner, int zIndex)
    : Renderable(pOwner, zIndex)
{
}

void Animation::Update(float deltaTime)
{
    if(not m_playing)
        return;

    m_elapsedTime += deltaTime;

    while(m_elapsedTime >= GetActiveFrame()->duration)
    {
        m_elapsedTime -= GetActiveFrame()->duration;
        AdvanceFrame();
    }
}

void Animation::Render()
{
    const auto* frame = GetActiveFrame();
    if(not frame)
        return;

    const glm::vec3 worldPos{ m_pOwner->GetWorldPosition() };
    const glm::vec2 scale{ m_pOwner->GetTransform().GetWorldScale() };

    Renderer::Get().RenderTextureScaleFlipped(*frame->spriteSheet.texture.get(), worldPos.x, worldPos.y, scale.x, scale.y,
                                              frame->flipX, frame->flipY, frame->srcRect);
}

void Animation::SetPlaying(bool playing)
{
    m_playing = playing;
}

SpriteSheet& Animation::AddSpriteSheet(const std::shared_ptr<Texture2D>& texture, int cols, int rows)
{
    return m_spriteSheets.emplace_back(texture, cols, rows);
}

void Animation::AddAnimation(unsigned int animationID, SpriteSheet& spriteSheet, std::initializer_list<FrameData> frameData)
{
    if(m_animations.contains(animationID))
        return;

    const glm::vec2 size{ spriteSheet.texture->GetSize() };
    const float width{ size.x / static_cast<float>(spriteSheet.cols) };
    const float height{ size.y / static_cast<float>(spriteSheet.rows) };

    std::vector<Frame> frames;
    frames.reserve(frames.size());
    for(const auto& frame : frameData)
    {
        const SDL_FRect srcRect{
            .x = static_cast<float>(frame.colIdx) * width, .y = static_cast<float>(frame.rowIdx) * height, .w = width, .h = height
        };
        frames.emplace_back(spriteSheet, frame.duration, srcRect, frame.flipX, frame.flipY);
    }

    m_animations.emplace(animationID, std::move(frames));
}

void Animation::SetActiveAnimation(unsigned int animationID, bool startPlaying)
{
    if(not m_animations.contains(animationID))
        throw std::runtime_error("Animation not found");

    m_currentAnimation = animationID;
    m_frameIndex = 0;
    m_elapsedTime = 0.0f;
    SetPlaying(startPlaying);
}

const Frame* Animation::GetActiveFrame() const
{
    if(m_animations.empty())
        return nullptr;

    return &m_animations.at(m_currentAnimation).at(m_frameIndex);
}

void Animation::AdvanceFrame()
{
    ++m_frameIndex;
    if(m_frameIndex >= m_animations.at(m_currentAnimation).size())
        m_frameIndex = 0;
}

}  // namespace dae