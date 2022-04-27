#include "Game.h"
#include "Utils.h"
#include "Log.h"
#include "Network.h"

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
    if (m_isMasterSession)
        m_seed = time(NULL);
    WorldPosition worldSize = {CELL_COUNT, CELL_COUNT};
    LOG("Create a game world. Seed: " + tstr(m_seed) + " X: " + tstr(worldSize.x) + " Y: " + tstr(worldSize.y) + " Bombs: " + tstr(BOMBS_COUNT));
    srand(m_seed);
    m_gameWorld.CreateWorld(worldSize, BOMBS_COUNT);
}

void Game::spawnCharacters()
{
    if (!m_isMasterSession)
        return;

    unsigned id = m_gameWorld.GenerateId();
    m_gameWorld.SpawnCharacter(true, id);
    // sendSpawnRequest()
    // for(number of connected peers)
    unsigned id2 = m_gameWorld.GenerateId();
    m_gameWorld.SpawnCharacter(false, id2);

    sf::Packet packet;
    packet << static_cast<sf::Uint16>(NetworkPacketType::CREATE_CHARACTER);
    packet << false;
    packet << id;
    packet << true; // a master for this spesific peer
    packet << id2;
    // Position
    // Send to all peers
    Network::Get().Send(packet, m_otherPeers);

}

void Game::resetGame()
{
    m_gameWorld.CreateWorld({CELL_COUNT, CELL_COUNT}, BOMBS_COUNT);
    m_isGameEnded = false;
}
 
void Game::onStateEnter(GameState _newState)
{
    switch (_newState)
    {
    case GameState::INIT:
        // m_wantsToChangeState = true;
        m_infoPanel.OnEnterInit();
        break;
    case GameState::LOBBY:
        initGame();
        m_infoPanel.OnEnterLobby(m_isMasterSession );
        break;
    case GameState::GAME:
        if (m_isMasterSession)
            spawnCharacters();
        
        m_infoPanel.OnGameStart(BOMBS_COUNT);
        break;
    case GameState::FINISH:
        m_infoPanel.OnGameFinish();
        // Draw menu
        // m_wantsToChangeState = true;
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
        newState = GameState::LOBBY; // ?INIT // if wants to restart
        break;
    default:
        break;
    }

    onStateExit(m_currentState);
    m_currentState = newState;
    onStateEnter(m_currentState);
}

void Game::OnPlayerUncoverCell(WorldPosition _pos)
{
    m_gameWorld.OnPlayerUncoverCell(_pos);
}

void Game::OnPlayerToggleFlagCell(WorldPosition _pos)
{
    m_gameWorld.OnPlayerToggleFlagCell(_pos);

    if (m_gameWorld.getCell(_pos).m_state == Cell::State::FLAGGED)
        m_infoPanel.OnFlagSet();
    else if (m_gameWorld.getCell(_pos).m_state == Cell::State::COVERED)
        m_infoPanel.OnFlagUnset();
    
}

void Game::OnGameEnded(bool _isVictory)
{
    (void)_isVictory;
    m_isGameEnded = true;
}

void Game::OnStartButtonPressed()
{
    size_t index = m_enteredText.find(":");
    if (index == std::string::npos)
    {
        LOG("No entered text. Assume to be a master");
        return;
    }

    m_isMasterSession = false;

    NetworkAddress address;
    address.address = sf::IpAddress(m_enteredText.substr(0, index));
    address.port = std::stoi(m_enteredText.substr(index+1));
    m_otherPeers = address;

    sf::Packet packet;
    NetworkPacketType type = NetworkPacketType::JOIN_REQUEST;
    packet << static_cast<sf::Uint16>(type);

    Network::Get().Send(packet, address);
}

void Game::OnTextEntered(sf::Uint32 _char)
{
    if (m_currentState == GameState::INIT)
    {
        if (_char < 46 || _char > 58) // Only numbers and dots
            return;
        
        if (_char == 8) // Backspace
            m_enteredText.pop_back();
        else
            m_enteredText += _char;

        m_infoPanel.OnTextEntered(m_enteredText);
    }
}

void Game::Update(float _dt)
{
    while (true)
    {
        NetEvent event;
        if (!Network::Get().PollEvents(event))
            break;

        switch (event.type)
        {
        case NetEvent::Type::ON_CONNECT:    // TODO: implement
        case NetEvent::Type::ON_DISCONNECT:
            break;
        case NetEvent::Type::ON_RECEIVE:
        {
            sf::Uint16 type1;
            event.packet >> type1;
            NetworkPacketType type = static_cast<NetworkPacketType>(type1);   
            if (type == NetworkPacketType::JOIN_REQUEST)
            {
                LOG("JOIN_REQUEST");
                m_otherPeers = event.sender;
                // Send the accept only if we are in the lobby state
                sf::Packet packet;
                NetworkPacketType type = NetworkPacketType::JOIN_ACCEPT;
                packet << static_cast<sf::Uint16>(type);
                packet << m_seed;

                Network::Get().Send(packet, event.sender);
            }
            else if (type == NetworkPacketType::JOIN_ACCEPT)
            {
                LOG("JOIN_ACCEPT");

                event.packet >> m_seed;
                m_wantsToChangeState = true;
            }
            else if (type == NetworkPacketType::CREATE_CHARACTER)
            {
                LOG("CREATE_CHARACTER");
                while (!event.packet.endOfPacket())
                {
                    bool isMaster;
                    event.packet >> isMaster;
                    unsigned id;
                    event.packet >> id;
                    m_gameWorld.SpawnCharacter(isMaster, id);
                }
                
                m_wantsToChangeState = true;
            }
            else if (type == NetworkPacketType::REPLICATE_CHARACTER_POS)
            {
                m_gameWorld.OnReplicateCharacterPacketReceived(event.packet);
            }
            else if (type == NetworkPacketType::REPLICATE_CHARACTER_UNCOVER)
            {
                m_gameWorld.OnReplicateUncoverCellPacketReceived(event.packet);
            }
            else if (type == NetworkPacketType::REPLICATE_CHARACTER_TOGGLE)
            {
                m_gameWorld.OnReplicateToggleFlagCellPacketReceived(event.packet);
            }

            break;
        }
        }   
        
    }

    updateState();

    for (size_t i = 0; i < ms_keysState.size(); ++i)
        if (ms_keysState[i])
            if (!isKeyDown(static_cast<sf::Keyboard::Key>(i)))
                ms_keysState[i] = false;

    if (m_currentState == GameState::INIT)
    {
        if (isKeyPressed(sf::Keyboard::B))
        {
            OnStartButtonPressed();
            if (m_isMasterSession)
                m_wantsToChangeState = true;            
        }
    }
    else if (m_currentState == GameState::LOBBY)
    {
        if (isKeyPressed(sf::Keyboard::B))
        {
            if (m_isMasterSession)
                m_wantsToChangeState = true;            
        }
    }
    else if (m_currentState == GameState::GAME)
    {
        m_gameWorld.Update(_dt);
        m_gameView.setCenter(m_gameWorld.GetCamera().GetPos());		
        m_gameView.setSize(m_gameWorld.GetCamera().GetSize());		
        
        if (m_isGameEnded)
        {
            m_wantsToChangeState = true;
        }

        // if (isKeyPressed(sf::Keyboard::S))
        //     m_gameWorld.SpawnMasterCharacter();
    }
    else if (m_currentState == GameState::FINISH)
    {
        if (isKeyPressed(sf::Keyboard::R))
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

