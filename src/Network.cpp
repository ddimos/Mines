#include "Network.h"
#include "Utils.h"
#include "Log.h"
#include <algorithm>

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

void Network::internalConnect(NetworkAddress _addressToConnect, bool _isFirstConnect)
{
    sf::Packet packet;
    packet << static_cast<sf::Int8>(InternalPacketType::INTERNAL_CONNECT_REQUEST);
    packet << _isFirstConnect;
    m_peers.push_back(Peer{_addressToConnect, Peer::Status::CONNECTING});
    Send(packet, _addressToConnect);
}

void Network::Update(float _dt)
{
    (void)_dt;
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
            auto it = std::find_if(m_peers.begin(), m_peers.end(), 
                [&sender](const Peer& _peer) { return _peer.address == sender; });
            sf::Int8 internalType;
            packet >> internalType;
            switch (static_cast<InternalPacketType>(internalType))
            {
            case InternalPacketType::INTERNAL_CONNECT_REQUEST:
            {
                if (it != m_peers.end())
                {
                    LOG_ERROR("CONNECT_REQUEST received again from " + sender.address.toString() + ":" + tstr(sender.port));
                    break;
                }
                LOG("CONNECT_REQUEST received from " + sender.address.toString() + ":" + tstr(sender.port));
                
                bool isFirstConnect;
                packet >> isFirstConnect;

                sf::Packet packetResp;
                packetResp << static_cast<sf::Int8>(InternalPacketType::INTERNAL_CONNECT_ACCEPT);
                if (isFirstConnect)
                {
                    for (const Peer& _peer : m_peers)
                    {
                        packetResp << _peer.address.address.toInteger();
                        packetResp << _peer.address.port;
                    }
                }
                
                Send(packetResp, sender);

                m_peers.push_back(Peer{sender, Peer::Status::UP});
                m_events.push(NetEvent(NetEvent::Type::ON_CONNECT, sender));
                break;
            }
            case InternalPacketType::INTERNAL_CONNECT_ACCEPT:
            {
                if (it == m_peers.end())
                {
                    LOG_ERROR("CONNECT_ACCEPT received from " + sender.address.toString() + ":" + tstr(sender.port) + "who we didn't ask");
                    break;
                }

                if (it->status != Peer::Status::CONNECTING)
                {
                    LOG_ERROR("The status of " + sender.address.toString() + ":" + tstr(sender.port) + " isn't CONNECTING");
                    break;
                }
                LOG("CONNECT_ACCEPT received from " + sender.address.toString() + ":" + tstr(sender.port));
                it->status = Peer::Status::UP;
                
                while (!packet.endOfPacket())
                {
                    sf::Uint32 intAddress;
                    unsigned short port;
                    packet >> intAddress;
                    packet >> port;
                    NetworkAddress addressToConnect{sf::IpAddress(intAddress), port};
                    auto peerIt = std::find_if(m_peers.begin(), m_peers.end(), 
                        [&addressToConnect](const Peer& _peer) { return _peer.address == addressToConnect; });
                    if (peerIt == m_peers.end())
                    {
                        internalConnect(addressToConnect, false);
                    }
                }

                m_events.push(NetEvent(NetEvent::Type::ON_CONNECT, sender));
                break;
            }
            case InternalPacketType::INTERNAL_DISCONNECT:
            case InternalPacketType::INTERNAL_HEARTBEAT:
                break;
            case InternalPacketType::USER_PACKET:
            {
                if (it == m_peers.end())
                {
                    LOG_ERROR("Received from " + sender.address.toString() + ":" + tstr(sender.port) + " who is not in the list of peers");
                    break;
                }
                else if (it->status != Peer::Status::UP)
                {
                    LOG_ERROR("Received from " + sender.address.toString() + ":" + tstr(sender.port) + " who is not UP");
                    break;
                }
                LOG("Received from " + sender.address.toString() + ":" + tstr(sender.port));

                m_events.push(NetEvent(NetEvent::Type::ON_RECEIVE, std::move(packet), sender));
                break;
            }
            default:
                break;
            }
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
    
    _event = m_events.front();
    m_events.pop();

    return true;
}

sf::Packet Network::CreatePacket()
{
    sf::Packet packet;
    packet << static_cast<sf::Int8>(InternalPacketType::USER_PACKET);
    return packet;
}

void Network::Send(sf::Packet _packet, NetworkAddress _address)
{
    m_localSocket.send(_packet, _address.address, _address.port);
}

void Network::Send(sf::Packet _packet)
{
    for(const Peer& peer : m_peers)
        Send(_packet, peer.address);
}

void Network::Connect(NetworkAddress _addressToConnect)
{
    internalConnect(_addressToConnect, true);
}

void Network::DisconnectMyself()
{
    sf::Packet packet;
    packet << static_cast<sf::Int8>(InternalPacketType::INTERNAL_DISCONNECT);
    Send(packet);
    m_peers.clear();
    // ? Send ON_DISCONNECT ?
}