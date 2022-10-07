#include "Game.h"
#include "Utils.h"
#include "Config.h"
#include "Log.h"
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

Game::Game(sf::RenderWindow* _window)
:
    m_window(_window)
{

    std::fill(ms_keysState.begin(), ms_keysState.end(), false);

    m_gameView.setViewport(sf::FloatRect(0.f, 0.f, 0.8f, 1.f));
    m_infoView.setViewport(sf::FloatRect(0.8f, 0.f, 0.8f, 1.f));

    loadResources();

    m_infoPanel.OnInit(m_font);
    // Create a preview
    m_gameWorld.CreateWorld({35, 35}, 1);

    m_wantsToChangeState = true;
}

Game::~Game()
{
}

bool Game::StartUp(sf::RenderWindow* _window)
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
    return sf::Keyboard::isKeyPressed(_key) && Game::Get().m_window->hasFocus();
}

void Game::loadResources()
{
    if (!m_font.loadFromFile("res/fonts/times_new_roman.ttf"))
	{
		LOG_ERROR("Couldn't load the font");
	}
}

void Game::initGame()
{
    if (IsSessionMaster())
        m_seed = time(NULL);
    WorldPosition worldSize = {CELL_COUNT, CELL_COUNT};
    LOG("Create a game world. Seed: " + tstr(m_seed) + " X: " + tstr(worldSize.x) + " Y: " + tstr(worldSize.y) + " Bombs: " + tstr(BOMBS_COUNT));
    provideSeed(m_seed);
    m_gameWorld.CreateWorld(worldSize, BOMBS_COUNT);
}

void Game::spawnCharacters()
{
    if (!IsSessionMaster())
        return;

    for (PlayerInfo& playerInfo : m_players)
    {
        CharacterInfo charInfo;
        charInfo.color = m_gameWorld.GenerateColor();
        unsigned id = m_gameWorld.GenerateId();      // Maybe I can use a player id
        charInfo.playerId = playerInfo.networkPlayerCopy.GetPlayerId();
        playerInfo.charInfoCopy = charInfo;
        m_gameWorld.SpawnCharacter(true, playerInfo.networkPlayerCopy.IsLocal(), id, charInfo);   
        m_infoPanel.OnCharachterSpawned(playerInfo);
    }

    NetworkMessage message(true);
    message.Write(static_cast<sf::Uint16>(NetworkMessageType::CREATE_CHARACTER));
    for(const Character& ch : m_gameWorld.GetCharacters())
    {
        message.Write(ch.GetInfo().playerId);
        message.Write(ch.GetId());
        message.Write(ch.GetInfo().color.toInteger());
    }
    Network::Get().Send(message);
}

void Game::resetGame()
{
    m_gameWorld.DestroyWorld();
    m_isGameEnded = false;
}
 
void Game::onStateEnter(GameState _newState)
{
    switch (_newState)
    {
    case GameState::INIT:
        m_infoPanel.OnEnterInit();
        break;
    case GameState::LOBBY:
        m_infoPanel.OnEnterLobby(IsSessionMaster());
        break;
    case GameState::GAME:
        initGame();
        if (IsSessionMaster())
        {
            sendCreateGameMessage();
            spawnCharacters();
        }
        else
        {
            while (!m_messageWithPlayers.IsEnd())
            {
                PlayerID playerId;
                m_messageWithPlayers.Read(playerId);
                unsigned characterId;
                m_messageWithPlayers.Read(characterId);
                sf::Uint32 color;
                m_messageWithPlayers.Read(color);
                
                CharacterInfo charInfo;
                charInfo.color = sf::Color(color);
                charInfo.playerId = playerId;
                m_gameWorld.SpawnCharacter(false, m_localPlayerInfo.networkPlayerCopy.GetPlayerId() == playerId, characterId, charInfo);
                auto* playerInfo = GetPlayerInfo(playerId);
                playerInfo->charInfoCopy = charInfo;
                m_infoPanel.OnCharachterSpawned(*playerInfo);
            }
            m_messageWithPlayers = {};
        }
        
        m_infoPanel.OnGameStart(BOMBS_COUNT);
        break;
    case GameState::FINISH:
        m_infoPanel.OnGameFinish(m_isVictory);
        break;
    case GameState::None:
        break;
    default:
        break;
    }
}

