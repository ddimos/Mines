#pragma once

#include "Cell.h"
#include "Character.h"
#include "Camera.h"
#include "InfoPanel.h"
#include "GameWorld.h"

#include <SFML/Network.hpp>

namespace sf
{
    class RenderWindow;
}

enum class GameState
{
    None,
    INIT,
    GAME,
    FINISH
};

class Game
{
public:
    static bool StartUp(sf::RenderWindow* _window);
    static bool ShutDown();
    static Game& Get() {return *ms_game; }

    Game(sf::RenderWindow* _window);
    ~Game();
    
    void Update(float _dt);
    void Draw(sf::RenderWindow* _window);

    void OnPlayerUncoverCell(WorldPosition _pos);
    void OnPlayerToggleFlagCell(WorldPosition _pos);
    void OnGameEnded(bool _isVictory);

    const GameWorld& GetGameWorld() const { return m_gameWorld; }
     
    // Resources
    const sf::Font& GetFont() const { return m_font; }

private:
    sf::Font m_font;

private:

    void initGame();
    void loadResources();
    void resetGame();

    void onStateEnter(GameState _newState);
    void onStateExit(GameState _oldState);
    void updateState();

    // Network
    void CreateHost();
    
    sf::UdpSocket m_localSocket;    

    static Game* ms_game;
    sf::RenderWindow* m_window = nullptr;

    InfoPanel m_infoPanel;
    GameWorld m_gameWorld;
    sf::View m_gameView;
    sf::View m_infoView;


    bool m_isGameEnded = false;

    // State machine
    GameState m_currentState = GameState::None;
    bool m_wantsToChangeState = false;
    
    bool m_isNetwork = false;

public:
    // Keybord
    static bool isKeyPressed(sf::Keyboard::Key _key); // returns true only once 
    static bool isKeyDown(sf::Keyboard::Key _key);    // returns true whole time until a key is released
private:
    static std::array<bool, sf::Keyboard::KeyCount> ms_keysState;
};