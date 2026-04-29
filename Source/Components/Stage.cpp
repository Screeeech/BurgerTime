#include "Components/Stage.hpp"

#include <array>
#include <cstdint>
#include <fstream>
#include <nlohmann/json.hpp>
#include <print>
#include <ranges>

#include "Utils.hpp"
#include "Colors.hpp"
#include "ServiceLocator.hpp"
#include "Services/Renderer.hpp"
#include "Services/SoundService.hpp"

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

    for (auto const& [idx, tile] : stageData.at("tiles") | vw::join | vw::enumerate)
    {
        m_tileArray.at(idx) = tile;
    }

    if (auto* sound{ gla::ServiceLocator::Request<gla::SoundService>().value_or(nullptr) })
        sound->PlayAudio("game_start"_h);
}

void Stage::Update(float /*deltaTime*/)
{

}

void Stage::Render()
{
    auto const* renderer{ gla::ServiceLocator::Request<gla::Renderer>().value_or(nullptr) };
    if (not renderer)
    {
        std::println("Careful! No renderer was found, proceeding without rendering stage");
        return;
    }

    for (auto const [idx, tile] : m_tileArray | vw::enumerate)
    {
        auto const xIdx{ idx % stageWidth };
        auto const yIdx{ idx / stageWidth };
        glm::vec2 const cursor{ static_cast<float>(xIdx * 24) + 32.f, static_cast<float>(yIdx * 16) + 32.f };

        bool connectLeft{};
        bool connectRight{};
        auto const leftTile{ GetTileType(xIdx - 1, yIdx) };
        auto const rightTile{ GetTileType(xIdx + 1, yIdx) };
        if (leftTile == TileType::Platform or leftTile == TileType::LadderPlatform)
            connectLeft = true;
        if (rightTile == TileType::Platform or rightTile == TileType::LadderPlatform)
            connectRight = true;

        switch (tile)
        {
            case TileType::Platform:
            {
                DrawPlatform(cursor, connectLeft, connectRight, renderer);
            }
            break;
            case TileType::LadderPlatform:
            {
                DrawPlatform(cursor, connectLeft, connectRight, renderer);
                [[fallthrough]];
            }
            case TileType::Ladder:
            {
                renderer->SetColor(xIdx % 2 != 0 ? colors::GreenLadderColor : colors::BlueLadderColor);
                DrawLadder(cursor, renderer);
            }
            break;
            case TileType::Null:
                break;
        }
    }
}

auto Stage::GetTileType(uint32_t xIdx, uint32_t yIdx) const -> TileType
{
    if (xIdx >= stageWidth or yIdx >= stageHeight)
        return TileType::Null;

    auto const idx = (yIdx * stageWidth) + xIdx;
    if (idx >= m_tileArray.size())
        return TileType::Null;

    return m_tileArray.at(idx);
}

void Stage::DrawPlatform(glm::vec2 cursor, bool connectLeft, bool connectRight, gla::Renderer const* renderer)
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

    renderer->SetColor(colors::PlatformColor);
    renderer->DrawRect({ .x = cursor.x, .y = cursor.y, .w = width, .h = 2.f });
}

void Stage::DrawLadder(glm::vec2 cursor, gla::Renderer const* renderer)
{
    renderer->DrawLines(
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