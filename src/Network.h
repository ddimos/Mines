#pragma once

#include <queue>
#include <utility>
#include <SFML/Network.hpp>

struct NetworkAddress
{
    sf::IpAddress   address = {};
    unsigned short  port = 0;
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

    void Send(sf::Packet _packet, NetworkAddress _address);
    void Send(sf::Packet _packet); // Brodcast


private:

    void createHost();

    static Network* ms_network;
    sf::UdpSocket m_localSocket;

    NetworkAddress m_localAddress = {};
    NetworkAddress m_others = {};
    
    std::queue<NetEvent> m_events;
};

