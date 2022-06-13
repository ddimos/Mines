#pragma once
#include <SFML/Network.hpp>
#include "NetworkAddress.h"
#include <vector>

class Transport
{
public:
    void Send(sf::Packet _packet, NetworkAddress _address);
    NetworkAddress GetLocalAddress() const { return m_localAddress; }
    NetworkAddress GetPublicAddress() const { return m_publicAddress; }

protected:

    Transport();
    virtual ~Transport() = default;

    void Update();

    virtual void OnReceivePacket(sf::Packet _packet, NetworkAddress _sender) = 0;

private:
    void createHost();

    sf::UdpSocket m_localSocket;
    NetworkAddress m_localAddress = {};
    NetworkAddress m_publicAddress = {};
};