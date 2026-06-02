#ifndef BURGERTIME_STAGE_HPP
#define BURGERTIME_STAGE_HPP
#include <SDL3/SDL_pixels.h>

#include <string>

#include "Component.hpp"
#include "Renderable.hpp"
#include "Services/Renderer.hpp"

namespace bt
{

class Stage : public gla::Renderable
{
public:
    enum class TileType : std::uint8_t
    {
        Null = 0,
        Platform = 1,
        Ladder = 2,
        LadderPlatform = 3,
    };

    explicit Stage(gla::GameObject* pOwner, std::string const& stageDataPath);
    ~Stage() noexcept override = default;

    Stage(Stage const&) = delete;
    auto operator=(Stage const&) -> Stage& = delete;
    Stage(Stage&&) = delete;
    auto operator=(Stage&&) -> Stage& = delete;

    void PrintTileType(glm::vec2 position) const;

    [[nodiscard]] auto GetTileAtPosition(glm::vec2 globalPosition) const -> TileType;

    static constexpr int zIndex{ 1 };
    static constexpr uint32_t stageWidth{ 9 };
    static constexpr uint32_t stageHeight{ 10 };
    static constexpr uint32_t stageSize{ stageWidth * stageHeight };
    static constexpr float stageOffset{ 32.f };
    static constexpr float tileWidth{ 24.f };
    static constexpr float tileHeight{ 16.f };

protected:
    void Render() override;

private:
    std::array<TileType, stageSize> m_tileArray;

    [[nodiscard]] auto GetTileAtIndex(uint32_t xIdx, uint32_t yIdx) const -> TileType;

    static void DrawPlatform(glm::vec2 cursor, bool connectLeft, bool connectRight, gla::Renderer const& renderer);
    static void DrawLadder(glm::vec2 cursor, gla::Renderer const& renderer);
};

}  // namespace bt

#endif  // BURGERTIME_STAGE_HPP
