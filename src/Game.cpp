#include "Game.h"
#include "Utils.h"
#include "Log.h"

Game* Game::ms_game = nullptr;
std::array<bool, sf::Keyboard::KeyCount> Game::ms_keysState;

Game::Game(sf::RenderWindow* _window)
:
    m_window(_window)
{
    m_seed = time(NULL);

    std::fill(ms_keysState.begin(), ms_keysState.end(), false);

    m_gameView.setViewport(sf::FloatRect(0.f, 0.f, 0.8f, 1.f));
    m_infoView.setViewport(sf::FloatRect(0.8f, 0.f, 0.8f, 1.f));

    loadResources();

    m_infoPanel.OnInit(m_font);
    // Create a preview
    m_gameWorld.CreateWorld({35, 35}, 1);

    m_wantsToChangeState = true;
    
    CreateHost();
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
    srand(m_seed);
    m_gameWorld.CreateWorld({CELL_COUNT, CELL_COUNT}, BOMBS_COUNT);
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
        m_infoPanel.OnGameInit();
        break;
    case GameState::GAME:
        initGame();
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
    case GameState::GAME_INIT:
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
        newState = GameState::GAME_INIT;
        break;
    case GameState::GAME_INIT:
        newState = GameState::GAME;
        break;
    case GameState::GAME:
        newState = GameState::FINISH;
        break;
    case GameState::FINISH:
        newState = GameState::GAME_INIT; // if wants to restart
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
    //m_wantsToChangeState = true;
    size_t index = m_enteredText.find(":");
    if (index == std::string::npos)
    {
        LOG_ERROR("Entered a bad address: " + m_enteredText);
        return;
    }

    m_isMasterSession = false;

    m_address = sf::IpAddress(m_enteredText.substr(0, index));
    m_port = std::stoi(m_enteredText.substr(index+1));

    sf::Packet packet;
    NetworkPacketType type = NetworkPacketType::CONNECT;
    packet << static_cast<sf::Uint16>(type);

    LOG("Type " + tstr(static_cast<sf::Uint16>(type)));
    Send(packet, m_address, m_port);
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
        sf::Packet packet;
        sf::IpAddress sender;
        unsigned short port;
        const sf::Socket::Status status = m_localSocket.receive(packet, sender, port);

        if (status == sf::Socket::Status::NotReady)
        {
            break;
        }
        else if(status == sf::Socket::Status::Done)
        {
            LOG("Received from " + sender.toString() + " on port " + tstr(port) );
            sf::Uint16 type1;
            packet >> type1;
            NetworkPacketType type = static_cast<NetworkPacketType>(type1);
            if (type == NetworkPacketType::CONNECT)
            {
                m_address = sender;
                m_port = port;

                sf::Packet packet;
                NetworkPacketType type = NetworkPacketType::CREATE_GAME;
                packet << static_cast<sf::Uint16>(type);
                packet << m_seed;
                Send(packet, m_address, m_port);
                m_wantsToChangeState = true;
            }
            else if (type == NetworkPacketType::CREATE_GAME)
            {
                packet >> m_seed;
                LOG("Type " + tstr(type1) + " seed: " + tstr(m_seed));
                m_wantsToChangeState = true;

            }
            else if (type == NetworkPacketType::CREATE_CHARACTER)
            {
                m_gameWorld.OnSpawnCharacterPacketReceived(packet);
            }
            else if (type == NetworkPacketType::REPLICATE_CHARACTER_POS)
            {
                m_gameWorld.OnReplicateCharacterPacketReceived(packet);
            }
            else if (type == NetworkPacketType::REPLICATE_CHARACTER_UNCOVER)
            {
                m_gameWorld.OnReplicateUncoverCellPacketReceived(packet);
            }
            else if (type == NetworkPacketType::REPLICATE_CHARACTER_TOGGLE)
            {
                m_gameWorld.OnReplicateToggleFlagCellPacketReceived(packet);
            }
        }
        else
        {
            LOG_ERROR("The status of the socket: " + tstr(status));
            break;
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
        }
    }
    else if (m_currentState == GameState::GAME_INIT)
    {
        m_wantsToChangeState = true;
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

        if (isKeyPressed(sf::Keyboard::S))
            m_gameWorld.SpawnMasterCharacter();
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

void Game::CreateHost()
{
    if (m_localSocket.bind(sf::Socket::AnyPort) != sf::Socket::Done)
    {
        LOG_ERROR("Couldn't bind a port");
        return;
    }

        m_localSocket.setBlocking(false);

    LOG("\n\tHost created on a port:\t" + tstr(m_localSocket.getLocalPort()) 
     + "\n\tLocal address:\t\t" +  sf::IpAddress::getLocalAddress().toString()
     + "\n\tGlobal:\t\t\t" + sf::IpAddress::getPublicAddress().toString());

}

void Game::Send(sf::Packet _packet, sf::IpAddress _address, unsigned short _port)
{
    m_localSocket.send(_packet, _address, _port);
}

void Game::Send(sf::Packet _packet)
{
    m_localSocket.send(_packet, m_address, m_port);
}