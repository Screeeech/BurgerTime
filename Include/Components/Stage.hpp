#ifndef BURGERTIME_STAGE_HPP
#define BURGERTIME_STAGE_HPP

#include <SDL3/SDL_pixels.h>

#include <nlohmann/json_fwd.hpp>
#include <string>

#include "Component.hpp"
#include "Entity.hpp"
#include "Renderable.hpp"
#include "Services/Renderer.hpp"

namespace gla
{
class CollisionRect;
class Timer;
}
namespace bt
{

struct SpawnPoints final
{
    glm::vec2 player1;
    glm::vec2 player2;
    glm::vec2 enemy;
};


class Stage : public gla::Renderable
{
public:
    enum class Tile : std::uint8_t
    {
        Null = 0,
        Platform = 1,
        Ladder = 2,
        LadderPlatform = 3,
    };

    explicit Stage(gla::GameObject* pOwner, std::string const& stageDataPath);

    void PrintTileType(glm::vec2 position) const;

    [[nodiscard]] auto GetTileAtPosition(glm::vec2 stageLocalPosition) const -> Tile;
    [[nodiscard]] auto GetSpawnPositions() const -> SpawnPoints;
    [[nodiscard]] auto GetEnemyCounts() -> std::unordered_map<Entity::Type, int>&;

    static constexpr uint32_t stageWidth{ 9 };
    static constexpr uint32_t stageHeight{ 12 };
    static constexpr uint32_t stageSize{ stageWidth * stageHeight };
    static constexpr float stageOffset{ 32.f };
    static constexpr float tileWidth{ 24.f };
    static constexpr float tileHeight{ 16.f };

protected:
    void Render() override;
    void OnActivate() override;
    void OnDeactivate() override;

private:
    void OnBonusAppear(std::any const& eventArgs) const;
    void OnPlateFinished(std::any const& eventArgs);

    void LoadTiles(nlohmann::json const& tileList);
    void LoadSpawnPositions(nlohmann::json const& spawnList);
    void SpawnBurgerParts(nlohmann::json const& burgerPartList);
    void SpawnPlates(nlohmann::json const& plateList);
    void LoadEnemyCounts(nlohmann::json const& enemyCounts);
    void LoadFood(nlohmann::json const& food);


    [[nodiscard]] auto GetTileAtIndex(uint32_t xIdx, uint32_t yIdx) const -> Tile;
    static void DrawPlatform(glm::vec2 cursor, bool connectLeft, bool connectRight, gla::Renderer const& renderer);
    static void DrawLadder(glm::vec2 cursor, gla::Renderer const& renderer);

    int m_platesFinished{};
    int m_totalPlateCount{};
    std::array<Tile, stageSize> m_tileArray;
    gla::Timer* m_pTimer;
    SpawnPoints m_spawnPoints{};
    std::unordered_map<Entity::Type, int> m_enemyCounts;

    // Food
    gla::Sprite* m_pFoodSprite{};
    gla::CollisionRect* m_pFoodHitBox{};
};

}  // namespace bt

#endif  // BURGERTIME_STAGE_HPP
