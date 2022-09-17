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

    void CreateAndJoinSession(const std::string& _playerName);  // Should be just CreateSession when I have a server
    void JoinSession(NetworkAddress _address, const std::string& _name);

    bool IsSessionMaster() const { return m_isSessionMaster; }

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
    void onDisconnect(const Peer& _peer);

    void processSessionJoinRequest(NetworkMessage& _message, Peer* _peer);
    void processSessionJoinAccept(NetworkMessage& _message);
    void processSessionOnJoin(NetworkMessage& _message);
    void processSessionOnLeave(NetworkMessage& _message);
    
    static Network* ms_network;
  
    std::queue<NetworkEvent> m_events;
    std::vector<Peer>   m_peers;


    // session
    NetworkPlayer* createPlayerIntrernal(const std::string& _name, PlayerID _id, bool _isLocal);
    std::vector<NetworkPlayer> m_players; // TODO sharedPtr
    NetworkPlayer* m_localPlayer = nullptr;

    bool m_isSessionMaster = false;
    bool m_isSessionCreated = false;
    bool m_connectingToHost = false;

    PlayerID m_playerIdGenerator = 0;

    std::string m_pendingPlayerToJoin;
};
