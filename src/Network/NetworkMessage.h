#pragma once 
#include "NetworkAddress.h"
#include "InternalPacketType.h"
#include <SFML/Network.hpp>

class Network;
class Peer;

class NetworkMessage final
{
public:
    enum class Type
    {
        UNICAST,
        BRODCAST,
        EXCLUDE_BRODCAST
    };

    NetworkMessage()=default;
    NetworkMessage(bool _isReliable)
        : m_type(Type::BRODCAST), m_address(), m_isReliable(_isReliable)
    {}
    NetworkMessage(NetworkAddress _address, bool _isReliable)
        : m_type(Type::UNICAST), m_address(_address), m_isReliable(_isReliable)
    {}
    NetworkMessage(Type _type, NetworkAddress _address, bool _isReliable)
        : m_type(_type), m_address(_address), m_isReliable(_isReliable)
    {}

    NetworkAddress GetAddress() const { return m_address; }
    bool IsBroadcast() const { return m_type == Type::BRODCAST; }
    bool IsExcludeBroadcast() const { return m_type == Type::EXCLUDE_BRODCAST; }
    bool IsUnicast() const { return m_type == Type::UNICAST; }
    bool IsReliable() const { return m_isReliable; }
// TODO    bool IsValid() const
    const sf::Packet& GetData() const { return m_data; }
    bool IsEnd() const { return m_data.endOfPacket(); }
    size_t GetDataSize() const { return m_data.getDataSize(); }

    template <typename T>
    void Write(const T& _value) { m_data << _value; }
    template <typename T>
    void Read(T& _value) {m_data >> _value; }

private:
    friend class Network;
    friend class Peer;

    Type m_type = Type::BRODCAST;
    NetworkAddress m_address = {};
    bool m_isReliable = false;
    sf::Packet m_data;
    InternalPacketType m_messageType = InternalPacketType::USER_PACKET;// ? messageType
};