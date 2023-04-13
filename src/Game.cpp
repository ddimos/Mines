#include "Game.h"
#include "Utils/Utils.h"
#include "Utils/Config.h"
#include "Utils/Log.h"
#include "GameListener.h"
#include "ResourceManager.h"
#include "Network/Network.h"
#include "Network/NetworkPlayer.h"
#include "NetworkMessageType.h"

namespace
{
    std::string toString(GameState _state)
    {
        switch (_state)
        {
        case GameState::INIT:
            return "INIT";
        case GameState::CREATE:
            return "CREATE";
        case GameState::CONFIG_WORLD:
            return "CONFIG_WORLD";
        case GameState::JOIN:
            return "JOIN";
        case GameState::LOBBY:
            return "LOBBY";
        case GameState::GAME:
            return "GAME";
        case GameState::FINISH:
            return "FINISH";
        default:
            return "";
        }
    }
} // namespace


Game* Game::ms_game = nullptr;
std::array<bool, sf::Keyboard::KeyCount> Game::ms_keysState;

Game::Game(sf::RenderWindow& _window)
    : m_window(_window)
    , m_menuManager(_window)
{
    std::fill(ms_keysState.begin(), ms_keysState.end(), false);

    m_gameView.setViewport(sf::FloatRect(0.f,           0.f, WINDOWS_RATIO, 1.f));
    m_infoView.setViewport(sf::FloatRect(WINDOWS_RATIO, 0.f, 1.f-WINDOWS_RATIO, 1.f));
    m_gameView.setSize(GAME_FIELD_SIZE.x, GAME_FIELD_SIZE.y);
    m_infoView.setSize(INFO_FIELD_SIZE.x, INFO_FIELD_SIZE.y);
    m_infoView.setCenter({INFO_FIELD_SIZE.x / 2.f, INFO_FIELD_SIZE.y / 2.f});
    m_wantsToChangeState = true;

    m_background.setTexture(ResourceManager::getTexture("background"));
    m_background.setPosition({0.f, 0.f});
}

Game::~Game()
{
}

bool Game::StartUp(sf::RenderWindow& _window)
{
    if (!ms_game)
    {
        ms_game = new Game(_window);
        return true;
    }
    return false;
}
 
bool Game::ShutDown()
{
    if (!ms_game)
    {
        delete ms_game;
        ms_game = nullptr;
        return true;
    }
    return false;
}

void Game::Init()
{
    m_menuManager.OnInit();
    m_infoPanel.OnInit();
    m_statsController.OnInit();
    // Create a preview
     m_gameWorld.CreateWorld({{35, 35}, 1});
}

bool Game::isKeyPressed(sf::Keyboard::Key _key)
{
    if (!Game::isKeyDown(_key))
        return false;
    
    if (ms_keysState[_key])
        return false;

    ms_keysState[_key] = true;
    return true ;
}

bool Game::isKeyDown(sf::Keyboard::Key _key)
{
    return sf::Keyboard::isKeyPressed(_key) && Game::Get().m_window.hasFocus();
}

void Game::initGame()
{
    if (IsSessionMaster())
        m_seed = time(NULL);

    LOG("Create a game world. Seed: " + tstr(m_seed) + " X: " + tstr(m_worldConfig.worldSize.x)
                                                     + " Y: " + tstr(m_worldConfig.worldSize.y)
                                                     + " Bombs: " + tstr(m_worldConfig.bombsCount)
                                                     + " Mode: " + tstr((int)m_worldConfig.gameMode));
    provideSeed(m_seed);
    m_gameWorld.CreateWorld(m_worldConfig);
}

void Game::spawnCharacters()
{
    for (PlayerInfo& playerInfo : m_players)
    {
        CharacterInfo charInfo;
        charInfo.colorId = playerInfo.colorId;
        charInfo.characterId = playerInfo.networkPlayerCopy.GetPlayerId();
        charInfo.playerId = playerInfo.networkPlayerCopy.GetPlayerId();
        playerInfo.charInfoCopy = charInfo;
        m_gameWorld.SpawnCharacter(IsSessionMaster(), playerInfo.networkPlayerCopy.IsLocal(), charInfo);
    }
}


