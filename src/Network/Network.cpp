#include "Network.h"
#include "Utils.h"
#include "Log.h"
#include "InternalPacketType.h"
#include "PacketHeader.h"


Network* Network::ms_network = nullptr;


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

    // if down delete
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

void Network::OnReceivePacket(sf::Packet _packet, NetworkAddress _sender)
{
    Peer* senderPeer = getPeer(_sender);
 
    // read a header
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

        m_peers.emplace_back(Peer(*this, _sender, true));
        m_events.push(NetworkEvent(NetworkEvent::Type::ON_CONNECT, _sender));
        break;
    }
    case InternalPacketType::INTERNAL_CONNECT_ACCEPT:
    {
        if (!senderPeer)
        {
            LOG_ERROR("CONNECT_ACCEPT received from " + _sender.toString() + "who we didn't ask");
            break;
        }
        else if (senderPeer->GetStatus() != Peer::Status::CONNECTING)
        {
            LOG_ERROR("The status of " + _sender.toString() + " isn't CONNECTING");
            break;
        }
        LOG("CONNECT_ACCEPT received from " + _sender.toString());
        senderPeer->OnConnectionAcceptReceived();
        m_events.push(NetworkEvent(NetworkEvent::Type::ON_CONNECT, _sender));
        break;
    }
    case InternalPacketType::INTERNAL_DISCONNECT:
        break;
    case InternalPacketType::INTERNAL_HEARTBEAT:
        break;
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
        LOG("Received from " + _sender.toString());

        NetworkMessage message;
        message.m_data = std::move(_packet);
        message.m_address = _sender;
        message.m_isReliable = header.isReliable;

        m_events.push(NetworkEvent(NetworkEvent::Type::ON_RECEIVE, std::move(message), _sender));
        break;
    }
    default:
        break;
    }
}

void Network::Connect(NetworkAddress _addressToConnect)
{
    if (getPeer(_addressToConnect))
    {
        LOG_ERROR("Don't connect to " + _addressToConnect.toString() + " because it is already connected.");
        return;
    }
    m_peers.emplace_back(Peer(*this, _addressToConnect, false));
}

void Network::DisconnectMyself()
{
    // TODO
}

Peer* Network::getPeer(NetworkAddress _address)
{
    for (Peer& peer : m_peers)
        if (peer.GetAddress() == _address)
            return &peer;
        
    return nullptr;
}