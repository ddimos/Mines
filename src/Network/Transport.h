#pragma once
#include <SFML/Network.hpp>
#include "NetworkAddress.h"
#include <vector>

// class IPacketReceiver
// {
// public:
//     ~IPacketReceiver() = default;
//     virtual void OnReceivePacket(sf::Packet _packet, NetworkAddress _sender) = 0;
// };

class Transport
{
public:
    void Send(sf::Packet _packet, NetworkAddress _address);

protected:

    Transport();
    virtual ~Transport() = default;

    void Update();

    virtual void OnReceivePacket(sf::Packet _packet, NetworkAddress _sender) {}
    // void RegisterReceiver(IPacketReceiver* _receiver, NetworkAddress _receiveFrom);
    // void UnregisterReceiver(IPacketReceiver* _receiver);

private:
    
    void createHost();
    sf::UdpSocket m_localSocket;
    NetworkAddress m_localAddress = {};
    // struct ReceiverPair
    // {
    //     NetworkAddress from = {};
    //     IPacketReceiver* receiver = nullptr;
    // };
    // std::vector<ReceiverPair> m_receivers;
};