#include "Network.h"
#include "Log.h"
#include "InternalPacketType.h"
#include "PacketHeader.h"


Network* Network::ms_network = nullptr;

bool Network::StartUp(unsigned short _port)
{
    if (!ms_network)
    {
        ms_network = new Network(_port);
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

Network::Network(unsigned short _port)
    : Transport(_port)
{
    m_peers.reserve(10);
}

bool Network::PollEvents(NetworkEvent& _event)
{
    if (m_events.empty())
        return false; 
    
    _event = m_events.front();
    m_events.pop();

    return true;
}

void Network::Update(float _dt)
{
    Transport::Update();
    
    for (Peer& peer : m_peers)
    {
        peer.Update(_dt);
    }

    m_peers.erase(std::remove_if(m_peers.begin(), m_peers.end(), 
    [this](const Peer& _peer)
    {
        if (_peer.IsDown())
        {
            m_events.push(NetworkEvent(NetworkEvent::Type::ON_DISCONNECT, _peer.GetAddress()));
            return true;
        }
        return false;
    }), m_peers.end());
}

void Network::Send(const NetworkMessage& _message)
{
    if (_message.IsBroadcast())
    {
        for (Peer& peer : m_peers)
        {
            if (peer.IsUp())
                peer.Send(_message);
            else
                LOG("Don't send the message to the peer " + _message.GetAddress().toString() + " because it isn't up.");
        }
    }
    else
    {
        Peer* peer = getPeer(_message.GetAddress());
        if (peer && peer->IsUp())
            peer->Send(_message);
        else
            LOG("Don't send the message to the peer " + _message.GetAddress().toString() + " because it isn't up.");
    }
}

void Network::CreateSession()
{
    m_isSessionMaster = true;
    m_isSessionCreated = true;  // TODO: onSessionCreatedEvent
}

void Network::JoinSession(NetworkAddress _address, const std::string& _name)
{
    if (!m_isSessionCreated)
    {
        LOG_ERROR("Cannot join the session, because it isn't created");
        return;
    }


    if (m_isSessionMaster)
    {
        auto& player = m_players.emplace_back(NetworkPlayer(_name, ++m_playerIdGenerator, true));
        m_events.push(NetworkEvent(NetworkEvent::Type::ON_PLAYER_JOIN, player)); // it's a copy
    }
    else
    {
        connect(_address);
    }

    
}

void Network::OnReceivePacket(sf::Packet _packet, NetworkAddress _sender)
{
    Peer* senderPeer = getPeer(_sender);
 
    PacketHeader header;
    header.Deserialize(_packet);

    switch (header.type)
    {
    case InternalPacketType::INTERNAL_CONNECT_REQUEST:
    {
        if (senderPeer)
        {
            LOG_ERROR("CONNECT_REQUEST received again from " + _sender.toString());
            break;
        }
        LOG("CONNECT_REQUEST received from " + _sender.toString());

        Peer& peer = m_peers.emplace_back(Peer(*this, _sender, true));
        onConnect(peer);
    //    m_events.push(NetworkEvent(NetworkEvent::Type::ON_CONNECT, _sender));
        break;
    }
    case InternalPacketType::INTERNAL_CONNECT_ACCEPT:
    {
        if (!senderPeer)
        {
            LOG_ERROR("CONNECT_ACCEPT received from " + _sender.toString() + " who we didn't ask");
            break;
        }
        else if (senderPeer->GetStatus() != Peer::Status::CONNECTING)
        {
            LOG_ERROR("The status of " + _sender.toString() + " isn't CONNECTING");
            break;
        }
        LOG("CONNECT_ACCEPT received from " + _sender.toString());
        senderPeer->OnConnectionAcceptReceived();
        onConnect(*senderPeer);
        //m_events.push(NetworkEvent(NetworkEvent::Type::ON_CONNECT, _sender));
        break;
    }
    case InternalPacketType::INTERNAL_DISCONNECT:
    {
        if (!senderPeer)
        {
            LOG_ERROR("DISCONNECT received from " + _sender.toString() + " who is not in the list of peers");
            break;
        }
        else if (senderPeer->GetStatus() == Peer::Status::DOWN)
        {
            LOG_ERROR("The status of " + _sender.toString() + " is already DOWN");
            break;
        }
        LOG("DISCONNECT received from " + _sender.toString());
        senderPeer->Close(true);
        break;
    }
    case InternalPacketType::INTERNAL_HEARTBEAT:
    {
        if (!senderPeer)
        {
            LOG_ERROR("Received from " + _sender.toString() + " who is not in the list of peers");
            break;
        }
        else if (senderPeer->GetStatus() == Peer::Status::CONNECTING)
        {
            senderPeer->OnConnectionAcceptReceived();
            m_events.push(NetworkEvent(NetworkEvent::Type::ON_CONNECT, _sender));
            LOG("Received a heartbeat from " + _sender.toString() + " while waiting for connection accept");
        }
        else if (!senderPeer->IsUp())
        {
            LOG_ERROR("Received from " + _sender.toString() + " who is not UP");
            break;
        }
        senderPeer->OnHeartbeatReceived();
        LOG_DEBUG("Received a heartbeat from " + _sender.toString());
        break;
    }
    case InternalPacketType::INTERNAL_AR:
    {
        if (!senderPeer)
        {
            LOG_ERROR("Received from " + _sender.toString() + " who is not in the list of peers");
            break;
        }
        else if (!senderPeer->IsUp())
        {
            LOG_ERROR("Received from " + _sender.toString() + " who is not UP");
            break;
        }
        LOG_DEBUG("AR received from " + _sender.toString() + " seqNum: " + tstr(header.sequenceNum));

        senderPeer->OnAcknowledgmentReceived(header.sequenceNum);

        break;
    }
    //case InternalPacketType::INTERNAL_SESSION_JOIN_REQUEST:
    //{
        // if (!senderPeer)
        // {
        //     LOG_ERROR("Received from " + _sender.toString() + " who is not in the list of peers");
        //     break;
        // }
        // else if (!senderPeer->IsUp())
        // {
        //     LOG_ERROR("Received from " + _sender.toString() + " who is not UP");
        //     break;
        // }
        // if (!m_isSessionMaster)
        // {
        //     LOG_ERROR("Cannot process the JoinRequest recieved from " + _sender.toString() + " because not the session master");
        //     break;
        // }
        // LOG_DEBUG("JoinRequest received from " + _sender.toString());

        // auto& player = m_players.emplace_back(NetworkPlayer(_name, ++m_playerIdGenerator, true));
        // m_events.push(NetworkEvent(NetworkEvent::Type::ON_PLAYER_JOIN, player));
        
        break;
    //}
    case InternalPacketType::INTERNAL_SESSION_JOIN_REQUEST:
    case InternalPacketType::INTERNAL_SESSION_JOIN_ACCEPT:
    case InternalPacketType::USER_PACKET:
    {
        if (!senderPeer)
        {
            LOG_ERROR("Received from " + _sender.toString() + " who is not in the list of peers");
            break;
        }
        else if (!senderPeer->IsUp())
        {
            LOG_ERROR("Received from " + _sender.toString() + " who is not UP");
            break;
        }
        LOG_DEBUG("Received from " + _sender.toString());

        NetworkMessage message;
        message.m_data = std::move(_packet);
        message.m_address = _sender;
        message.m_isReliable = header.isReliable;

        if (header.isReliable)
        {
            senderPeer->OnReliableReceived(header.sequenceNum, message);
            auto& messages = senderPeer->GetMessagesToDeliver();
            while(!messages.empty())
            {
                m_events.push(NetworkEvent(NetworkEvent::Type::ON_RECEIVE, std::move(messages.front()), _sender));
                messages.pop();
            }
        }
        else
        {
            m_events.push(NetworkEvent(NetworkEvent::Type::ON_RECEIVE, std::move(message), _sender));
        }
        
        break;
    }
    default:
        break;
    }
}

void Network::connect(NetworkAddress _addressToConnect)
{
    if (getPeer(_addressToConnect))
    {
        LOG_ERROR("Don't connect to " + _addressToConnect.toString() + " because it is already connected.");
        return;
    }
    LOG("Connect to " + _addressToConnect.toString());

    m_peers.emplace_back(Peer(*this, _addressToConnect, false));
}

void Network::disconnect(NetworkAddress _addressToDisconnect)
{
    if (_addressToDisconnect.address == sf::IpAddress::Broadcast)
    {
        for (Peer& peer : m_peers)
            peer.Close();
    }
    else
    {
        if (Peer* peer = getPeer(_addressToDisconnect))
        {
            peer->Close();
            return;
        }   
        LOG_ERROR("Don't disconnect from " + _addressToDisconnect.toString() + " because this peer is not connected now.");
    }
}

bool Network::DoesPeerExist(NetworkAddress _address) const
{
    for (const Peer& peer : m_peers)
        if (peer.GetAddress() == _address)
            return true;
    return false;
}

Peer* Network::getPeer(NetworkAddress _address)
{
    for (Peer& peer : m_peers)
        if (peer.GetAddress() == _address)
            return &peer;
        
    return nullptr;
}

void Network::onConnect(Peer& _peer)
{
    if (m_isSessionMaster)
        return;
    
    NetworkMessage message(_peer.GetAddress(), true);
    message.m_type = InternalPacketType::INTERNAL_JOIN_SESSION_REQUEST;
    message.Write(m_pendingPlayerToJoin);
    LOG_DEBUG("Send a join session request to " + _peer.GetAddress().toString());  
    _peer.Send(message);
}