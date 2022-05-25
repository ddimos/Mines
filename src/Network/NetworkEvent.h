#pragma once

#include "NetworkAddress.h"

struct NetworkEvent
{
    enum class Type
    {
        ON_CONNECT,
        ON_RECEIVE,
        ON_DISCONNECT
    };
    
    NetworkEvent() = default;
    NetworkEvent(Type _type, const NetworkAddress& _sender)
    : type(_type), sender(_sender)
    {}
    NetworkEvent(Type _type, sf::Packet&& _packet, const NetworkAddress& _sender)
    : type(_type), packet(std::move(_packet)), sender(_sender)
    {}
    
    Type type;
    sf::Packet packet;
    NetworkAddress sender;
};