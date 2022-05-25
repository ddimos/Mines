#pragma once
#include "NetworkAddress.h"
#include "NetworkMessage.h"
#include "Transport.h"
#include "Connection.h"

class Peer : public Connection
{
public:


    static constexpr float TIME_TO_RETRY_CONNECT_s = 2.5f; // reliable
    static const int CONNECTION_ATTEMPTS = 10;

    Peer(Transport& _transport, NetworkAddress _addressToConnect, bool _isCreatingFromRequest);

    void Update(float _dt);
    void Send(const NetworkMessage& _message);
    
    NetworkAddress GetAddress() const { return m_address; }
    
    bool IsUp() const { return GetStatus() == Connection::Status::UP; }
    bool IsDown() const { return GetStatus() == Connection::Status::DOWN; }

private:

    // void onReceive() override;
    // void onConnect() override;
    // void onDisconnect() override;

   // NetworkAddress m_address = {};
    
    // reliableMessages
    // unreliableMessages


    //bool isFirstConnect = false;
    //float timeout = TIME_TO_RETRY_CONNECT_s;
    //float hearbeat = HEARTBEAT_s;
    //int attemptsLeft = CONNECTION_ATTEMPTS;
};