void Game::resetGame()
{
    m_gameWorld.DestroyWorld();
    m_gameResult = {};
}

bool Game::updatePlayerColor(PlayerID _playerId, ColorID _colorId)
{
    auto* playerInfo = GetPlayerInfo(_playerId);
    if (!playerInfo)
    {
        LOG_ERROR("Couldn't find a player with id: " + tstr(_playerId));
        return false;
    }

    playerInfo->colorId = _colorId;
    notifyGameListeners([&playerInfo](GameListener* _list) {
        _list->onPlayerInfoUpdated(*playerInfo);
    });
    return true;
}

void Game::requestLocalPlayerColorUpdate(ColorID _colorId)
{
    NetworkMessage message(Network::Get().GetHostPeerId(), true);
    message.Write(static_cast<sf::Uint16>(NetworkMessageType::REQUST_PLAYER_INFO_UPDATE));
    message.Write(static_cast<PlayerID>(m_localPlayerId));
    message.Write(static_cast<ColorID>(_colorId));
    Network::Get().Send(message);
}

void Game::onStateEnter(GameState _newState)
{
    switch (_newState)
    {
    case GameState::INIT:
        m_wantsToReturnToMenu = false;
        m_menuManager.Push(MenuType::START_MENU);
        break;
    case GameState::CREATE:
        m_menuManager.Push(MenuType::CREATE_MENU);
        break;
    case GameState::CONFIG_WORLD:
        m_menuManager.Push(MenuType::CONFIGURE_MENU);
        break;
    case GameState::JOIN:
        m_menuManager.Push(MenuType::JOIN_MENU);
        break;
    case GameState::LOBBY:
        m_wantsToGonfigWorld = false;
        m_menuManager.Push(MenuType::LOBBY_MENU);
        if (m_wantToStartGame)  // It means that we were in the Finish state when the host started a new game
            m_wantsToChangeState = true;

        if (m_needsToUpdateColor)
        {
            if (IsSessionMaster())
                updatePlayerColor(m_localPlayerId, m_menuInputs.playerColorId);
            else
                requestLocalPlayerColorUpdate( m_menuInputs.playerColorId);
            m_needsToUpdateColor = false;
        }
        break;
    case GameState::GAME:
        initGame();
        spawnCharacters();
        if (IsSessionMaster())
            sendCreateGameMessage();
        
        notifyGameListeners([this](GameListener* _list) {
                _list->onGameStart(m_worldConfig);
            });
        m_wantToStartGame = false;
        break;
    case GameState::FINISH:
        m_menuManager.Push(MenuType::FINISH_MENU);
        break;
    case GameState::None:
        break;
    default:
        break;
    }
}

void Game::onStateExit(GameState _oldState)
{
    m_menuManager.Pop();
    switch (_oldState)
    {
    case GameState::INIT:
    case GameState::CREATE:
    case GameState::CONFIG_WORLD:
    case GameState::JOIN:
    case GameState::LOBBY:
        break;
    case GameState::GAME:
        notifyGameListeners([&](GameListener* _list) {
            _list->onGameFinish(m_gameResult);
        });
        if (IsSessionMaster())
            sendFinishGameMessage();
        break;
    case GameState::FINISH:
        resetGame();
        break;
    case GameState::None:
        break;
    default:
        break;
    }
}

void Game::updateState()
{
    if (!m_wantsToChangeState)
        return;
    m_wantsToChangeState = false;
    GameState newState = GameState::None;
    switch (m_currentState)
    {
    case GameState::None:
        newState = GameState::INIT;
        break;
    case GameState::INIT:
        newState = m_isAssumedToBeAHost ? GameState::CREATE : GameState::JOIN;
        break;
    case GameState::CREATE:
        newState = GameState::CONFIG_WORLD;
        break;
    case GameState::CONFIG_WORLD:   //NOTE: I think I could merge Create and Join states/menu
    case GameState::JOIN:
        newState = GameState::LOBBY;
        break;
    case GameState::LOBBY:
        newState = m_wantsToGonfigWorld ? GameState::CONFIG_WORLD : GameState::GAME;
        break;
    case GameState::GAME:
        newState = GameState::FINISH;
        break;
    case GameState::FINISH:
        newState = m_wantsToReturnToMenu ? GameState::INIT : GameState::LOBBY;
        break;
    default:
        break;
    }

    onStateExit(m_currentState);
    LOG("Change the game state from: " + toString(m_currentState) + " to: "+ toString(newState));
    m_currentState = newState;
    onStateEnter(m_currentState);
}

