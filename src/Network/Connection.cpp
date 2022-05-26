#include "Connection.h"
#include "Log.h"
#include "InternalPacketType.h"
#include "PacketHeader.h"

Connection::Connection(Transport& _transport, NetworkAddress _addressToConnect, bool _isCreatingFromRequest)
    : m_transport(_transport)
    , m_address(_addressToConnect)
{
    sf::Packet packet;
    PacketHeader header;
    if (!_isCreatingFromRequest)
    {
        header.type = InternalPacketType::INTERNAL_CONNECT_REQUEST;
        m_status = Status::CONNECTING;  
    }
    else
    {
        header.type = InternalPacketType::INTERNAL_CONNECT_ACCEPT;
        m_status = Status::UP;
    }
    header.Serialize(packet);
    Send(packet, _addressToConnect);
}

Connection::~Connection()
{
    //m_transport.UnregisterReceiver(this);
}

void Connection::Update(float _dt)
{

}

void Connection::Send(sf::Packet _packet, NetworkAddress _address)
{
    m_transport.Send(_packet, _address);
}

void Connection::OnConnectionAcceptReceived()
{
    m_status = Status::UP;
}

// void Connection::OnReceivePacket(sf::Packet _packet, NetworkAddress _sender) 
// {
//     // auto it = std::find_if(m_peers.begin(), m_peers.end(), 
//     //             [&sender](const Peer& _peer) { return _peer.address == sender; });
    
// }