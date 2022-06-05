#include "InfoPanel.h"
#include "Game.h"
#include <string>

InfoPanel::InfoPanel(/* args */)
{
    m_text.setPosition(sf::Vector2f{20.f, 20.f});
    m_text.setFillColor(sf::Color::Magenta);

    m_bombsNumText.setPosition(sf::Vector2f{20.f, 200.f});
    m_bombsNumText.setFillColor(sf::Color::Magenta);

    m_enteredText.setPosition(sf::Vector2f{20.f, 250.f});
    m_enteredText.setFillColor(sf::Color::Magenta);
}

InfoPanel::~InfoPanel()
{
}

void InfoPanel::updateBombsLeftText(int _newNumber)
{
    m_bombsLeft = _newNumber;
    m_bombsNumText.setString("Bombs Left\n" + std::to_string(_newNumber));
}

void InfoPanel::OnInit(const sf::Font& _font)
{
    m_text.setFont(_font);
    m_bombsNumText.setFont(_font);
    m_enteredText.setFont(_font);
}

void InfoPanel::OnTextEntered(const std::string& _text)
{
    m_enteredText.setString(_text);
}

void InfoPanel::OnFlagSet()
{
    updateBombsLeftText(m_bombsLeft - 1);
}

void InfoPanel::OnFlagUnset()
{
    updateBombsLeftText(m_bombsLeft + 1);
}

void InfoPanel::OnEnterInit()
{
    m_text.setString("Welcome\n\nStart - B\n\nOr start entering \nthe address \nto connect");
}

void InfoPanel::OnEnterLobby(bool _isMaster)
{
    m_isMaster = _isMaster;
    sf::String text = _isMaster
                ? "Wait others\n\nStart - B\n"
                : "Wait others\n\nThe host\nwill start\n";
 
    m_text.setString(text);
    m_enteredText.setString("");
}

void InfoPanel::OnGameStart(int _bombsNum)
{
    updateBombsLeftText(_bombsNum);
    m_text.setString("PLAY!!\n\nMovement - Arrows\nUncover - Space\nFlag/Unflag - X");
    m_enteredText.setString("");
}

void InfoPanel::OnGameFinish()
{
    sf::String text = m_isMaster
                ? "To restart press R!!"
                : "Wait for the host\nto restart"; // TODO \n\nOr press R\nto go to Menu";
    m_text.setString(text);
    m_bombsNumText.setString("");
}

void InfoPanel::OnPlayerJoined(const PlayerInfo& _info)
{
    PlayerText player;
    player.info = _info;
    player.text.setFont(Game::Get().GetFont());
    player.text.setString(_info.name);
    player.text.setFillColor(_info.color);
    const float posY = 300.f + m_players.size() * 50.f;
    player.text.setPosition(sf::Vector2f{20.f, posY});
    m_players.emplace_back(std::move(player));
}

void InfoPanel::OnPlayerLeft(const PlayerInfo& _info)
{
    m_players.erase(std::find_if(m_players.begin(), m_players.end(),
      [&_info](const PlayerText& _p){ return _p.info.name == _info.name; }),
      m_players.end());
}

void InfoPanel::Update(float _dt)
{
(void)_dt;
}

void InfoPanel::Render(sf::RenderWindow& _window)
{
    _window.draw(m_text);
    _window.draw(m_bombsNumText);
    _window.draw(m_enteredText);
    for (auto& player : m_players)
        _window.draw(player.text);
}
    