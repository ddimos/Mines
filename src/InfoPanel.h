#pragma once
#include "Utils/Utils.h"
#include "PlayerInfo.h"
#include "GameListener.h"

class InfoPanel : public GameListener
{
public:
    InfoPanel();
    ~InfoPanel();

    void Update(float _dt);
    void Draw(sf::RenderWindow& _window);

    void OnInit();

   // void OnGameStart(int _bombsNum);
   // void OnGameFinish(bool _isVictory);

   /*?*/ void OnCharachterSpawned(const PlayerInfo& _info);

    void OnTextEntered(sf::Uint32 _char);
    void OnFlagSet();
    void OnFlagUnset();

private:
    void onPlayerJoined(const PlayerInfo& _info) override;
    void onPlayerLeft(const PlayerInfo& _info) override;

    void updateBombsLeftText(int _newNumber);
    
    sf::Sprite m_helpersSprite;
    sf::Text m_bombsNumText;

    bool m_isMaster = true;

    int m_bombsLeft = 0;

    struct PlayerText
    {
        sf::Text text;
        PlayerInfo info;
        bool isCharacterSpawned = false;
        sf::RectangleShape shape;
    };
    std::vector<PlayerText> m_players;
};