void Game::onStateExit(GameState _oldState)
{
    switch (_oldState)
    {
    case GameState::INIT:
    case GameState::GAME:
    case GameState::LOBBY:
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
        newState = GameState::LOBBY;
        break;
    case GameState::LOBBY:
        newState = GameState::GAME;
        break;
    case GameState::GAME:
        newState = GameState::FINISH;
        break;
    case GameState::FINISH:
        newState = GameState::GAME;
        // TODO make an exit to main menu
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
            m_infoPanel.OnPlayerJoined(playerInfo);
            
            if (event.player.IsLocal())
            {
                m_localPlayerInfo = playerInfo;
                m_wantsToChangeState = true;
            }
            break;
        }
        case NetworkEvent::Type::ON_PLAYER_LEAVE:
        {
            LOG("ON_PLAYER_LEAVE " + event.player.GetName());

            auto it = std::find_if(m_players.begin(), m_players.end(),
            [&event](const PlayerInfo& _p){ return _p.networkPlayerCopy.GetPlayerId() == event.player.GetPlayerId(); });
            m_infoPanel.OnPlayerLeft(*it);

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
            }
            else if (type == NetworkMessageType::CREATE_CHARACTER)
            {
                LOG("CREATE_CHARACTER");
                if (m_currentState != GameState::LOBBY && m_currentState != GameState::FINISH)
                {
                    LOG_ERROR("Cannot handle the CreateCharachter message in this state");
                    break;
                }
                m_messageWithPlayers = event.message;
                m_wantsToChangeState = true;
            }
            else if (type == NetworkMessageType::REPLICATE_CHARACTER_CONTROLS)
            {
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
    Network::Get().Send(message);
}

void Game::OnCharacterUncoverCell(WorldPosition _pos, Character& _char)
{
    m_gameWorld.OnCharacterUncoverCell(_pos, _char);
}

void Game::OnCharacterToggleFlagCell(WorldPosition _pos, Character& _char)
{
    m_gameWorld.OnCharacterToggleFlagCell(_pos, _char);

    if (m_gameWorld.getCell(_pos).m_state == Cell::State::FLAGGED)
        m_infoPanel.OnFlagSet();
    else if (m_gameWorld.getCell(_pos).m_state == Cell::State::COVERED)
        m_infoPanel.OnFlagUnset();
    
}

void Game::OnGameEnded(bool _isVictory)
{
    m_isVictory = _isVictory;
    m_isGameEnded = true;
}

void Game::OnStartButtonPressed()
{
    std::string enteredText = m_infoPanel.GetEnteredAddress();
    size_t index = enteredText.find(":");
    if (index == std::string::npos)
    {
        Network::Get().CreateAndJoinSession(m_infoPanel.GetEnteredName());
        LOG("No entered text. Assume to be a master");
        return;
    }

    NetworkAddress address;
    address.address = sf::IpAddress(enteredText.substr(0, index));
    address.port = std::stoi(enteredText.substr(index+1));

    Network::Get().JoinSession(address, m_infoPanel.GetEnteredName());
}

void Game::OnTextEntered(sf::Uint32 _char)
{
    m_infoPanel.OnTextEntered(_char);
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
        if (isKeyPressed(sf::Keyboard::B) && !m_infoPanel.IsInInputMode())
        {
            OnStartButtonPressed();
            if (IsSessionMaster())
                m_wantsToChangeState = true;            
        }
    }
    else if (m_currentState == GameState::LOBBY)
    {
        if (isKeyPressed(sf::Keyboard::B))
        {
            if (IsSessionMaster())
                m_wantsToChangeState = true;            
        }
// #if DEBUG
        if (isKeyPressed(sf::Keyboard::S))
        {
            NetworkAddress address;
            address.address = sf::IpAddress::Broadcast;
            NetworkMessage message(address, true);
            NetworkMessageType type = NetworkMessageType::None;
            message.Write(static_cast<sf::Uint16>(type));
            
            Network::Get().Send(message);
        }
        if (isKeyPressed(sf::Keyboard::J))
        {
            NetworkAddress address;
            address.address = sf::IpAddress::Broadcast;
            // TODO Network::Get().LeaveSession;
        }
// #endif
        
    }
    else if (m_currentState == GameState::GAME)
    {
        m_gameWorld.Update(_dt);
        m_gameView.setCenter(m_gameWorld.GetCamera().GetPos());		
        m_gameView.setSize(m_gameWorld.GetCamera().GetSize());		
        
        if (m_isGameEnded)
            m_wantsToChangeState = true;
    }
    else if (m_currentState == GameState::FINISH)
    {
        if (isKeyPressed(sf::Keyboard::R))
            if (IsSessionMaster())
                m_wantsToChangeState = true;
    }

    m_infoPanel.Update(_dt);
}


void Game::Draw(sf::RenderWindow* _window)
{
    _window->setView(m_gameView);
    m_gameWorld.Render(*_window);

    _window->setView(m_infoView);
    m_infoPanel.Render(*_window);
}
