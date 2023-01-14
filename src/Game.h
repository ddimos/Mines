#pragma once

#include "Cell.h"
#include "Character.h"
#include "Camera.h"
#include "InfoPanel.h"
#include "GameWorld.h"
#include "PlayerInfo.h"
#include "Menu/MenuManager.h"

namespace sf
{
    class RenderWindow;
}

enum class GameState
{
    None,
    INIT,
    LOBBY,
    GAME,
    FINISH
};

class Game
{
public:
    static bool StartUp(sf::RenderWindow& _window);
    static bool ShutDown();
    static Game& Get() {return *ms_game; }

    Game(sf::RenderWindow& _window);
    ~Game();

    void Init();
    
    void Update(float _dt);
    void Draw(sf::RenderWindow& _window);

    void OnCharacterUncoverCell(WorldPosition _pos, Character& _char);
    void OnCharacterToggleFlagCell(WorldPosition _pos, Character& _char);
    void OnGameEnded(bool _isVictory);

    void OnStartButtonPressed();

    void OnTextEntered(sf::Uint32 _char);

    const sf::RenderWindow& GetWindow() const { return m_window; } 
    const GameWorld& GetGameWorld() const { return m_gameWorld; }
    GameState GetState() const { return m_currentState; }

    PlayerInfo* GetPlayerInfo(PlayerID _playerId);

    bool IsSessionMaster() const;

private:

    void initGame();
    void spawnCharacters();
    void resetGame();

    void onStateEnter(GameState _newState);
    void onStateExit(GameState _oldState);
    void updateState();
    void receiveNetworkMessages();
    void sendCreateGameMessage();

    static Game* ms_game;
    sf::RenderWindow& m_window;

    InfoPanel m_infoPanel;
    GameWorld m_gameWorld;
    sf::View m_gameView;
    sf::View m_infoView;

    bool m_isGameEnded = false;
    bool m_isVictory = false;
    unsigned m_seed = 0;

    NetworkMessage m_messageWithPlayers;
    
    PlayerInfo m_localPlayerInfo;
    std::vector<PlayerInfo> m_players;

    // State machine
    GameState m_currentState = GameState::None;
    bool m_wantsToChangeState = false;

    MenuManager m_menuManager;
    
public:
    // Keybord
    static bool isKeyPressed(sf::Keyboard::Key _key); // returns true only once 
    static bool isKeyDown(sf::Keyboard::Key _key);    // returns true whole time until a key is released
private:
    static std::array<bool, sf::Keyboard::KeyCount> ms_keysState;
};