void Game::receiveNetworkMessages()
{
    while (true)
    {
        NetworkEvent event;
        if (!Network::Get().PollEvents(event))
            break;

        switch (event.type)
        {
        case NetworkEvent::Type::ON_PLAYER_JOIN: 
        {
            LOG("ON_PLAYER_JOIN " + event.player.GetName());
            
            PlayerInfo playerInfo;  // Do I even need this playerInfo??
            playerInfo.networkPlayerCopy = event.player;
            m_players.push_back(playerInfo);
            notifyGameListeners([playerInfo](GameListener* _list) {
                _list->onPlayerJoined(playerInfo);
            });

            if (event.player.IsLocal())
            {
                m_localPlayerId = playerInfo.networkPlayerCopy.GetPlayerId();

                if (m_isJoiningOrJoined && (m_currentState == GameState::JOIN || m_currentState == GameState::CREATE))
                    m_wantsToChangeState = true;
            }

            if (IsSessionMaster() && !event.player.IsLocal())
            {
                NetworkMessage message(event.player.GetPeerId(), true);
                message.Write(static_cast<sf::Uint16>(NetworkMessageType::UPDATE_PLAYER_INFO));
                for (const auto& info : m_players)
                {
                    if (info.networkPlayerCopy.GetPlayerId() == playerInfo.networkPlayerCopy.GetPlayerId())
                        continue;

                    message.Write(static_cast<PlayerID>(info.networkPlayerCopy.GetPlayerId()));
                    message.Write(static_cast<ColorID>(info.colorId));
                }
                Network::Get().Send(message);
            }
            
            break;
        }
        case NetworkEvent::Type::ON_PLAYER_LEAVE:
        {
            LOG("ON_PLAYER_LEAVE " + event.player.GetName());

            if (event.player.IsLocal())
            {
                m_isJoiningOrJoined = false;
                m_localPlayerId = PlayerIdInvalid;
                m_wantsToReturnToMenu = true;
            }
            
            auto it = std::find_if(m_players.begin(), m_players.end(),
            [&event](const PlayerInfo& _p){ return _p.networkPlayerCopy.GetPlayerId() == event.player.GetPlayerId(); });
            notifyGameListeners([it](GameListener* _list) {
                _list->onPlayerLeft(*it);
            });
            // TODO when the host leaves return to the main menu
            // TODO session leave/join events
            m_players.erase(it, m_players.end());
            break;
        }
        case NetworkEvent::Type::ON_RECEIVE:
        {
            sf::Uint16 type1;
            event.message.Read(type1);
            NetworkMessageType type = static_cast<NetworkMessageType>(type1);   

            if (type == NetworkMessageType::CREATE_GAME)
            {
                LOG("CREATE_GAME");
                if (m_currentState != GameState::LOBBY && m_currentState != GameState::FINISH)
                {
                    LOG_ERROR("Cannot handle the CreateGame message in this state");
                    break;
                }
                
                event.message.Read(m_seed);
                event.message.Read<sf::Int32>(m_worldConfig.worldSize.x);
                event.message.Read<sf::Int32>(m_worldConfig.worldSize.y);
                event.message.Read<sf::Uint32>(m_worldConfig.bombsCount);
                sf::Uint8 mode;
                event.message.Read<sf::Uint8>(mode);
                m_worldConfig.gameMode = static_cast<GameMode>(mode);

                m_wantsToChangeState = true;
                m_wantToStartGame = true;
            }
            else if (type == NetworkMessageType::FINISH_GAME)
            {
                LOG("FINISH_GAME");

                if (m_currentState != GameState::GAME)
                {
                    LOG_ERROR("Cannot handle the FinishGame message in this state");
                    break;
                }

                event.message.Read<bool>(m_gameResult.isVictory);
                PlayerID playerId = PlayerIdInvalid;
                event.message.Read<PlayerID>(playerId);
                if (playerId != PlayerIdInvalid)
                    if (auto* playerInfo = GetPlayerInfo(playerId))
                        m_gameResult.loserPtr = playerInfo;

                PlayersStats stats;
                while(!event.message.IsEnd())
                {
                    PlayerID playerId = PlayerIdInvalid;
                    PlayerStats playerStats;
                    
                    event.message.Read(playerId);
                    event.message.Read<sf::Uint32>(playerStats.numberOfDeathsTotal);
                    event.message.Read<sf::Uint32>(playerStats.numberOfDeathsLastGame);
                    event.message.Read<sf::Uint32>(playerStats.numberOfFoundBombsTotal);
                    event.message.Read<sf::Uint32>(playerStats.numberOfFoundBombsLastGame);
                    stats.insert_or_assign(playerId, playerStats);
                }
                m_statsController.ProvideStatsFromHost(std::move(stats));

                m_wantsToChangeState = true;
            }
            else if (type == NetworkMessageType::REQUST_PLAYER_INFO_UPDATE)
            {
                LOG("REQUST_PLAYER_INFO_UPDATE");
                if (m_currentState != GameState::LOBBY)
                {
                    LOG_ERROR("Cannot handle the RequestPlayerInfoUpdate message in this state");
                    break;
                }
                if (!IsSessionMaster())
                {
                    LOG_ERROR("Cannot handle the RequestPlayerInfoUpdate message because it's not host");
                    break;
                }

                PlayerID playerId = PlayerIdInvalid;
                ColorID colorId = ColorIdInvalid;
                event.message.Read(playerId);
                event.message.Read(colorId);

                // TODO validate if the color is not taken, if it's, send a denied message 
                bool result = updatePlayerColor(playerId, colorId);
                if (!result)
                {
                    LOG_ERROR("Cannot update color " + tstr(colorId) + " for player " + tstr(playerId));
                    break;
                }
                NetworkMessage message(true);
                message.Write(static_cast<sf::Uint16>(NetworkMessageType::UPDATE_PLAYER_INFO));
                message.Write(static_cast<PlayerID>(playerId));
                message.Write(static_cast<ColorID>(colorId));
                Network::Get().Send(message);
            }
            else if (type == NetworkMessageType::UPDATE_PLAYER_INFO)
            {
                LOG("UPDATE_PLAYER_INFO");
                if (m_currentState != GameState::LOBBY && m_currentState != GameState::JOIN)
                {
                    LOG_ERROR("Cannot handle the UpdatePlayerInfo message in this state");
                    break;
                }

                while(!event.message.IsEnd())
                {
                    PlayerID playerId = PlayerIdInvalid;
                    ColorID colorId = ColorIdInvalid;
                    event.message.Read(playerId);
                    event.message.Read(colorId);

                    updatePlayerColor(playerId, colorId);
                }
            }
            else if (type == NetworkMessageType::REPLICATE_CHARACTER_CONTROLS)
            {
                if (m_currentState != GameState::GAME)
                {
                    LOG_ERROR("Cannot handle the ReplicateCharacterControls message in this state");
                    break;
                }
                m_gameWorld.OnReplicateCharacterControlsMessageReceived(event.message);
            }
            else if (type == NetworkMessageType::REPLICATE_CHARACTER_POS)
            {
                m_gameWorld.OnReplicateCharacterMessageReceived(event.message);
            }
            else if (type == NetworkMessageType::REPLICATE_CHARACTER_UNCOVER)
            {
                m_gameWorld.OnReplicateUncoverCellMessageReceived(event.message);
            }
            else if (type == NetworkMessageType::REPLICATE_CHARACTER_TOGGLE)
            {
                m_gameWorld.OnReplicateToggleFlagCellMessageReceived(event.message);
            }

            break;
        }
        }   
    }
}

