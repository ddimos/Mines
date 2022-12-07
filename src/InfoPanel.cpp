#include "InfoPanel.h"
#include "Game.h"
#include "Log.h"
#include <string>

namespace
{
    const unsigned CHARACTER_SIZE_M = 25;
    const unsigned CHARACTER_SIZE_S = 20;
}
InputField::InputField()
{
    m_shape.setSize(sf::Vector2f(220.f, 80.f));
    m_shape.setPosition(sf::Vector2f(20.f, 400.f));
    m_shape.setFillColor(sf::Color::White);

    m_helpText.setPosition(sf::Vector2f{25.f, 400.f});
    m_helpText.setFillColor(sf::Color::Black);
    m_helpText.setCharacterSize(CHARACTER_SIZE_S);

    m_enteredText.setPosition(sf::Vector2f{25.f, 445.f});
    m_enteredText.setFillColor(sf::Color::Black);
    m_enteredText.setCharacterSize(CHARACTER_SIZE_M);
}

void InputField::OnInit(const sf::Font& _font)
{
    m_enteredText.setFont(_font);
    m_helpText.setFont(_font);
}

void InputField::OnTextEntered(sf::Uint32 _char)
{
    if (m_enterMode == EnterMode::MENU)
        return;

    if (_char == 8) // Backspace
    {
        if (!m_enteredStr.empty())
            m_enteredStr.pop_back();
    }
    else if (m_enterMode == EnterMode::ENTER_ADDRESS)
    {
        if ((_char >= 46 && _char <= 58)) // Only numbers, a colon, a dot
            m_enteredStr += _char;
        
    }
    else if (m_enterMode == EnterMode::ENTER_NAME)
        m_enteredStr += _char;
    
    m_enteredText.setString(m_enteredStr);
}

void InputField::SetMode(EnterMode _mode)
{
    m_enterMode = _mode;
    if (m_enterMode == EnterMode::ENTER_ADDRESS)
        m_helpText.setString("Enter the address\nin the format: 0.0.0.0:0");
    else if (m_enterMode == EnterMode::ENTER_NAME)
        m_helpText.setString("Enter your name");  
    
    m_enteredStr.clear();
    m_enteredText.setString(""); 
}

void InputField::Update()
{
    if (m_enterMode == EnterMode::MENU)
        return;

}

void InputField::Render(sf::RenderWindow& _window)
{
    if (m_enterMode == EnterMode::MENU)
        return;

    _window.draw(m_shape);
    _window.draw(m_enteredText);
    _window.draw(m_helpText);
}

InfoPanel::InfoPanel()
{
    const auto textColor = sf::Color(246, 244, 234); 
    m_welcomeText.setPosition(sf::Vector2f{20.f, 20.f});
    m_welcomeText.setFillColor(textColor);

    m_text.setPosition(sf::Vector2f{20.f, 80.f});
    m_text.setFillColor(textColor);
    m_text.setCharacterSize(CHARACTER_SIZE_M);

    m_bombsNumText.setPosition(sf::Vector2f{20.f, 220.f});
    m_bombsNumText.setFillColor(textColor);

    m_enteredAddressText.setPosition(sf::Vector2f{20.f, 240.f});
    m_enteredAddressText.setFillColor(textColor);
    m_enteredAddressText.setCharacterSize(CHARACTER_SIZE_M);
    m_enteredNameText.setPosition(sf::Vector2f{20.f, 300.f});
    m_enteredNameText.setFillColor(textColor);
    m_enteredNameText.setCharacterSize(CHARACTER_SIZE_M);
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
    m_inputField.OnInit(_font);

    m_welcomeText.setFont(_font);
    m_text.setFont(_font);
    m_bombsNumText.setFont(_font);
    m_enteredAddressText.setFont(_font);
    m_enteredNameText.setFont(_font);
}

void InfoPanel::OnTextEntered(sf::Uint32 _char)
{
    if (Game::Get().GetState() == GameState::INIT)
    {
        m_inputField.OnTextEntered(_char);
    }
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
    m_welcomeText.setString("Welcome");

    m_text.setString(
    "Start - B\n"
    "Enter the address - Q\n"
    "Enter the name - W\n");

    m_inputField.SetMode(InputField::EnterMode::MENU);
}

