#pragma once


#include <SFML/Network.hpp>

struct NetworkAddress
{
    sf::IpAddress   address = {};
    unsigned short  port = 0;
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


    void Send(sf::Packet _packet, NetworkAddress _address);
    // void Send(sf::Packet _packet); // Brodcast

private:

    void createHost();

    static Network* ms_network;
    sf::UdpSocket m_localSocket;

    NetworkAddress m_localAddress = {};
    
};