void Game::sendCreateGameMessage() // TODO  join in progress
{
    NetworkMessage message(true);
    message.Write(static_cast<sf::Uint16>(NetworkMessageType::CREATE_GAME));
    message.Write(static_cast<sf::Uint32>(m_seed));
    message.Write(static_cast<sf::Int32>(m_worldConfig.worldSize.x));
    message.Write(static_cast<sf::Int32>(m_worldConfig.worldSize.y));
    message.Write(static_cast<sf::Uint32>(m_worldConfig.bombsCount));
    message.Write(static_cast<sf::Uint8>(m_worldConfig.gameMode));
    Network::Get().Send(message);
}

void Game::sendFinishGameMessage()
{
    NetworkMessage message(true);
    message.Write(static_cast<sf::Uint16>(NetworkMessageType::FINISH_GAME));
    message.Write(static_cast<bool>(m_gameResult.isVictory));
    PlayerID loserId = (m_gameResult.loserPtr) ? m_gameResult.loserPtr->networkPlayerCopy.GetPlayerId() : PlayerIdInvalid;
    message.Write(static_cast<PlayerID>(loserId));
    for(const auto&[id, stats] : m_statsController.GetPlayersStats())
    {
        message.Write(static_cast<PlayerID>(id));
        message.Write(static_cast<sf::Uint32>(stats.numberOfDeathsTotal));
        message.Write(static_cast<sf::Uint32>(stats.numberOfDeathsLastGame));
        message.Write(static_cast<sf::Uint32>(stats.numberOfFoundBombsTotal));
        message.Write(static_cast<sf::Uint32>(stats.numberOfFoundBombsLastGame));
    }
    Network::Get().Send(message);
}

