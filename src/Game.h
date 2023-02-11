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
    CREATE,
    JOIN,
    LOBBY,
    GAME,
    FINISH
};

struct MenuInputs
{
    std::string addressToConnect = "";
    WorldConfig worldConfig = {};// WorldConfigs::GetSmallWorld();
    ColorID playerColorId = ColorIdInvalid;
    std::string playerName = "";
};

struct GameResult
{
    std::string loserName = "";
    bool isVictory = false;
};

class GameListener;

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
    void OnGameEnded(bool _isVictory, PlayerID _loserId = PlayerIdInvalid);

    void OnStartMenuStartButtonPressed();   // not really happy abour the names
    void OnStartMenuJoinButtonPressed();
    void OnCreateMenuButtonPressed(const MenuInputs& _input);
    void OnJoinMenuButtonPressed(const MenuInputs& _input);
    void OnLobbyMenuButtonPressed();
    void OnFinishMenuStartAgainButtonPressed();
    void OnFinishMenuBackToMenuButtonPressed();
    // void OnStartButtonPressed();
    // void OnJoinButtonPressed();

    void OnTextEntered(sf::Uint32 _char);
    sf::Uint32 GetEnteredChar() const { return m_enteredChar; }

    const sf::RenderWindow& GetWindow() const { return m_window; } 
    const GameWorld& GetGameWorld() const { return m_gameWorld; }
    GameState GetState() const { return m_currentState; }

    PlayerInfo* GetPlayerInfo(PlayerID _playerId);
    const std::vector<PlayerInfo>& GetPlayers() const { return m_players; }

    bool IsSessionMaster() const;

    const GameResult& GetGameResult() const { return m_gameResult; }

    void RegisterGameListener(GameListener* _listener);
    void UnregisterGameListener(GameListener* _listener);

private:

    void initGame();
    void spawnCharacters();
    void resetGame();
    bool updatePlayerColor(PlayerID _playerId, ColorID _colorId);
    void requestLocalPlayerColorUpdate(ColorID _colorId);

    void onStateEnter(GameState _newState);
    void onStateExit(GameState _oldState);
    void updateState();
    void receiveNetworkMessages();
    void sendCreateGameMessage();
    void notifyGameListeners(std::function<void(GameListener*)> _callback);

    static Game* ms_game;
    sf::RenderWindow& m_window;

    InfoPanel m_infoPanel;
    GameWorld m_gameWorld;
    sf::View m_gameView;
    sf::View m_infoView;

    sf::Uint32 m_enteredChar = 0;

    bool m_isGameEnded = false;
    GameResult m_gameResult = {};
    unsigned m_seed = 0;
    WorldConfig m_worldConfig = {};

    bool m_wantToStartGame = false;
    bool m_needsToUpdateColor = false;
    
    PlayerID m_localPlayerId = PlayerIdInvalid;
    std::vector<PlayerInfo> m_players;

    bool m_isAssumedToBeAHost = false;
    bool m_isJoiningOrJoined = false;

    // State machine
    GameState m_currentState = GameState::None;
    bool m_wantsToChangeState = false;
    bool m_wantsToReturnToMenu = false;

    MenuManager m_menuManager;
    MenuInputs m_menuInputs;

    std::vector<GameListener*> m_gameListeners;
    
public:
    // Keybord
    static bool isKeyPressed(sf::Keyboard::Key _key); // returns true only once 
    static bool isKeyDown(sf::Keyboard::Key _key);    // returns true whole time until a key is released
private:
    static std::array<bool, sf::Keyboard::KeyCount> ms_keysState;
};