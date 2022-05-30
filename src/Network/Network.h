#pragma once

#include "NetworkAddress.h"
#include "NetworkMessage.h"
#include "NetworkEvent.h"
#include "Peer.h"
#include "Transport.h"

#include <queue>
#include <vector>

class Network : public Transport
{
public:
    static bool StartUp();
    static bool ShutDown();
    static Network& Get() {return *ms_network; }

    bool PollEvents(NetworkEvent& _event);
    void Update(float _dt);

    void Send(const NetworkMessage& _message);

    void Connect(NetworkAddress _addressToConnect);
    void Disconnect(NetworkAddress _addressToDisconnect);   

    const std::vector<Peer>& GetPeers() const { return m_peers; }

private:
    Network();
    ~Network() = default;
    Network(const Network&) = delete;
    Network& operator=(const Network&) = delete;

    void OnReceivePacket(sf::Packet _packet, NetworkAddress _sender) override;
    
    Peer* getPeer(NetworkAddress _address);

    static Network* ms_network;
  
    std::queue<NetworkEvent> m_events;
    std::vector<Peer>   m_peers;

};
