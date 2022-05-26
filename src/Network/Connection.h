#pragma once
#include <SFML/Network.hpp>
#include "NetworkAddress.h"
#include "Transport.h"

class Connection
{
public:
    enum class Status
    {
        None,
        CONNECTING,
        UP,
        DISCONNECTING,
        DOWN
    };

    Status GetStatus() const { return m_status; }
    void OnConnectionAcceptReceived();
    void OnHeartbeatReceived();

protected:

    Connection(Transport& _transport, NetworkAddress _addressToConnect, bool _isCreatingFromRequest);
    ~Connection();

    void Update(float _dt);
    
    void Send(sf::Packet _packet, NetworkAddress _address);

    // virtual void onReceive() {}
    // virtual void onConnect() {}
    // virtual void onDisconnect() {}

protected:
    NetworkAddress m_address = {};

private:

    static constexpr float TIME_TO_RETRY_CONNECT_s = 2.5f;
    static constexpr float HEARTBEAT_TIMEOUT_s = 10.f;
    static constexpr float HEARTBEAT_s = 1.f;
    static const int CONNECTION_ATTEMPTS = 10;

    Transport* m_transport;
    Status m_status = Status::None;
    float m_timeout = 0.f;
    int m_connectionAttemptsLeft = CONNECTION_ATTEMPTS;
    float m_heartbeat = HEARTBEAT_s;
};


