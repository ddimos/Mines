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
    m_players.reserve(10); // TODO use ptrs
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
            onDisconnect(_peer);
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
    else if(_message.IsExcludeBroadcast())
    {
        for (Peer& peer : m_peers)
        {
            if (peer.GetAddress() == _message.GetAddress())
                continue;
            
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

void Network::CreateAndJoinSession(const std::string& _playerName)
{
    m_isSessionMaster = true;
    m_isSessionCreated = true;  // TODO: onSessionCreatedEvent

    m_localPlayer = createPlayerIntrernal(_playerName, ++m_playerIdGenerator, true);
    m_events.emplace(NetworkEvent(NetworkEvent::Type::ON_PLAYER_JOIN, *m_localPlayer)); // it's a copy
}

void Network::JoinSession(NetworkAddress _address, const std::string& _playerName)
{
    if (m_isSessionMaster || m_connectingToHost)
        return;
    
    m_connectingToHost = true;
    m_localPlayer = createPlayerIntrernal(_playerName, PlayerIdInvalid, true);
    connect(_address);
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
        if (!m_isSessionMaster)
        {
            LOG_ERROR("Cannot process CONNECT_REQUEST recieved from " + _sender.toString() + " because not the session master");
            break;
        }
        
        LOG("CONNECT_REQUEST received from " + _sender.toString());

        Peer& peer = m_peers.emplace_back(Peer(*this, _sender, true));
        onConnect(peer);
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
            onConnect(*senderPeer);
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
    
    case InternalPacketType::INTERNAL_SESSION_JOIN_REQUEST:
    case InternalPacketType::INTERNAL_SESSION_JOIN_ACCEPT:
    case InternalPacketType::INTERNAL_SESSION_ON_JOIN:
    case InternalPacketType::INTERNAL_SESSION_ON_LEAVE:
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
        message.m_messageType = header.type;

        if (header.isReliable)
        {
            senderPeer->OnReliableReceived(header.sequenceNum, message);
            auto& messages = senderPeer->GetMessagesToDeliver();
            while(!messages.empty())
            {
                auto& mes = messages.front();   // It's a real mess to have different types here
                if (mes.m_messageType == InternalPacketType::INTERNAL_SESSION_JOIN_REQUEST)
                    processSessionJoinRequest(mes, senderPeer);
                else if (mes.m_messageType == InternalPacketType::INTERNAL_SESSION_JOIN_ACCEPT)
                    processSessionJoinAccept(mes);
                else if (mes.m_messageType == InternalPacketType::INTERNAL_SESSION_ON_JOIN)
                    processSessionOnJoin(mes);
                else if (mes.m_messageType == InternalPacketType::INTERNAL_SESSION_ON_LEAVE)
                    processSessionOnLeave(mes);
                else
                    m_events.push(NetworkEvent(NetworkEvent::Type::ON_RECEIVE, std::move(mes), _sender));
                
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
    message.m_messageType = InternalPacketType::INTERNAL_SESSION_JOIN_REQUEST;
    message.Write(m_localPlayer->m_name);
    LOG_DEBUG("Send a join session request to " + _peer.GetAddress().toString());  
    _peer.Send(message);
}

void Network::onDisconnect(const Peer& _peer)
{
    if (m_isSessionMaster)
    {
        // One player leave
        // send a brodacast 
    }
    else
    {
        // All players leave
        // for (NetworkPlayer& player : m_players) 
        //m_events.emplace(NetworkEvent(NetworkEvent::Type::ON_PLAYER_LEAVE, *player));
    }
    
    // add a player left if it's a master
    // ? a host left if not a master?
}

NetworkPlayer* Network::createPlayerIntrernal(const std::string& _name, PlayerID _id, bool _isLocal)
{
    auto& player = m_players.emplace_back(NetworkPlayer(_name, _id, _isLocal));
    return &player;
}

void Network::processSessionJoinRequest(NetworkMessage& _message, Peer* _peer)
{
    if (!m_isSessionMaster)
    {
        LOG_ERROR("Cannot process the JoinRequest recieved from " + _message.GetAddress().toString() + " because not the session master");
        return;
    }
    LOG_DEBUG("JoinRequest received from " + _message.GetAddress().toString()); // TODO replace with a peer id

    std::string newPlayerName;
    _message.Read(newPlayerName);
    PlayerID newPlayerId = ++m_playerIdGenerator;

    NetworkMessage message(NetworkMessage::Type::UNICAST, _peer->GetAddress(), true);
    message.m_messageType = InternalPacketType::INTERNAL_SESSION_JOIN_ACCEPT;
    message.Write(newPlayerName);
    message.Write(newPlayerId);
    for (const auto& player : m_players)
    {
        message.Write(player.m_name);
        message.Write(player.m_id);
    }
    
    LOG_DEBUG("Send a join session accept to " + _peer->GetAddress().toString());  
    Send(message);   

    NetworkMessage messageOnJoin(NetworkMessage::Type::EXCLUDE_BRODCAST, _peer->GetAddress(), true);
    messageOnJoin.m_messageType = InternalPacketType::INTERNAL_SESSION_ON_JOIN;
    messageOnJoin.Write(newPlayerName);
    messageOnJoin.Write(newPlayerId);
     
    LOG_DEBUG("Send a onPlayerJoin");  
    Send(messageOnJoin);   

    auto* player = createPlayerIntrernal(newPlayerName, newPlayerId, false);
    player->m_peer = _peer; // I guess I need this only on the host side
    m_events.emplace(NetworkEvent(NetworkEvent::Type::ON_PLAYER_JOIN, *player));
}

void Network::processSessionJoinAccept(NetworkMessage& _message)
{
    if (m_isSessionMaster)
    {
        LOG_ERROR("The session master should not receive a JoinAccept");
        return;
    }

    LOG_DEBUG("JoinAccept received from " + _message.GetAddress().toString());

    std::string playerName;
    PlayerID playerId;
    _message.Read(playerName);
    _message.Read(playerId);

    if (m_localPlayer->m_name != playerName)
    {
        LOG_ERROR("The local player name is not equal to the one received from the host: " + m_localPlayer->m_name + " != " + playerName);
        return;
    }
    m_localPlayer->m_id = playerId;
    m_events.emplace(NetworkEvent(NetworkEvent::Type::ON_PLAYER_JOIN, *m_localPlayer));
    while (!_message.IsEnd())
    {
        _message.Read(playerName);
        _message.Read(playerId);
        auto player = createPlayerIntrernal(playerName, playerId, false);
        m_events.emplace(NetworkEvent(NetworkEvent::Type::ON_PLAYER_JOIN, *player));
    }
}

void Network::processSessionOnJoin(NetworkMessage& _message)
{
    if (m_isSessionMaster)
    {
        LOG_ERROR("The session master should not receive a JoinAccept");
        return;
    }

    LOG_DEBUG("OnJoin received from " + _message.GetAddress().toString());

    std::string playerName;
    PlayerID playerId;
    _message.Read(playerName);
    _message.Read(playerId);

    auto player = createPlayerIntrernal(playerName, playerId, false);
    m_events.emplace(NetworkEvent(NetworkEvent::Type::ON_PLAYER_JOIN, *player));
}

void Network::processSessionOnLeave(NetworkMessage& _message)
{
    if (m_isSessionMaster)
    {
        LOG_ERROR("The session master should not receive a JoinAccept");
        return;
    }

}
    