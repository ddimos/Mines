#pragma once
#include "Utils/Utils.h"
#include "PlayerInfo.h"
#include "GameListener.h"

class InfoPanel : public GameListener
{
public:
    InfoPanel();
    ~InfoPanel() = default;

    void Update(float _dt);
    void Draw(sf::RenderWindow& _window);

    void OnInit();

    void OnFlagSet();
    void OnFlagUnset();

private:
    void onPlayerJoined(const PlayerInfo& _info) override;
    void onPlayerLeft(const PlayerInfo& _info) override;
    void onPlayerInfoUpdated(const PlayerInfo&) override;
    void onGameStart(const WorldConfig& _worldConfig) override;
    void onCharacterToggleFlagCell(const Cell& _cell, const Character& _char) override;
    void onCharacterExplode(const Character& _char) override;

    void updateBombsLeftText(int _newNumber);
    
    sf::Sprite m_infoPanelSprite;
    sf::Text m_bombsNumText;

    bool m_isMaster = true;

    int m_bombsLeft = 0;

    struct PlayerText
    {
        sf::Text text;
        PlayerInfo info;
        sf::Sprite sprite;
    };
    std::vector<PlayerText> m_players;
};

