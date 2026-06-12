#include "Components/InitialsInput.hpp"

#include "Commands/CallbackCommand.hpp"
#include "Components/LeaderBoard.hpp"
#include "Components/TextComponent.hpp"
#include "Constants.hpp"
#include "Events.hpp"
#include "GameObject.hpp"
#include "Locator.hpp"
#include "Services/EventManager.hpp"
#include "Services/InputManager.hpp"
#include "Services/ResourceManager.hpp"
#include "Services/SceneManager.hpp"
#include "Utils.hpp"

namespace gla
{
class Font;
}
namespace bt
{

InitialsInput::InitialsInput(
    gla::GameObject* pOwner, gla::TextComponent* first, gla::TextComponent* last, gla::GameObject* indicator, LeaderBoard* leaderBoard)
    : Component(pOwner)
    , m_pInfoText(pOwner->AddComponent<gla::TextComponent>(
          "INPUT INITIALS: ", gla::Locator::Get<gla::ResourceManager>().LoadFont("Fonts/nes.ttf", 8), layers::text))
    , m_pFirstText(first)
    , m_pLastText(last)
    , m_pIndicator(indicator)
    , m_pLeaderBoard(leaderBoard)
{
}

void InitialsInput::OnActivate()
{
    auto& inputManager = gla::Locator::Get<gla::InputManager>();
    inputManager.BindAction<gla::CallbackCommand>("pressUp"_h, 0, [this] -> void { CycleInitial(+1); });
    inputManager.BindAction<gla::CallbackCommand>("pressDown"_h, 0, [this] -> void { CycleInitial(-1); });
    inputManager.BindAction<gla::CallbackCommand>("pressRight"_h, 0, [this] -> void { SelectInitial(); });
    inputManager.BindAction<gla::CallbackCommand>("pressLeft"_h, 0, [this] -> void { SelectInitial(); });
    inputManager.BindAction<gla::CallbackCommand>("start"_h, 0, [this] -> void { SaveScoreAndRestart(); });
}

void InitialsInput::OnDeactivate()
{
    auto& inputManager = gla::Locator::Get<gla::InputManager>();
    inputManager.UnbindAction("pressUp"_h, 0);
    inputManager.UnbindAction("pressDown"_h, 0);
    inputManager.UnbindAction("pressRight"_h, 0);
    inputManager.UnbindAction("pressLeft"_h, 0);
    inputManager.UnbindAction("start"_h, 0);
}

void InitialsInput::CycleInitial(int count)
{
    char& current = m_firstSelected ? m_first : m_last;

    int newChar = current + count;

    if (newChar > 'Z')
        newChar = 'A';
    else if (newChar < 'A')
        newChar = 'Z';

    current = static_cast<char>(newChar);
    UpdateText();
}

void InitialsInput::SelectInitial()
{
    m_firstSelected = not m_firstSelected;
    m_pIndicator->GetTransform().ChangeLocalPosition({ m_firstSelected ? -8.f : 8.f, 0.f });
}

void InitialsInput::UpdateText() const
{
    if (m_firstSelected)
        m_pFirstText->SetText(std::string{ m_first });
    else
        m_pLastText->SetText(std::string{ m_last });
}

void InitialsInput::SaveScoreAndRestart() const
{
    m_pLeaderBoard->SaveHighScoreData({ m_first, m_last });
    gla::Locator::Get<gla::EventManager>().InvokeEvent(gla::Event{"Restart"_h});
}

}  // namespace bt
