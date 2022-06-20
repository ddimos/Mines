#pragma once 

#include <string>
#include <SFML/Config.hpp>

// typedef sf::Uint16 PlayerID
using PlayerID = sf::Uint16;
const PlayerID PlayerIdInvalid = 65535; 

class Network;

class NetworkPlayer
{
public:
    NetworkPlayer() = default;
    NetworkPlayer(const std::string& _name, PlayerID _id, bool _isLocal);

private:
    friend class Network;
    
    std::string m_name = "Player";
    PlayerID m_playerId = PlayerIdInvalid;
    bool m_isLocalPlayer = false;
    // peerid or a ptr
};
