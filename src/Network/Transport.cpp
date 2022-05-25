#include "Transport.h"
#include "Log.h"
#include <algorithm>

Transport::Transport()
{
    createHost();
}

void Transport::Update()
{
    while (true)
    {
        sf::Packet packet;
        NetworkAddress sender;
        const sf::Socket::Status status = m_localSocket.receive(packet, sender.address, sender.port);

        if (status == sf::Socket::Status::NotReady)
        {
            break;
        }
        else if(status == sf::Socket::Status::Done)
        {
            LOG("RECEIVE");
            OnReceivePacket(packet, sender);
            // for (auto receiver : m_receivers)
            //     if (receiver.from == sender)    // TODO: support receiving packets from all??
            //         receiver.receiver->OnReceivePacket(packet, sender);
        }
        else
        {
            LOG_ERROR("The status of the socket: " + tstr(status));
            break;
        }       
    }

}

void Transport::Send(sf::Packet _packet, NetworkAddress _address)
{
    m_localSocket.send(_packet, _address.address, _address.port);
}
    
// void Transport::RegisterReceiver(IPacketReceiver* _receiver, NetworkAddress _receiveFrom)
// {
//     m_receivers.push_back({_receiveFrom, _receiver});
// }

// void Transport::UnregisterReceiver(IPacketReceiver* _receiver)
// {
//     m_receivers.erase(
//         std::remove_if(m_receivers.begin(), m_receivers.end(), 
//         [_receiver](const ReceiverPair& _pair)
//         {
//             return _pair.receiver == _receiver;
//         }));
// }

void Transport::createHost()
{
    if (m_localSocket.bind(sf::Socket::AnyPort) != sf::Socket::Done)
    {
        LOG_ERROR("Couldn't bind a port");
        return;
    }
    m_localSocket.setBlocking(false);

    m_localAddress.address = sf::IpAddress::getLocalAddress();
    m_localAddress.port = m_localSocket.getLocalPort();

    LOG("\n\tHost created on a port:\t" + tstr(m_localSocket.getLocalPort()) 
     + "\n\tLocal address:\t\t" +  sf::IpAddress::getLocalAddress().toString()
     + "\n\tGlobal:\t\t\t" + sf::IpAddress::getPublicAddress().toString());
}