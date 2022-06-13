#include "Transport.h"
#include "Log.h"
#include <algorithm>

Transport::Transport(unsigned short _port)
{
    createHost(_port);
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
            OnReceivePacket(packet, sender);
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

void Transport::createHost(unsigned short _port)
{
    int attemptsLeft = 20;
    while (m_localSocket.bind(_port) != sf::Socket::Done || attemptsLeft < 0)
    {
        ++_port;
        --attemptsLeft;
    }
    
    if (attemptsLeft < 0)
    {
        LOG_ERROR("Couldn't bind a port");
        return;
    }

    m_localSocket.setBlocking(false);

    m_localAddress.address = sf::IpAddress::getLocalAddress();
    m_localAddress.port = m_localSocket.getLocalPort();
    m_publicAddress.address = sf::IpAddress::getPublicAddress();
    m_publicAddress.port = m_localSocket.getLocalPort();

    LOG("\n\tHost created on a port:\t" + tstr(m_localSocket.getLocalPort()) 
     + "\n\tLocal address:\t\t" +  sf::IpAddress::getLocalAddress().toString()
     + "\n\tGlobal:\t\t\t" + m_publicAddress.address.toString());
}