#pragma once
#include "Utils.h"
#include "PlayerInfo.h"

class InfoPanel
{
public:
    InfoPanel();
    ~InfoPanel();

    void Update(float _dt);
    void Render(sf::RenderWindow& _window);

    void OnInit(const sf::Font& _font);

    void OnEnterInit();
    void OnEnterLobby(bool _isMaster);
    void OnGameStart(int _bombsNum);
    void OnGameFinish();
    void OnPlayerJoined(const PlayerInfo& _info);
    void OnPlayerLeft(const PlayerInfo& _info);

    void OnTextEntered(const std::string& _text);
    void OnFlagSet();
    void OnFlagUnset();

private:

    void updateBombsLeftText(int _newNumber);
    
    sf::Text m_text;
    sf::Text m_enteredText;
    sf::Text m_bombsNumText;

    bool m_isMaster = true;

    int m_bombsLeft = 0;

    struct PlayerText
    {
        sf::Text text;
        PlayerInfo info;
    };
    std::vector<PlayerText> m_players;
};

