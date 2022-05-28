#include "Connection.h"
#include "Log.h"
#include "InternalPacketType.h"
#include "PacketHeader.h"

Connection::Connection(Transport& _transport, NetworkAddress _addressToConnect, bool _isCreatingFromRequest)
    : m_address(_addressToConnect)
    , m_transport(&_transport)
{
    sf::Packet packet;
    PacketHeader header;
    if (!_isCreatingFromRequest)
    {
        header.type = InternalPacketType::INTERNAL_CONNECT_REQUEST;
        m_status = Status::CONNECTING;  
        m_timeout = TIME_TO_RETRY_CONNECT_s;
    }
    else
    {
        header.type = InternalPacketType::INTERNAL_CONNECT_ACCEPT;
        m_status = Status::UP;
        m_timeout = HEARTBEAT_TIMEOUT_s;
    }
    header.Serialize(packet);
    Send(packet, _addressToConnect);
    LOG_DEBUG("Send a connect request to " + m_address.toString());
}

void Connection::Update(float _dt)
{
    m_timeout -= _dt;
    if (m_status == Status::CONNECTING)
    {
        if (m_timeout <= 0.f)
        {
            --m_connectionAttemptsLeft;
            if (m_connectionAttemptsLeft < 0)
            {
                LOG("No atempts left, disconnect peer " + m_address.toString());
                m_status = Status::DOWN;
            }
            else
            {
                sf::Packet packet;
                PacketHeader header;
                header.type = InternalPacketType::INTERNAL_CONNECT_REQUEST; 
                m_timeout = TIME_TO_RETRY_CONNECT_s;
                LOG_DEBUG("Send a connect request to " + m_address.toString());  
                header.Serialize(packet);
                Send(packet, m_address);
            }
        }
    }
    else if (m_status == Status::UP)
    {
        if (m_timeout <= 0.f)
        {       
            LOG("Disconnect peer " + m_address.toString() + " because didn't receive a heartbeat");
            m_status = Status::DOWN;
        }
        else
        {
            m_heartbeat -= _dt;
            if (m_heartbeat <= 0)
            {
                sf::Packet packet;
                PacketHeader header;
                header.type = InternalPacketType::INTERNAL_HEARTBEAT; 
                LOG_DEBUG("Send a heartbeat to " + m_address.toString()); 
                header.Serialize(packet);
                Send(packet, m_address);   
                m_heartbeat = HEARTBEAT_s;
            }
        }
    }
}

void Connection::Send(sf::Packet _packet, NetworkAddress _address)
{
    m_transport->Send(_packet, _address);
}

void Connection::OnConnectionAcceptReceived()
{
    m_status = Status::UP;
    m_timeout = HEARTBEAT_TIMEOUT_s;
}

void Connection::OnHeartbeatReceived()
{
    m_timeout = HEARTBEAT_TIMEOUT_s;
}
