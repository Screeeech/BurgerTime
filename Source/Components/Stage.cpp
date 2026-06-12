#include "Components/Stage.hpp"

#include <array>
#include <cstdint>
#include <fstream>
#include <nlohmann/json.hpp>
#include <print>
#include <ranges>

#include "Components/BurgerPart.hpp"
#include "Components/Plate.hpp"
#include "Components/Timer.hpp"
#include "Constants.hpp"
#include "GameEvents.hpp"
#include "Locator.hpp"
#include "Services/EventManager.hpp"
#include "Services/Renderer.hpp"
#include "Services/ResourceManager.hpp"
#include "Services/Sound.hpp"
#include "Utils.hpp"

using nlohmann::json;

namespace vw = std::ranges::views;

namespace bt
{

Stage::Stage(gla::GameObject* pOwner,std::string const& stageDataPath)
    : Renderable{ pOwner, layers::stage }
    , m_tileArray{}
    , m_pTimer(pOwner->AddComponent<gla::Timer>())
{
    std::ifstream file{ stageDataPath };
    if (not file.is_open())
        throw std::runtime_error{ "Could not open stage data file" };

    json const stageData = json::parse(file);

    if (not stageData.contains("Tiles"))
        throw std::runtime_error{ R"(Invalid stage data file: No "Tiles" list found)" };
    LoadTiles(stageData.at("Tiles"));

    if (not stageData.contains("Spawns"))
        throw std::runtime_error{ R"(Invalid stage data file: No "Spawns" list found)" };
    LoadSpawnPositions(stageData.at("Spawns"));

    if (not stageData.contains("Parts"))
        throw std::runtime_error{ R"(Invalid stage data file: No "Parts" list found)" };
    SpawnBurgerParts(stageData.at("Parts"));

    if (not stageData.contains("Plates"))
        throw std::runtime_error{ R"(Invalid stage data file: No "Plates" list found)" };
    SpawnPlates(stageData.at("Plates"));

    if (not stageData.contains("Enemies"))
        throw std::runtime_error{ R"(Invalid stage data file: No "Enemies" list found)" };
    LoadEnemyCounts(stageData.at("Enemies"));
}

void Stage::PrintTileType(glm::vec2 position) const
{
    switch (GetTileAtPosition(position))
    {
        case Tile::Null:
            std::println("Tile: Null");
            break;
        case Tile::Platform:
            std::println("Tile: Platform");
            break;
        case Tile::Ladder:
            std::println("Tile: Ladder");
            break;
        case Tile::LadderPlatform:
            std::println("Tile: LadderPlatform");
            break;
    }
}

auto Stage::GetTileAtPosition(glm::vec2 stageLocalPosition) const -> Tile
{
    if (stageLocalPosition.x < 0.f or stageLocalPosition.y < 0.f)
        return Tile::Null;

    auto const xIdx{ static_cast<uint32_t>(stageLocalPosition.x / tileWidth) };
    auto const yIdx{ static_cast<uint32_t>(stageLocalPosition.y / tileHeight) };

    return GetTileAtIndex(xIdx, yIdx);
}

auto Stage::GetSpawnPositions() const -> SpawnPoints
{
    return m_spawnPoints;
}

auto Stage::GetEnemyCounts() -> std::unordered_map<Entity::Type, int>&
{
    return m_enemyCounts;
}

void Stage::Render()
{
    auto const& renderer{ gla::Locator::Get<gla::Renderer>() };
    for (size_t i{}; i < m_tileArray.size(); ++i)
    {
        auto const& tile = m_tileArray.at(i);
        uint32_t const xIdx{ static_cast<uint32_t>(i) % stageWidth };
        uint32_t const yIdx{ static_cast<uint32_t>(i) / stageWidth };
        glm::vec2 const cursor{ (static_cast<float>(xIdx) * tileWidth) + stageOffset, (static_cast<float>(yIdx) * tileHeight) + stageOffset };

        bool connectLeft{};
        bool connectRight{};
        Tile const leftTile{ GetTileAtIndex(xIdx - 1, yIdx) };
        Tile const rightTile{ GetTileAtIndex(xIdx + 1, yIdx) };
        if (leftTile == Tile::Platform or leftTile == Tile::LadderPlatform)
            connectLeft = true;
        if (rightTile == Tile::Platform or rightTile == Tile::LadderPlatform)
            connectRight = true;

        if (tile == Tile::Platform)
        {
            // renderer->SetColor(colors::Red);
            // renderer->DrawRect({cursor.x, cursor.y, tileWidth, tileHeight});

            DrawPlatform(cursor, connectLeft, connectRight, renderer);
        }
        else if (tile == Tile::LadderPlatform)
        {
            // renderer->SetColor(colors::Green);
            // renderer->DrawRect({cursor.x, cursor.y, tileWidth, tileHeight});

            DrawPlatform(cursor, connectLeft, connectRight, renderer);
            renderer.SetColor(xIdx % 2 != 0 ? colors::GreenLadderColor : colors::BlueLadderColor);
            DrawLadder(cursor, renderer);
        }
        else if (tile == Tile::Ladder)
        {
            // renderer->SetColor(colors::Blue);
            // renderer->DrawRect({cursor.x, cursor.y, tileWidth, tileHeight});

            renderer.SetColor(xIdx % 2 != 0 ? colors::GreenLadderColor : colors::BlueLadderColor);
            DrawLadder(cursor, renderer);
        }
    }
}

void Stage::OnActivate()
{
    Renderable::OnActivate();

    gla::Locator::Get<gla::Sound>().PlayAudio("game_start"_h);

    auto& eventManager = gla::Locator::Get<gla::EventManager>();
    eventManager.BindEvent("PlateFinished"_h, this, &Stage::OnPlateFinished);
    eventManager.QueueEvent(gla::Event("DisableEntities"_h));

    m_pTimer->Start(
        stageBeginDelay,
        [] -> void
        {
            gla::Locator::Get<gla::EventManager>().QueueEvent(gla::Event("EnableEntities"_h));
            gla::Locator::Get<gla::Sound>().PlayTrack("background");
        });
}

void Stage::OnDeactivate()
{
    Renderable::OnDeactivate();

    gla::Locator::Get<gla::EventManager>().UnbindEvents(this);
}

void Stage::OnPlateFinished(std::any const& /*eventArgs*/)
{
    ++m_platesFinished;
    std::println("Plate {}/{} finished!", m_platesFinished, m_totalPlateCount);
    if (m_platesFinished >= m_totalPlateCount)
    {
        gla::Locator::Get<gla::EventManager>().InvokeEvent(gla::Event("StageCompleted"_h));
        gla::Locator::Get<gla::Sound>().PlayAudio("round_clear"_h);
    }
}

auto Stage::GetTileAtIndex(uint32_t xIdx, uint32_t yIdx) const -> Tile
{
    if (xIdx >= stageWidth or yIdx >= stageHeight)
        return Tile::Null;

    auto const idx = (yIdx * stageWidth) + xIdx;
    if (idx >= m_tileArray.size())
        return Tile::Null;

    return m_tileArray.at(idx);
}

void Stage::LoadTiles(json const& tileList)
{
    if (not tileList.is_array() or tileList.size() != stageHeight or tileList.at(0).size() != stageWidth)
        throw std::runtime_error{ "TileList incorrectly formatted" };

    for (auto const& [i, tile] : tileList | vw::join | vw::enumerate)
        m_tileArray.at(i) = tile;
}

void Stage::LoadSpawnPositions(json const& spawnList)
{
    if (not spawnList.is_array() or spawnList.size() != 3)
        throw std::runtime_error{ "Spawn list incorrectly formatted" };

    auto getPos = [&](int idx) -> glm::vec2
    {
        auto const xIndex = spawnList.at(idx).at("x").get<float>();
        auto const yIndex = spawnList.at(idx).at("y").get<float>();
        return { xIndex * tileWidth, yIndex * tileHeight };
    };

    m_spawnPoints.player1 = getPos(0);
    m_spawnPoints.player2 = getPos(1);
    m_spawnPoints.enemy = getPos(2);
}

void Stage::SpawnBurgerParts(json const& burgerPartList)
{
    for (auto const& [i, part] : burgerPartList | vw::enumerate)
    {
        if (not part.is_object())
            throw std::runtime_error("Invalid parts list: Part is not a valid json object");

        if (part.size() != 1)
            throw std::runtime_error("Invalid parts list: Part pair must be of size 1");

        BurgerPart::Type type{};
        std::string key{};
        if (part.contains("TopBun"))
        {
            type = BurgerPart::Type::TopBun;
            key = "TopBun";
        }
        else if (part.contains("BottomBun"))
        {
            type = BurgerPart::Type::BottomBun;
            key = "BottomBun";
        }
        else if (part.contains("Cheese"))
        {
            type = BurgerPart::Type::Cheese;
            key = "Cheese";
        }
        else if (part.contains("Patty"))
        {
            type = BurgerPart::Type::Patty;
            key = "Patty";
        }
        else if (part.contains("Tomato"))
        {
            type = BurgerPart::Type::Tomato;
            key = "Tomato";
        }
        else if (part.contains("Lettuce"))
        {
            type = BurgerPart::Type::Lettuce;
            key = "Lettuce";
        }
        else
        {
            throw std::runtime_error("Invalid parts list: Part does not contain a valid key");
        }

        auto const& indices = part.at(key);
        int const xIndex = indices.at("x").get<int>();
        int const yIndex = indices.at("y").get<int>();

        auto const xPosition = 16.f + static_cast<float>(xIndex * 48);
        auto const yPosition = static_cast<float>(yIndex) * tileHeight;

        auto* partObject = m_pOwner->CreateChild(xPosition, yPosition, std::format("BurgerPart: {}, {}", key, i));
        partObject->AddComponent<BurgerPart>(
            this,
            type,
            gla::Locator::Get<gla::ResourceManager>().LoadTexture("Textures/spritesheet.png"));
    }
}

void Stage::SpawnPlates(json const& plateList)
{
    for (auto const& [i, plate] : plateList | vw::enumerate)
    {
        if (not plate.is_object())
            throw std::runtime_error("Invalid plates list: Plate is not a valid json object");
        if (not plate.contains("StackSize"))
            throw std::runtime_error(R"(Invalid plates list: Plate does not have valid "StackSize" field)");
        if (not plate.contains("x") or not plate.contains("y"))
            throw std::runtime_error(R"(Invalid plates list: Plate does not have valid "x" or "y" fields)");

        int const xIndex = plate.at("x").get<int>();
        int const yIndex = plate.at("y").get<int>();

        auto const xPosition = 16.f + static_cast<float>(xIndex * 48);
        auto const yPosition = static_cast<float>(yIndex) * tileHeight;

        int const stackSize = plate.at("StackSize").get<int>();

        auto* plateObject = m_pOwner->CreateChild(xPosition, yPosition, std::format("Plate: {}", i));
        plateObject->AddComponent<Plate>(stackSize, i);
        ++m_totalPlateCount;
    }
}
void Stage::LoadEnemyCounts(json const& enemyCounts)
{
    if (not enemyCounts.is_object())
        throw std::runtime_error("Invalid enemy count list: Enemies is not a valid json object");
    if (not enemyCounts.contains("HotDog"))
        throw std::runtime_error(R"(Invalid enemy count list: Enemies does not contain a "HotDog" entry)");
    if (not enemyCounts.contains("Pickle"))
        throw std::runtime_error(R"(Invalid enemy count list: Enemies does not contain a "Pickle" entry)");
    if (not enemyCounts.contains("Egg"))
        throw std::runtime_error(R"(Invalid enemy count list: Enemies does not contain a "Egg" entry)");

    m_enemyCounts[Entity::Type::HotDog] = enemyCounts.at("HotDog").get<int>();
    m_enemyCounts[Entity::Type::Pickle] = enemyCounts.at("Pickle").get<int>();
    m_enemyCounts[Entity::Type::Egg] = enemyCounts.at("Egg").get<int>();
}

void Stage::DrawPlatform(glm::vec2 cursor, bool connectLeft, bool connectRight, gla::Renderer const& renderer)
{
    cursor.y += 14.f;

    float width{ 16.f };
    if (connectLeft)
    {
        width += 8.f;
        cursor -= glm::vec2{ 8.f, 0.f };
    }
    if (connectRight)
    {
        width += 8.f;
    }

    renderer.SetColor(colors::PlatformColor);
    renderer.DrawRect({ .x = cursor.x, .y = cursor.y, .w = width, .h = 2.f });
}

void Stage::DrawLadder(glm::vec2 cursor, gla::Renderer const& renderer)
{
    renderer.DrawLines(
        {
            {
                cursor + glm::vec2{ 2.f, 0.f },
                cursor + glm::vec2{ 12.f, 0.f },
            },
            {
                cursor + glm::vec2{ 2.f, 2.f },
                cursor + glm::vec2{ 12.f, 2.f },
            },
            {
                cursor + glm::vec2{ 2.f, 8.f },
                cursor + glm::vec2{ 12.f, 8.f },
            },
            {
                cursor + glm::vec2{ 2.f, 10.f },
                cursor + glm::vec2{ 12.f, 10.f },
            },
        });
}

}  // namespace bt