void Game::OnCharacterToggleFlagCell(const Cell& _cell, const Character& _char)
{
    notifyGameListeners([&](GameListener* _list) {
        _list->onCharacterToggleFlagCell(_cell, _char);
    });
}

void Game::OnGameEnded(bool _isVictory, PlayerID _loserId)
{
    if (!IsSessionMaster())
        return;

    m_gameResult.isVictory = _isVictory;
    if (_loserId != PlayerIdInvalid)
        if (auto* playerInfo = GetPlayerInfo(_loserId))
            m_gameResult.loserPtr = playerInfo;

    m_wantsToChangeState = true;
}

void Game::OnCharacterExplode(const Character& _char)
{
    notifyGameListeners([&](GameListener* _list) {
        _list->onCharacterExplode(_char);
    });
}

void Game::OnStartMenuStartButtonPressed()
{
    if (m_currentState != GameState::INIT)
        return;

    m_wantsToChangeState = true;
    m_isAssumedToBeAHost = true;
}

void Game::OnStartMenuJoinButtonPressed()
{
    if (m_currentState != GameState::INIT)
        return;

    m_wantsToChangeState = true;
    m_isAssumedToBeAHost = false;    
}

void Game::OnCreateMenuButtonPressed(const MenuInputs& _input)
{
    if (m_currentState != GameState::CREATE || m_isJoiningOrJoined)
        return;

    m_menuInputs = _input;
    Network::Get().CreateAndJoinSession(m_menuInputs.playerName);   // TODO to pass a player descriptor 
    m_isJoiningOrJoined = true;
    m_needsToUpdateColor = true;
}

void Game::OnConfigureMenuButtonPressed(const MenuInputs& _input)
{
    if (m_currentState != GameState::CONFIG_WORLD)
        return;

    m_wantsToChangeState = true;
    if (_input.worldConfig.IsValid())
        m_worldConfig = _input.worldConfig;
    else
        m_worldConfig.ApplyDefaultSmallWorld();
}

void Game::OnJoinMenuButtonPressed(const MenuInputs& _input)
{
    // TODO handle a failure
    if (m_currentState != GameState::JOIN || m_isJoiningOrJoined)
        return;

    m_menuInputs = _input;

    size_t index = m_menuInputs.addressToConnect.find(":");
    if (index == std::string::npos)
    {
        LOG_ERROR("No entered address.");
        return;
    }
    NetworkAddress address;
    address.address = sf::IpAddress(m_menuInputs.addressToConnect.substr(0, index));
    address.port = std::stoi(m_menuInputs.addressToConnect.substr(index+1));

    Network::Get().JoinSession(address, m_menuInputs.playerName);
    m_isJoiningOrJoined = true;
    m_needsToUpdateColor = true;
}

