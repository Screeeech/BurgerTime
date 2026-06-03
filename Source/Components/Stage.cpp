#include "Components/Stage.hpp"

#include <array>
#include <cstdint>
#include <fstream>
#include <nlohmann/json.hpp>
#include <print>
#include <ranges>

#include "Components/BurgerPart.hpp"
#include "Constants.hpp"
#include "Locator.hpp"
#include "Services/ISound.hpp"
#include "Services/Renderer.hpp"
#include "Utils.hpp"

using nlohmann::json;

namespace vw = std::ranges::views;

namespace bt
{

Stage::Stage(gla::GameObject* pOwner, std::string const& stageDataPath, std::shared_ptr<gla::Texture2D> const& spriteSheetTexture)
    : Renderable{ pOwner, layers::stage }
    , m_tileArray{}
{
    std::ifstream file{ stageDataPath };
    if (not file.is_open())
        throw std::runtime_error{ "Could not open stage data file" };

    json const stageData = json::parse(file);

    if (not stageData.contains("tiles"))
        throw std::runtime_error{ "Invalid stage data file: No \"tiles\" list found" };
    PopulateTiles(stageData.at("tiles"));

    if (not stageData.contains("tiles"))
        throw std::runtime_error{ "Invalid stage data file: No \"parts\" list found" };
    SpawnBurgerParts(stageData.at("parts"), spriteSheetTexture);

    gla::Locator::Get<gla::ISound>().PlayAudio("game_start"_h);
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
        TileType const leftTile{ GetTileAtIndex(xIdx - 1, yIdx) };
        TileType const rightTile{ GetTileAtIndex(xIdx + 1, yIdx) };
        if (leftTile == TileType::Platform or leftTile == TileType::LadderPlatform)
            connectLeft = true;
        if (rightTile == TileType::Platform or rightTile == TileType::LadderPlatform)
            connectRight = true;

        if (tile == TileType::Platform)
        {
            // renderer->SetColor(colors::Red);
            // renderer->DrawRect({cursor.x, cursor.y, tileWidth, tileHeight});

            DrawPlatform(cursor, connectLeft, connectRight, renderer);
        }
        else if (tile == TileType::LadderPlatform)
        {
            // renderer->SetColor(colors::Green);
            // renderer->DrawRect({cursor.x, cursor.y, tileWidth, tileHeight});

            DrawPlatform(cursor, connectLeft, connectRight, renderer);
            renderer.SetColor(xIdx % 2 != 0 ? colors::GreenLadderColor : colors::BlueLadderColor);
            DrawLadder(cursor, renderer);
        }
        else if (tile == TileType::Ladder)
        {
            // renderer->SetColor(colors::Blue);
            // renderer->DrawRect({cursor.x, cursor.y, tileWidth, tileHeight});

            renderer.SetColor(xIdx % 2 != 0 ? colors::GreenLadderColor : colors::BlueLadderColor);
            DrawLadder(cursor, renderer);
        }
    }
}

void Stage::PrintTileType(glm::vec2 position) const
{
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

auto Stage::GetTileAtPosition(glm::vec2 stageLocalPosition) const -> TileType
{
    if (stageLocalPosition.x < 0.f or stageLocalPosition.y < 0.f)
        return TileType::Null;

    auto const xIdx{ static_cast<uint32_t>(stageLocalPosition.x / tileWidth) };
    auto const yIdx{ static_cast<uint32_t>(stageLocalPosition.y / tileHeight) };

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

void Stage::PopulateTiles(json const& tileList)
{
    if (not tileList.is_array() or tileList.size() != stageHeight or tileList.at(0).size() != stageWidth)
        throw std::runtime_error{ "TileList incorrectly formatted" };

    for (auto const& [i, tile] : tileList | vw::join | vw::enumerate)
        m_tileArray.at(i) = tile;
}

void Stage::SpawnBurgerParts(json const& burgerPartList, std::shared_ptr<gla::Texture2D> const& spriteSheetTexture)
{
    for (auto const& [i, part] : burgerPartList | vw::enumerate)
    {
        ValidateBurgerPart(part);

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

        auto* partObject = m_pOwner->CreateChild(xPosition, yPosition, std::format("BurgerPart{}", i));
        partObject->AddComponent<BurgerPart>(this, type, spriteSheetTexture);
    }
}

void Stage::ValidateBurgerPart(json const& burgerPart)
{
    if (not burgerPart.is_object())
        throw std::runtime_error("Invalid parts list: Part is not a valid json object");

    if (burgerPart.size() != 1)
        throw std::runtime_error("Invalid parts list: Part pair must be of size 1");
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
                cursor + glm::vec2{ 11.f, 0.f },
            },
            {
                cursor + glm::vec2{ 2.f, 2.f },
                cursor + glm::vec2{ 11.f, 2.f },
            },
            {
                cursor + glm::vec2{ 2.f, 8.f },
                cursor + glm::vec2{ 11.f, 8.f },
            },
            {
                cursor + glm::vec2{ 2.f, 10.f },
                cursor + glm::vec2{ 11.f, 10.f },
            },
        });
}

}  // namespace bt