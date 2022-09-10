#pragma once 

#include <string>
#include <SFML/Config.hpp>

using PlayerID = sf::Uint16;
const PlayerID PlayerIdInvalid = 65535; 

class Network;
class Peer;

class NetworkPlayer
{
public:
    NetworkPlayer() = default;
    NetworkPlayer(const std::string& _name, PlayerID _id, bool _isLocal)
    : m_name(_name), m_id(_id), m_isLocalPlayer(_isLocal)
    {
        if (m_name.empty())
            m_name = "Player";
    }

    const std::string& GetName() const { return m_name; }
    PlayerID GetPlayerId() const { return m_id; }
    bool IsLocal() const { return m_isLocalPlayer; }

private:
    friend class Network;
    
    std::string m_name = "Player";
    PlayerID m_id = PlayerIdInvalid;
    bool m_isLocalPlayer = false;
    Peer* m_peer = nullptr;
    // peerid or a ptr // I guess I need this only on the host side
};
