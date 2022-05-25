#include "Peer.h"


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
