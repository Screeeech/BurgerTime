#include "Components/Stage.hpp"

#include <array>
#include <cstdint>
#include <fstream>
#include <nlohmann/json.hpp>
#include <print>
#include <ranges>

#include "Colors.hpp"
#include "Locator.hpp"
#include "Services/ISound.hpp"
#include "Services/Renderer.hpp"
#include "Utils.hpp"

using nlohmann::json;

namespace vw = std::ranges::views;

namespace bt
{

Stage::Stage(gla::GameObject* pOwner, std::string const& stageDataPath)
    : Renderable{ pOwner, zIndex }
    , m_tileArray{}
{
    std::ifstream file{ stageDataPath };
    if (not file.is_open())
        throw std::runtime_error{ "Could not open stage data file" };

    json const stageData = json::parse(file);

    // clang-format off
    if (not stageData.contains("tiles") or
        not stageData.at("tiles").is_array() or
        stageData.at("tiles").size() != stageHeight or
        stageData.at("tiles").at(0).size() != stageWidth)
        throw std::runtime_error{ "Invalid stage data file" };

    for (auto const& [i, tile] : stageData.at("tiles") | vw::join | vw::enumerate)
        m_tileArray.at(i) = tile;

    gla::Locator::Get<gla::ISound>().PlayAudio("game_start"_h);
}

void Stage::Render()
{
    auto& renderer{ gla::Locator::Get<gla::Renderer>() };
    for (size_t i{}; i < m_tileArray.size(); ++i)
    {
        auto const& tile = m_tileArray.at(i);
        uint32_t const xIdx{ static_cast<uint32_t>(i) % stageWidth };
        uint32_t const yIdx{ static_cast<uint32_t>(i) / stageWidth };
        glm::vec2 const cursor{ (static_cast<float>(xIdx) * tileWidth) + stageOffset, (static_cast<float>(yIdx) * tileHeight) + stageOffset };

        bool connectLeft{};
        bool connectRight{};
        TileType const leftTile{ GetTileAtIndex(xIdx - 1, yIdx) };
        TileType const rightTile{ GetTileAtIndex(xIdx + 1, yIdx) };
        if (leftTile == TileType::Platform or leftTile == TileType::LadderPlatform)
            connectLeft = true;
        if (rightTile == TileType::Platform or rightTile == TileType::LadderPlatform)
            connectRight = true;

        if (tile == TileType::Platform)
        {
            //renderer->SetColor(colors::Red);
            //renderer->DrawRect({cursor.x, cursor.y, tileWidth, tileHeight});

            DrawPlatform(cursor, connectLeft, connectRight, renderer);
        }
        else if (tile == TileType::LadderPlatform)
        {
            //renderer->SetColor(colors::Green);
            //renderer->DrawRect({cursor.x, cursor.y, tileWidth, tileHeight});

            DrawPlatform(cursor, connectLeft, connectRight, renderer);
            renderer.SetColor(xIdx % 2 != 0 ? colors::GreenLadderColor : colors::BlueLadderColor);
            DrawLadder(cursor, renderer);
        }
        else if (tile == TileType::Ladder)
        {
            //renderer->SetColor(colors::Blue);
            //renderer->DrawRect({cursor.x, cursor.y, tileWidth, tileHeight});

            renderer.SetColor(xIdx % 2 != 0 ? colors::GreenLadderColor : colors::BlueLadderColor);
            DrawLadder(cursor, renderer);
        }
    }
}

void Stage::PrintTileType(glm::vec3 position) const
{
    std::println("Position: {}, {}", position.x - stageOffset, position.y - stageOffset);
    switch (GetTileAtPosition(position))
    {
        case TileType::Null:
            std::println("Tile: Null");
            break;
        case TileType::Platform:
            std::println("Tile: Platform");
            break;
        case TileType::Ladder:
            std::println("Tile: Ladder");
            break;
        case TileType::LadderPlatform:
            std::println("Tile: LadderPlatform");
            break;
    }
}

auto Stage::IsOnGround(glm::vec3 position) const -> bool
{
    int const mod = static_cast<int>(position.y - stageOffset) % static_cast<int>(tileHeight);
    TileType const tile = GetTileAtPosition(position - glm::vec3{0.f, 2.f, 0.f});

    if (mod != 14)
        return false;

    switch (tile)
    {
        case TileType::Platform:
            return true;
        case TileType::LadderPlatform:
            return true;
        default:
            return false;
    }
}

auto Stage::CanClimbUp(glm::vec3 position) const-> bool
{
    int const mod = static_cast<int>(position.x - stageOffset) % static_cast<int>(tileWidth);
    if (mod > 4 and mod < 20)
        return false;

    auto const tile = GetTileAtPosition(position);
    switch (tile)
    {
        case TileType::Ladder:
            return true;
        case TileType::LadderPlatform:
            return true;
        default:
            return false;
    }
}

auto Stage::CanClimbDown(glm::vec3 position) const-> bool
{
    int const mod = static_cast<int>(position.x - stageOffset) % static_cast<int>(tileWidth);
    if (mod > 4 and mod < 20)
        return false;

    auto const tile = GetTileAtPosition(position);
    switch (tile)
    {
        case TileType::Ladder:
            return true;
        case TileType::LadderPlatform:
            return true;
        default:
            return false;
    }
}

auto Stage::CanWalk(glm::vec3 position, glm::vec3 direction) const-> bool
{
    direction.y = 0.f;
    TileType const tile = GetTileAtPosition(position + (direction * 8.f));

    return tile == TileType::Platform or tile == TileType::LadderPlatform;
}

auto Stage::GetTileAtPosition(glm::vec3 position) const-> TileType
{
    position.x -= stageOffset;
    position.y -= stageOffset;

    if (position.x < 0.f or position.y < 0.f)
        return TileType::Null;

    auto const xIdx{ static_cast<uint32_t>(position.x / tileWidth) };
    auto const yIdx{ static_cast<uint32_t>(position.y / tileHeight) };

    return GetTileAtIndex(xIdx, yIdx);
}

auto Stage::GetTileAtIndex(uint32_t xIdx, uint32_t yIdx) const -> TileType
{
    if (xIdx >= stageWidth or yIdx >= stageHeight)
        return TileType::Null;

    auto const idx = (yIdx * stageWidth) + xIdx;
    if (idx >= m_tileArray.size())
        return TileType::Null;

    return m_tileArray.at(idx);
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