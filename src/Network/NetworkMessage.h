#pragma once 
#include "NetworkAddress.h"
#include <SFML/Network.hpp>

class Network;

class NetworkMessage final
{
public:
    NetworkMessage()=default;
    NetworkMessage(NetworkAddress _address, bool _isReliable)
        : m_address(_address), m_isReliable(_isReliable)
    {}

    NetworkAddress GetAddress() const { return m_address; }
    bool IsBroadcast() const { return m_address.address == sf::IpAddress::Broadcast; }
    bool IsReliable() const { return m_isReliable; }
    const sf::Packet& GetData() const { return m_data; }
    bool IsEnd() const { return m_data.endOfPacket(); }
    size_t GetDataSize() const { return m_data.getDataSize(); }

    template <typename T>
    void Write(const T& _value) { m_data << _value; }
    template <typename T>
    void Read(T& _value) {m_data >> _value; }

private:
    friend class Network;

    NetworkAddress m_address = {};
    bool m_isReliable = false;
    sf::Packet m_data;
    InternalPacketType m_type = InternalPacketType::USER_PACKET;
};