void InfoPanel::OnEnterLobby(bool _isMaster)
{
    m_welcomeText.setString("Wait others");

    m_isMaster = _isMaster;
    sf::String text = _isMaster
                ? "Start - B\n"
                : "The host\nwill start\n";
 
    m_text.setString(text);
    
    m_enteredAddressText.setString("");
    m_enteredNameText.setString("");
}

void InfoPanel::OnGameStart(int _bombsNum)
{
    updateBombsLeftText(_bombsNum);
    m_welcomeText.setString("PLAY!!");
    m_text.setString("Movement - Arrows\nUncover - Space\nFlag/Unflag - X");
}

void InfoPanel::OnGameFinish(bool _isVictory)
{
    sf::String text1 = _isVictory ? ":)" : ":(" ;
    m_welcomeText.setString(text1);
    sf::String text2 = m_isMaster
                ? "To restart press R!!"
                : "Wait for the host\nto restart"; // TODO \n\nOr press R\nto go to Menu";
    m_text.setString(text2);
    m_bombsNumText.setString("");
}

void InfoPanel::OnPlayerJoined(const PlayerInfo& _info)
{
    PlayerText player;
    player.info = _info;
    player.text.setFont(Game::Get().GetFont());
    player.text.setString(_info.networkPlayerCopy.GetName());
    player.shape.setSize(sf::Vector2f{CELL_SIZE/1.3f, CELL_SIZE/1.3f});
    player.shape.setOutlineColor(sf::Color::White);
    player.shape.setOutlineThickness(1.5f);
    
    const float posY = 400.f + m_players.size() * 40.f;
    player.shape.setPosition(sf::Vector2f{10.f, posY + 10.f});
    player.text.setPosition(sf::Vector2f{50.f, posY});
    m_players.emplace_back(std::move(player));
}

void InfoPanel::OnPlayerLeft(const PlayerInfo& _info)
{
    m_players.erase(std::find_if(m_players.begin(), m_players.end(),
      [&_info](const PlayerText& _p){ return _p.info.networkPlayerCopy.GetPlayerId() == _info.networkPlayerCopy.GetPlayerId(); }),
      m_players.end());
}

void InfoPanel::OnCharachterSpawned(const PlayerInfo& _info)
{
    auto it = std::find_if(m_players.begin(), m_players.end(),
      [&_info](const PlayerText& _p){ return _p.info.networkPlayerCopy.GetPlayerId() == _info.networkPlayerCopy.GetPlayerId(); });
    if (it == m_players.end())
    {
        LOG_ERROR("No player info for this id: " + tstr(_info.networkPlayerCopy.GetPlayerId()));
        return;
    }
    it->isCharacterSpawned = true;
    it->shape.setFillColor(_info.charInfoCopy.color);
    it->info.charInfoCopy = _info.charInfoCopy;
}

void InfoPanel::Update(float _dt)
{
    (void)_dt;

    if (Game::Get().GetState() == GameState::INIT)
    {
        if (m_inputField.GetMode() == InputField::EnterMode::MENU)
        {
            if (Game::isKeyPressed(sf::Keyboard::Q))
                m_inputField.SetMode(InputField::EnterMode::ENTER_ADDRESS);
            
            if (Game::isKeyPressed(sf::Keyboard::W))
                m_inputField.SetMode(InputField::EnterMode::ENTER_NAME);
        }
        else
        {
            if (Game::isKeyPressed(sf::Keyboard::Enter))
            {
                if (m_inputField.GetMode() == InputField::EnterMode::ENTER_ADDRESS)
                {
                    m_enteredAddress = m_inputField.GetEnteredString();
                    m_enteredAddressText.setString("Connect:\n" + m_enteredAddress);
                }
                else if (m_inputField.GetMode() == InputField::EnterMode::ENTER_NAME)
                {
                    m_enteredName = m_inputField.GetEnteredString();
                    m_enteredNameText.setString("Name:\n" + m_enteredName);

                }
                m_inputField.SetMode(InputField::EnterMode::MENU);            
            }
        }
    }

    m_inputField.Update();
}

void InfoPanel::Render(sf::RenderWindow& _window)
{
    _window.draw(m_welcomeText);
    _window.draw(m_text);
    _window.draw(m_bombsNumText);
    _window.draw(m_enteredAddressText);
    _window.draw(m_enteredNameText);
    for (auto& player : m_players)
    {
        if (player.isCharacterSpawned)
            _window.draw(player.shape);
                
        _window.draw(player.text);
    }

    m_inputField.Render(_window);    
}
    