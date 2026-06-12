#include "Components/LeaderBoard.hpp"

#include <fstream>
#include <nlohmann/json.hpp>
#include <ranges>

#include "Components/TextComponent.hpp"
#include "Constants.hpp"
#include "Font.hpp"
#include "GameObject.hpp"

namespace vw = std::ranges::views;

template<>
struct nlohmann::adl_serializer<bt::Initials>
{
    static void to_json(json& j, bt::Initials const& init) { j = std::string{ init.first, init.last }; }

    static void from_json(json const& j, bt::Initials& init)
    {
        auto const s = j.get<std::string>();
        if (s.size() != 2)
        {
            throw std::runtime_error("Initials must be exactly 2 characters");
        }
        init.first = s.at(0);
        init.last = s.at(1);
    }
};  // namespace nlohmann

using namespace nlohmann;

namespace bt
{

LeaderBoard::LeaderBoard(gla::GameObject* pOwner, std::shared_ptr<gla::Font> const& font, int newScoreEntry)
    : Component(pOwner)
    , m_newScore(newScoreEntry)
    , m_pHeader(pOwner->AddComponent<gla::TextComponent>("LEADERBOARD", font, layers::text))
    , m_pRank1Text(pOwner->AddComponent<gla::TextComponent>("1. ", font, layers::text))
    , m_pRank2Text(pOwner->AddComponent<gla::TextComponent>("2. ", font, layers::text))
    , m_pRank3Text(pOwner->AddComponent<gla::TextComponent>("3. ", font, layers::text))
{
    LoadHighScoreData();
}

auto LeaderBoard::GetHighScore() const -> int
{
    if (m_highScores.empty())
        return 0;

    auto const it = m_highScores.rbegin();
    return it->first;
}

void LeaderBoard::OnActivate()
{
    m_pRank1Text->SetYOffset(20.f);
    m_pRank2Text->SetYOffset(36.f);
    m_pRank3Text->SetYOffset(52.f);

    SetLeaderBoardText();
}

void LeaderBoard::LoadHighScoreData()
{
    std::ifstream file(game::highScoreFile);

    if (not file.is_open())
        return;

    json const highScoreList = json::parse(file);

    if (not highScoreList.is_array())
        return;

    for (auto const& item : highScoreList)
    {
        //try
        //{
            auto const initial = item.at("initials").get<Initials>();
            auto const score = item.at("score").get<int>();
            m_highScores.insert({ score, initial });
        //}
        //catch (std::exception const&)
        //{
        //    std::println("Warning!\t{} contains an invalid entry, skipping entry", game::highScoreFile);
        //}
    }
}
void LeaderBoard::SetLeaderBoardText()
{
    auto it = m_highScores.rbegin();
    int count = 0;

    while (it != m_highScores.rend() && count < 3)
    {
        auto const score = it->first;
        auto const [first, last] = it->second;

        switch (count)
        {
            case 0:
                m_pRank1Text->SetText(std::format("1. {}{} - {}", first, last, score));
                break;
            case 1:
                m_pRank2Text->SetText(std::format("2. {}{} - {}", first, last, score));
                break;
            case 2:
                m_pRank3Text->SetText(std::format("3. {}{} - {}", first, last, score));
                break;
            default:;
        }
        ++count;
        ++it;
    }
}

void LeaderBoard::SaveHighScoreData(Initials initials)
{
    json jsonArray = json::array();

    m_highScores.insert({ m_newScore, initials });

    for (auto const& [score, initials] : m_highScores)
        jsonArray.push_back({ { "initials", initials }, { "score", score } });

    std::ofstream file(game::highScoreFile);
    file << jsonArray.dump(4);
}

}  // namespace bt