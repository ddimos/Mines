#pragma once 
#include "NetworkAddress.h"
#include <SFML/Network.hpp>

// class NetworkMessage
// {
// public:
//     NetworkMessage() = default;
//     virtual ~NetworkMessage() = default;

//     NetworkAddress GetAddress() const { return m_address; }
//     bool IsBroadcast() const { return m_address.address == sf::IpAddress::Broadcast; }
//     bool IsReliable() const { return m_isReliable; }

//     void Serialize(sf::Packet& _writeBuffer) { onSerialize(_writeBuffer); }
//     void Deserialize(sf::Packet& _readBuffer) { onDeserialize(_readBuffer); }

//     virtual std::unique_ptr<NetworkMessage> Copy() = 0;

// protected:
//     virtual void onSerialize(sf::Packet& _writeBuffer){}
//     virtual void onDeserialize(sf::Packet& _readBuffer){}

//     // internaltype
//     bool m_isReliable = false;
//     NetworkAddress m_address = {};
// };
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

    template <typename T>
    void Write(const T& _value) { m_data << _value; }
    template <typename T>
    void Read(T& _value) {m_data >> _value; }

private:
    friend class Network;

    bool m_isReliable = false;
    NetworkAddress m_address = {};
    sf::Packet m_data;
};