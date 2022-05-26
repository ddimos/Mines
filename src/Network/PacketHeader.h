#pragma once
#include "InternalPacketType.h"

struct PacketHeader
{
    PacketHeader() = default;
    PacketHeader(InternalPacketType _type, bool _isReliable, sf::Uint32 _seqNum)
        : type(_type), isReliable(_isReliable), sequenceNum(_seqNum)
    {}

    void Serialize(sf::Packet& _writeBuffer)
    {
        _writeBuffer << static_cast<sf::Uint8>(type);
        _writeBuffer << isReliable;
        _writeBuffer << sequenceNum;
    }

    void Deserialize(sf::Packet& _readBuffer)
    {
        sf::Uint8 t;
        _readBuffer >> t;
        type = static_cast<InternalPacketType>(t);
        _readBuffer >> isReliable;
        _readBuffer >> sequenceNum; 
    }

    InternalPacketType type = InternalPacketType::None;
    bool isReliable = false;
    sf::Uint32 sequenceNum = 0;
};
