#pragma once

#include <queue>
#include <vector>
#include <utility>
#include <SFML/Network.hpp>

enum class InternalPacketType : sf::Int8
{
    INTERNAL_CONNECT_REQUEST,
    INTERNAL_CONNECT_ACCEPT,
    INTERNAL_DISCONNECT,
    INTERNAL_HEARTBEAT,
    USER_PACKET
};

struct NetworkAddress
{
    sf::IpAddress   address = {};
    unsigned short  port = 0;
};
inline bool operator==(const NetworkAddress& _lhs, const NetworkAddress& _rhs)
{
    return _lhs.address == _rhs.address && _lhs.port == _rhs.port;
}
inline bool operator!=(const NetworkAddress& _lhs, const NetworkAddress& _rhs)
{
    return !(_lhs == _rhs);
}

struct Peer
{
    enum class Status
    {
        None,
        CONNECTING,
        UP,
        DISCONNECTING,
        DOWN
    };

    NetworkAddress address = {};
    Status status = Status::None;
};

struct NetEvent
{
    enum class Type
    {
        ON_CONNECT,
        ON_RECEIVE,
        ON_DISCONNECT
    };
    NetEvent() = default;
    NetEvent(Type _type, const NetworkAddress& _sender)
    : type(_type), sender(_sender)
    {}
    NetEvent(Type _type, sf::Packet&& _packet, const NetworkAddress& _sender)
    : type(_type), packet(std::move(_packet)), sender(_sender)
    {}
    
    Type type;
    sf::Packet packet;
    NetworkAddress sender;
};
// TODO: Handle connection

class Network
{
public:
    static bool StartUp();
    static bool ShutDown();
    static Network& Get() {return *ms_network; }

    Network();
    ~Network();

    void Update(float _dt);

    bool PollEvents(NetEvent& _event);

    sf::Packet CreatePacket();

    void Send(sf::Packet _packet, NetworkAddress _address);
    void Send(sf::Packet _packet); // Brodcast

    void Connect(NetworkAddress _addressToConnect);
    void DisconnectMyself();    // PeerToPeer

    const std::vector<Peer>& GetPeers() const { return m_peers; }

private:

    void createHost();
    void internalConnect(NetworkAddress _addressToConnect, bool _isFirstConnect);

    static Network* ms_network;
    sf::UdpSocket m_localSocket;

    NetworkAddress m_localAddress = {};
    
    std::queue<NetEvent> m_events;
    std::vector<Peer>   m_peers;
};

