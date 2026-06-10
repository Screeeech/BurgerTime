#ifndef BURGERTIME_STAGE_HPP
#define BURGERTIME_STAGE_HPP

#include <SDL3/SDL_pixels.h>

#include <nlohmann/json_fwd.hpp>
#include <string>

#include "Component.hpp"
#include "Renderable.hpp"
#include "Services/Renderer.hpp"

namespace gla
{
class Timer;
}
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

    void PrintTileType(glm::vec2 position) const;

    [[nodiscard]] auto GetTileAtPosition(glm::vec2 stageLocalPosition) const -> TileType;
    [[nodiscard]] auto GetSpawnPositions() const -> std::pair<glm::vec2, glm::vec2>;

    static constexpr uint32_t stageWidth{ 9 };
    static constexpr uint32_t stageHeight{ 12 };
    static constexpr uint32_t stageSize{ stageWidth * stageHeight };
    static constexpr float stageOffset{ 32.f };
    static constexpr float tileWidth{ 24.f };
    static constexpr float tileHeight{ 16.f };

    // TODO: Change back
    static constexpr float stageBeginDelay{ 0.5f };

protected:
    void Render() override;
    void OnActivate() override;
    void OnDeactivate() override;

private:

    int m_platesFinished{};
    int m_totalPlateCount{};
    std::array<TileType, stageSize> m_tileArray;
    gla::Timer* m_pTimer;
    std::pair<glm::vec2, glm::vec2> m_spawnPositions{};

    void OnPlateFinished(std::any const& eventArgs);
    [[nodiscard]] auto GetTileAtIndex(uint32_t xIdx, uint32_t yIdx) const -> TileType;

    void LoadTiles(nlohmann::json const& tileList);
    void LoadSpawnPositions(nlohmann::json const& spawnList);
    void SpawnBurgerParts(nlohmann::json const& burgerPartList);
    void SpawnPlates(nlohmann::json const& plateList);

    static void DrawPlatform(glm::vec2 cursor, bool connectLeft, bool connectRight, gla::Renderer const& renderer);
    static void DrawLadder(glm::vec2 cursor, gla::Renderer const& renderer);
};

}  // namespace bt

#endif  // BURGERTIME_STAGE_HPP
