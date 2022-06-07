#pragma once
#include "Utils.h"
#include "PlayerInfo.h"

class InputField
{
public:
    enum class EnterMode
    {
        MENU,
        ENTER_ADDRESS,
        ENTER_NAME
    };

    InputField();
    void OnInit(const sf::Font& _font);
    void OnTextEntered(sf::Uint32 _char);

    void Update();
    void Render(sf::RenderWindow& _window);

    EnterMode GetMode() const { return m_enterMode; }
    void SetMode(EnterMode _mode);

    const std::string& GetEnteredString() const { return m_enteredStr; }

private:
    sf::RectangleShape m_shape;
    sf::Text m_helpText;
    sf::Text m_enteredText;

    std::string m_enteredStr = "";

    EnterMode m_enterMode = EnterMode::MENU;
};

class InfoPanel
{
public:
    InfoPanel();
    ~InfoPanel();

    void Update(float _dt);
    void Render(sf::RenderWindow& _window);

    const std::string& GetEnteredAddress() const { return m_enteredAddress; }
    const std::string& GetEnteredName() const { return m_enteredName; }

    void OnInit(const sf::Font& _font);

    void OnEnterInit();
    void OnEnterLobby(bool _isMaster);
    void OnGameStart(int _bombsNum);
    void OnGameFinish(bool _isVictory);
    void OnPlayerJoined(const PlayerInfo& _info);
    void OnPlayerLeft(const PlayerInfo& _info);

    void OnTextEntered(sf::Uint32 _char);
    void OnFlagSet();
    void OnFlagUnset();

private:
    void updateBombsLeftText(int _newNumber);
    
    sf::Text m_welcomeText;
    sf::Text m_text;
    sf::Text m_enteredAddressText;
    sf::Text m_enteredNameText;
    sf::Text m_bombsNumText;

    bool m_isMaster = true;

    int m_bombsLeft = 0;

    struct PlayerText
    {
        sf::Text text;
        PlayerInfo info;
    };
    std::vector<PlayerText> m_players;

    std::string m_enteredAddress;
    std::string m_enteredName;

    InputField m_inputField;
};

