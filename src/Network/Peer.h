#pragma once
#include "NetworkAddress.h"
#include "NetworkMessage.h"
#include "Transport.h"
#include "Connection.h"
#include <queue>
#include <deque>
#include <map>


class Peer : public Connection
{
public:

    Peer(Transport& _transport, NetworkAddress _addressToConnect, bool _isCreatingFromRequest);

    void Update(float _dt);
    void Send(const NetworkMessage& _message);
    
    NetworkAddress GetAddress() const { return m_address; }
    
    bool IsUp() const { return GetStatus() == Connection::Status::UP; }
    bool IsDown() const { return GetStatus() == Connection::Status::DOWN; }

    void OnReliableReceived(sf::Uint32 _seqNum, const NetworkMessage& _message);
    void OnAcknowledgmentReceived(sf::Uint32 _seqNum);
    std::queue<NetworkMessage>& GetMessagesToDeliver() { return m_messagesToDeliver; };

private:
    static constexpr float TIME_TO_RESEND_s = 1.0f;

    struct ReliablePacketInfo
    {
        sf::Uint32 seqNum = 0;
        sf::Packet packet;
        float timeout = TIME_TO_RESEND_s;
        bool isAcknowledged = false;
    };
    void onReliableSent(sf::Packet _packet, sf::Uint32 _seqNum);
    void sendAR(sf::Uint32 _seqNum);

    sf::Uint32 m_sequenceNumGenerator = 0;
    sf::Uint32 m_sequenceNumberOfLastSent = 0;
    sf::Uint32 m_sequenceNumberOfLastDelivered = 0;
    std::vector<ReliablePacketInfo> m_reliableSent;
    std::queue<NetworkMessage> m_messagesToDeliver;
    std::map<sf::Uint32, NetworkMessage> m_messagesToStore;

    // TODO:
    // std::queue<NetworkMessage> m_reliableMessagesToSend;     
    // std::queue<NetworkMessage> m_unreliableMessagesToSend;     
};