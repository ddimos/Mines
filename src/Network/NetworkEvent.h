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
    NetworkEvent(Type _type, NetworkMessage&& _message, const NetworkAddress& _sender)
    : type(_type), message(std::move(_message)), sender(_sender)
    {}
    
    Type type;
    NetworkMessage message;
    NetworkAddress sender;
};