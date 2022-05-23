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
    Peer peer;
    peer.address = _addressToConnect;
    peer.isFirstConnect = _isFirstConnect;
    peer.status = Peer::Status::CONNECTING;
    m_peers.push_back(peer);
    Send(packet, _addressToConnect);
}

void Network::Update(float _dt)
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
                    LOG_ERROR("CONNECT_REQUEST received again from " + sender.toString());
                    break;
                }
                LOG("CONNECT_REQUEST received from " + sender.toString());
                
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

                Peer peer;
                peer.address = sender;
                peer.status = Peer::Status::UP;
                peer.timeout = Peer::HEARTBEAT_TIMEOUT_s;
                m_peers.push_back(peer);
                m_events.push(NetEvent(NetEvent::Type::ON_CONNECT, sender));
                break;
            }
            case InternalPacketType::INTERNAL_CONNECT_ACCEPT:
            {
                if (it == m_peers.end())
                {
                    LOG_ERROR("CONNECT_ACCEPT received from " + sender.toString() + "who we didn't ask");
                    break;
                }

                if (it->status != Peer::Status::CONNECTING)
                {
                    LOG_ERROR("The status of " + sender.toString() + " isn't CONNECTING");
                    break;
                }
                LOG("CONNECT_ACCEPT received from " + sender.toString());
                it->status = Peer::Status::UP;
                it->timeout = Peer::HEARTBEAT_TIMEOUT_s;

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
                break;
            case InternalPacketType::INTERNAL_HEARTBEAT:
            {
                if (it == m_peers.end())
                {
                    LOG_ERROR("Received from " + sender.toString() + " who is not in the list of peers");
                    break;
                }
                else if (it->status == Peer::Status::CONNECTING)
                {
                    it->status = Peer::Status::UP;
                    m_events.push(NetEvent(NetEvent::Type::ON_CONNECT, sender));
                    LOG("Received a heartbeat from " + sender.toString() + " while waiting for connection accept");
                }
                else if (it->status != Peer::Status::UP)
                {
                    LOG_ERROR("Received from " + sender.toString() + " who is not UP");
                    break;
                }
                it->timeout = Peer::HEARTBEAT_TIMEOUT_s;
                LOG("Received a heartbeat from " + sender.toString());

                break;
            }
            case InternalPacketType::USER_PACKET:
            {
                if (it == m_peers.end())
                {
                    LOG_ERROR("Received from " + sender.toString() + " who is not in the list of peers");
                    break;
                }
                else if (it->status != Peer::Status::UP)
                {
                    LOG_ERROR("Received from " + sender.toString() + " who is not UP");
                    break;
                }
                LOG("Received from " + sender.toString());

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

    for (Peer& peer : m_peers)
    {
        if (peer.status == Peer::Status::CONNECTING)
        {
            peer.timeout -= _dt;
            if (peer.timeout > 0.f)
                continue;

            --peer.attemptsLeft;
            if (peer.attemptsLeft < 0)
            {
                LOG("No atempts left, disconnect peer " + peer.address.toString());
                peer.status = Peer::Status::DOWN;
                continue;
            }
            
            sf::Packet packet;
            packet << static_cast<sf::Int8>(InternalPacketType::INTERNAL_CONNECT_REQUEST);
            packet << peer.isFirstConnect;
            Send(packet, peer.address);
            peer.timeout = Peer::TIME_TO_RETRY_CONNECT_s;
            LOG("Send a connect request to " + peer.address.toString());            
        }
        else if (peer.status == Peer::Status::UP)
        {
            peer.timeout -= _dt;
            if (peer.timeout <= 0.f)
            {       
                LOG("Disconnect peer " + peer.address.toString() + " because didn't receive a heartbeat");
                peer.status = Peer::Status::DOWN;
                m_events.push(NetEvent(NetEvent::Type::ON_DISCONNECT, peer.address));
                continue;
            }
            
            peer.hearbeat -= _dt;
            if (peer.hearbeat > 0)
                continue;

            sf::Packet packet;
            packet << static_cast<sf::Int8>(InternalPacketType::INTERNAL_HEARTBEAT);
            Send(packet, peer.address);
            LOG("Send a heartbeat to " + peer.address.toString());
            peer.hearbeat = Peer::HEARTBEAT_s;
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