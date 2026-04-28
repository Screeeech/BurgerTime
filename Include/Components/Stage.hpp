#ifndef BURGERTIME_STAGE_HPP
#define BURGERTIME_STAGE_HPP
#include <SDL3/SDL_pixels.h>

#include "Component.hpp"
#include "Renderable.hpp"

namespace bt
{

enum class TileType : std::uint8_t
{
    Null = 0,
    Platform = 1,
    Ladder = 2,
    LadderPlatform = 3,
};

class Stage : public gla::Renderable
{
    static constexpr int zIndex{ 1 };
    static constexpr size_t stageWidth{ 9 };
    static constexpr size_t stageHeight{ 10 };
    static constexpr size_t stageSize{ stageWidth * stageHeight };


public:
    explicit Stage(gla::GameObject* pOwner, std::string const& stageDataPath);
    ~Stage() noexcept override = default;

    Stage(Stage const&) = delete;
    auto operator=(Stage const&) -> Stage& = delete;
    Stage(Stage&&) = delete;
    auto operator=(Stage&&) -> Stage& = delete;

    void Update(float deltaTime) override;
    void Render() override;

private:
    std::array<TileType, stageSize> m_tileArray;


    [[nodiscard]] auto GetTileType(uint32_t xIdx, uint32_t yIdx) const -> TileType;

    static void DrawPlatform(glm::vec2 cursor, bool connectLeft, bool connectRight);
    static void DrawLadder(glm::vec2 cursor);
};

}  // namespace bt

#endif  // BURGERTIME_STAGE_HPP
