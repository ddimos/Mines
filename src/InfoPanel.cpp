#include "InfoPanel.h"
#include "Game.h"
#include "Utils/Log.h"
#include "Utils/Utils.h"
#include "Utils/ResourceManager.h"
#include <string>

InfoPanel::InfoPanel()
{
    m_bombsNumText.setPosition(sf::Vector2f{20.f, 320.f});
    m_bombsNumText.setFillColor(sf::Color::White);

    m_helpersSprite.setTexture(ResourceManager::getTexture("helpers"));
    m_helpersSprite.setPosition(sf::Vector2f{20.f, 20.f});
    m_helpersSprite.setScale(sf::Vector2f{0.5f, 0.5f});
}

void InfoPanel::updateBombsLeftText(int _newNumber)
{
    m_bombsLeft = _newNumber;
    m_bombsNumText.setString("Bombs Left\n" + std::to_string(_newNumber));
}

void InfoPanel::OnInit()
{
    const auto& font = ResourceManager::getFont("poppins_regular");
    m_bombsNumText.setFont(font);
    updateBombsLeftText(0);

    Game::Get().RegisterGameListener(this);
}

void InfoPanel::OnFlagSet()
{
    updateBombsLeftText(m_bombsLeft - 1);
}

void InfoPanel::OnFlagUnset()
{
    updateBombsLeftText(m_bombsLeft + 1);
}


// void InfoPanel::OnGameFinish(bool _isVictory)
// {
//     sf::String text1 = _isVictory ? ":)" : ":(" ;
//     m_welcomeText.setString(text1);
//     sf::String text2 = m_isMaster
//                 ? "To restart press R!!"
//                 : "Wait for the host\nto restart"; // TODO \n\nOr press R\nto go to Menu";
//     m_text.setString(text2);
//     m_bombsNumText.setString("");
// }

void InfoPanel::onPlayerJoined(const PlayerInfo& _info)
{
    PlayerText player;
    player.info = _info;
    player.text.setFont(ResourceManager::getFont("poppins_regular"));
    player.text.setString(_info.networkPlayerCopy.GetName());
    player.shape.setSize(sf::Vector2f{CELL_SIZE/1.3f, CELL_SIZE/1.3f});
    player.shape.setOutlineColor(sf::Color::White);
    player.shape.setOutlineThickness(1.5f);

    const float posY = 450.f + m_players.size() * 40.f;
    player.shape.setPosition(sf::Vector2f{10.f, posY + 10.f});
    player.text.setPosition(sf::Vector2f{50.f, posY});
    m_players.emplace_back(std::move(player));
}

void InfoPanel::onPlayerLeft(const PlayerInfo& _info)
{
    m_players.erase(std::find_if(m_players.begin(), m_players.end(),
      [&_info](const PlayerText& _p){ return _p.info.networkPlayerCopy.GetPlayerId() == _info.networkPlayerCopy.GetPlayerId(); }),
      m_players.end());
}

void InfoPanel::onGameStart(const WorldConfig& _worldConfig)
{
    updateBombsLeftText(_worldConfig.bombsCount);
}

void InfoPanel::onPlayerInfoUpdated(const PlayerInfo& _info)
{
    auto it = std::find_if(m_players.begin(), m_players.end(),
      [&_info](const PlayerText& _p){ return _p.info.networkPlayerCopy.GetPlayerId() == _info.networkPlayerCopy.GetPlayerId(); });
    if (it == m_players.end())
    {
        LOG_ERROR("No player info for this id: " + tstr(_info.networkPlayerCopy.GetPlayerId()));
        return;
    }
    it->isCharacterSpawned = true;
    
    it->shape.setFillColor(getColorById(_info.colorId));
}

void InfoPanel::Update(float _dt)
{
    (void)_dt;
}

void InfoPanel::Draw(sf::RenderWindow& _window)
{
    _window.draw(m_bombsNumText);
    _window.draw(m_helpersSprite);
    for (auto& player : m_players)
    {
        if (player.isCharacterSpawned)
            _window.draw(player.shape);
                
        _window.draw(player.text);
    }
}
    