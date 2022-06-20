#pragma once

#include "NetworkAddress.h"
#include "NetworkMessage.h"
#include "NetworkEvent.h"
#include "NetworkPlayer.h"
#include "Peer.h"
#include "Transport.h"

#include <queue>
#include <vector>

class Network : public Transport
{
public:
    static bool StartUp(unsigned short _port);
    static bool ShutDown();
    static Network& Get() {return *ms_network; }

    bool PollEvents(NetworkEvent& _event);
    void Update(float _dt);

    void Send(const NetworkMessage& _message);

    void CreateSession();
    void JoinSession(NetworkAddress _address, const std::string& _name);

  

    const std::vector<Peer>& GetPeers() const { return m_peers; }
    bool DoesPeerExist(NetworkAddress _address) const;

private:
    Network(unsigned short _port);
    ~Network() = default;
    Network(const Network&) = delete;
    Network& operator=(const Network&) = delete;

    void OnReceivePacket(sf::Packet _packet, NetworkAddress _sender) override;
    
    Peer* getPeer(NetworkAddress _address);

    void connect(NetworkAddress _addressToConnect);
    void disconnect(NetworkAddress _addressToDisconnect); 

    void onConnect(Peer& _peer);
    void onDisconnect(NetworkAddress _addressToDisconnect) {} 

    
    static Network* ms_network;
  
    std::queue<NetworkEvent> m_events;
    std::vector<Peer>   m_peers;

    // session
    std::vector<NetworkPlayer> m_players; // TODO sharedPtr

    bool m_isSessionMaster = false;
    bool m_isSessionCreated = false;

    PlayerID m_playerIdGenerator = 0;

    std::string m_pendingPlayerToJoin;
};