void Game::OnLobbyMenuStartButtonPressed()
{
    if (m_currentState != GameState::LOBBY)
        return;

    m_wantsToChangeState = true;
}

void Game::OnLobbyMenuConfigButtonPressed()
{
    if (m_currentState != GameState::LOBBY)
        return;

    m_wantsToGonfigWorld = true;
    m_wantsToChangeState = true;
}

void Game::OnFinishMenuStartAgainButtonPressed()
{
    if (m_currentState != GameState::FINISH)
        return;

    m_wantsToChangeState = true;
}

void Game::OnFinishMenuBackToMenuButtonPressed()
{
    if (m_currentState != GameState::FINISH)
        return;

    Network::Get().LeaveSession();
    m_wantsToChangeState = true;
    m_wantsToReturnToMenu = true;
}


void Game::RegisterGameListener(GameListener* _listener)
{
    if (std::find_if(m_gameListeners.begin(), m_gameListeners.end(),
            [_listener](GameListener* _list){ return _list == _listener; })
        != m_gameListeners.end())
    {
        LOG_ERROR("Double registration of a game listener");
        return;
    }
    m_gameListeners.push_back(_listener);
}

void Game::UnregisterGameListener(GameListener* _listener)
{
    m_gameListeners.erase(
        std::remove_if(m_gameListeners.begin(), m_gameListeners.end(),
            [_listener](GameListener* _list){ return _list == _listener; }),
        m_gameListeners.end());
}

void Game::notifyGameListeners(std::function<void(GameListener*)> _callback)
{
    for(auto listener : m_gameListeners)
        _callback(listener);
}

void Game::OnTextEntered(sf::Uint32 _char)
{
    m_enteredChar = _char;
}

PlayerInfo* Game::GetPlayerInfo(PlayerID _playerId)
{
    auto it = std::find_if(m_players.begin(), m_players.end(),
        [_playerId](const PlayerInfo& _p){ return _p.networkPlayerCopy.GetPlayerId() == _playerId; });
    if (it == m_players.end())
        return nullptr;
    return &(*it);
}

bool Game::IsSessionMaster() const
{
    return Network::Get().IsSessionMaster();
}

void Game::Update(float _dt)
{
    receiveNetworkMessages();
    updateState();

    for (size_t i = 0; i < ms_keysState.size(); ++i)
        if (ms_keysState[i])
            if (!isKeyDown(static_cast<sf::Keyboard::Key>(i)))
                ms_keysState[i] = false;

    if (m_currentState == GameState::INIT)
    {
    }
    else if (m_currentState == GameState::LOBBY)
    {
#if 0    // TODO add a DEBUG define
        if (isKeyPressed(sf::Keyboard::S))
        {
            NetworkMessage message(true);
            NetworkMessageType type = NetworkMessageType::None;
            message.Write(static_cast<sf::Uint16>(type));
            
            Network::Get().Send(message);
        }
        if (isKeyPressed(sf::Keyboard::J))
            Network::Get().LeaveSession();
#endif   
    }
    else if (m_currentState == GameState::GAME)
    {
        m_gameWorld.Update(_dt);
        m_gameView.setCenter(m_gameWorld.GetCamera().GetPos());
    }

    m_menuManager.Update(_dt);
    m_infoPanel.Update(_dt);

    m_enteredChar = 0;
}

void Game::Draw(sf::RenderWindow& _window)
{
    _window.setView(m_gameView);
    m_gameWorld.Render(_window);
    
    _window.setView(m_infoView);
    m_infoPanel.Draw(_window);

    _window.setView(_window.getDefaultView());

    if (m_currentState == GameState::INIT 
     || m_currentState == GameState::JOIN
     || m_currentState == GameState::CREATE
     || m_currentState == GameState::CONFIG_WORLD)
        _window.draw(m_background);

    m_menuManager.Draw(_window);
}
