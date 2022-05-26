#include "Peer.h"
#include "PacketHeader.h"

Peer::Peer(Transport& _transport, NetworkAddress _addressToConnect, bool _isCreatingFromRequest)
    : Connection(_transport, _addressToConnect, _isCreatingFromRequest)
{

}

void Peer::Update(float _dt)
{
    Connection::Update(_dt);
}

void Peer::Send(const NetworkMessage& _message)
{
    sf::Packet packet;
    PacketHeader header(InternalPacketType::USER_PACKET, _message.IsReliable(), 0);
    header.Serialize(packet);
    packet.append(_message.GetData().getData(),_message.GetData().getDataSize());

    Connection::Send(packet, m_address);
}
    
// void Peer::onReceive()
// {
    
// }

// void Peer::onConnect() 
// {

// }

// void Peer::onDisconnect() 
// {

// }
