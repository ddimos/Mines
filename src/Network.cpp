#include "Network.h"
#include "Utils.h"
#include "Log.h"

Network* Network::ms_network = nullptr;

Network::Network()
{
    createHost();
}

Network::~Network()
{
}

bool Network::StartUp()
{
    if (!ms_network)
    {
        ms_network = new Network();
        return true;
    }
    return false;
}
 
bool Network::ShutDown()
{
    if (!ms_network)
    {
        delete ms_network;
        ms_network = nullptr;
        return true;
    }
    return false;
}

void Network::createHost()
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

void Network::Update(float _dt)
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
            m_others.address = sender;
            m_others.port = port;
            m_events.push(NetEvent(NetEvent::Type::ON_RECEIVE, std::move(packet), NetworkAddress{sender, port}));
        }
        else
        {
            LOG_ERROR("The status of the socket: " + tstr(status));
            break;
        }       

    }
}

bool Network::PollEvents(NetEvent& _event)
{
    if (m_events.empty())
        return false; 
    
    _event = m_events.back();
    m_events.pop();

    return true;
}

void Network::Send(sf::Packet _packet, NetworkAddress _address)
{
    m_localSocket.send(_packet, _address.address, _address.port);
}

void Network::Send(sf::Packet _packet)
{
    m_localSocket.send(_packet, m_others.address, m_others.port);
}