#pragma once

#include "NetworkAddress.h"
#include "NetworkMessage.h"
#include "NetworkEvent.h"
#include "NetworkPlayer.h"
#include "NetworkUtils.h"
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

    void LeaveSession();

    bool IsSessionMaster() const { return m_isSessionMaster; }
    PeerID GetHostPeerId() const { return m_hostPeerId; }

    const std::vector<Peer>& GetPeers() const { return m_peers; }

private:
    Network(unsigned short _port);
    ~Network() = default;
    Network(const Network&) = delete;
    Network& operator=(const Network&) = delete;

    void OnReceivePacket(sf::Packet _packet, NetworkAddress _sender) override;
    
    Peer* getPeer(NetworkAddress _address);
    Peer* getPeer(PeerID _peerId);

    void connect(NetworkAddress _addressToConnect);
    void disconnect(); 

    void onConnect(Peer& _peer);
    void onDisconnect(const Peer& _peer);

    void processSessionJoinRequest(NetworkMessage& _message, Peer* _peer);
    void processSessionJoinAccept(NetworkMessage& _message);
    void processSessionOnJoin(NetworkMessage& _message);
    void processSessionOnLeave(NetworkMessage& _message);
    
    Peer& createPeerInternal(NetworkAddress _addressToConnect, bool _isCreatingFromRequest);

    static Network* ms_network;
  
    std::queue<NetworkEvent> m_events;
    std::vector<Peer>   m_peers;
    PeerID m_hostPeerId = PeerIdInvalid;

    // session
    NetworkPlayer* createPlayerIntrernal(const std::string& _name, PlayerID _id, bool _isLocal);
    std::vector<NetworkPlayer> m_players; // TODO sharedPtr
    NetworkPlayer* m_localPlayer = nullptr;

    bool m_isSessionMaster = false;
    bool m_isSessionCreated = false;
    bool m_connectingToHost = false;

    PeerID m_peerIdGenerator = 0;
    PlayerID m_playerIdGenerator = 0;

    std::string m_pendingPlayerToJoin;
};
