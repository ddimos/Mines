#include "InfoPanel.h"
#include "Game.h"
#include "Cell.h"
#include "Utils/Log.h"
#include "Utils/Utils.h"
#include "Utils/ResourceManager.h"
#include <string>

namespace
{
    constexpr unsigned NUMBER_TEXT_FONT_SIZE = 24;
    constexpr unsigned PLAYER_TEXT_FONT_SIZE = 14;

    constexpr int CHOOSE_COLOR_BUTTON_SIZE = 35;

    const sf::Vector2f NUMBER_TEXT_POS{155.f, 297.f};
    
    constexpr float PLAYER_X = 30.f;
    constexpr float FIRST_PLAYER_Y = 420.f;
    constexpr float PLAYER_TEXT_OFFSET_X = 30.f;
    constexpr float DISTANCE_BETWEEN_PLAYERS_Y = 43.f;

    constexpr float PLAYER_SPRITE_SCALE = 0.55f;

    const sf::Color PLAYER_TEXT_COLOR(173, 173, 226);
}

InfoPanel::InfoPanel()
{
    const auto& fontB = ResourceManager::getFont("poppins_bold");   // TODO should be extra bold
    m_bombsNumText.setPosition(NUMBER_TEXT_POS);
    m_bombsNumText.setFillColor(sf::Color::White);
    m_bombsNumText.setCharacterSize(NUMBER_TEXT_FONT_SIZE);
    m_bombsNumText.setFont(fontB);
    
    updateBombsLeftText(0);

    m_infoPanelSprite.setTexture(ResourceManager::getTexture("info_panel"));
    m_infoPanelSprite.setPosition(sf::Vector2f{0.f, 0.f});
}   

void InfoPanel::updateBombsLeftText(int _newNumber)
{
    m_bombsLeft = _newNumber;
    m_bombsNumText.setString(std::to_string(_newNumber));
}

void InfoPanel::OnInit()
{
    Game::Get().RegisterGameListener(this);
}

void InfoPanel::onPlayerJoined(const PlayerInfo& _info)
{
    PlayerText player;
    player.info = _info;
    player.text.setFont(ResourceManager::getFont("poppins_regular"));
    player.text.setString(_info.networkPlayerCopy.GetName());
    player.text.setFillColor(PLAYER_TEXT_COLOR);
    player.text.setCharacterSize(PLAYER_TEXT_FONT_SIZE);

    const float posY = FIRST_PLAYER_Y + m_players.size() * DISTANCE_BETWEEN_PLAYERS_Y;
    player.sprite.setPosition(sf::Vector2f{PLAYER_X, posY});
    player.text.setPosition(sf::Vector2f{PLAYER_X + PLAYER_TEXT_OFFSET_X, posY});
    m_players.emplace_back(std::move(player));
}

void InfoPanel::onPlayerLeft(const PlayerInfo& _info)
{
    m_players.erase(std::find_if(m_players.begin(), m_players.end(),
      [&_info](const PlayerText& _p){ return _p.info.networkPlayerCopy.GetPlayerId() == _info.networkPlayerCopy.GetPlayerId(); }),
      m_players.end());
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

    it->sprite.setTexture(ResourceManager::getTexture("colors"));
    it->sprite.setTextureRect(sf::IntRect{
                (int)getTextureNumByColorId(_info.colorId) * CHOOSE_COLOR_BUTTON_SIZE,
                0,
                CHOOSE_COLOR_BUTTON_SIZE,
                CHOOSE_COLOR_BUTTON_SIZE});
    it->sprite.setScale(PLAYER_SPRITE_SCALE, PLAYER_SPRITE_SCALE);
    it->info = _info;
}

void InfoPanel::onGameStart(const WorldConfig& _worldConfig)
{
    updateBombsLeftText(_worldConfig.bombsCount);
    for (auto& player : m_players)
    {
        player.sprite.setTextureRect(sf::IntRect{
            (int)getTextureNumByColorId(player.info.colorId) * CHOOSE_COLOR_BUTTON_SIZE,
            0,
            CHOOSE_COLOR_BUTTON_SIZE,
            CHOOSE_COLOR_BUTTON_SIZE});
    }
}

void InfoPanel::onCharacterToggleFlagCell(const Cell& _cell, const Character& _char)
{
    (void)_char;
    int newBobmsLeft = _cell.IsFlagged() ? m_bombsLeft - 1 : m_bombsLeft + 1;
    updateBombsLeftText(newBobmsLeft);
}

void InfoPanel::onCharacterExplode(const Character& _char)
{
    updateBombsLeftText(m_bombsLeft - 1); 
    if (!_char.IsDead())
        return;

    auto it = std::find_if(m_players.begin(), m_players.end(),
      [&_char](const PlayerText& _p){ return _p.info.networkPlayerCopy.GetPlayerId() == _char.GetInfo().playerId; });
    if (it == m_players.end())
    {
        LOG_ERROR("No player info for this id: " + tstr(_char.GetInfo().playerId));
        return;
    }

    it->sprite.setTextureRect(sf::IntRect{
                (int)getTextureNumByColorId(it->info.colorId) * CHOOSE_COLOR_BUTTON_SIZE,
                3 * CHOOSE_COLOR_BUTTON_SIZE,
                CHOOSE_COLOR_BUTTON_SIZE,
                CHOOSE_COLOR_BUTTON_SIZE});
}

void InfoPanel::Update(float _dt)
{
    (void)_dt;
}

void InfoPanel::Draw(sf::RenderWindow& _window)
{
    _window.draw(m_infoPanelSprite);
    _window.draw(m_bombsNumText);
    for (auto& player : m_players)
    {
        _window.draw(player.sprite);        
        _window.draw(player.text);
    }
}
    