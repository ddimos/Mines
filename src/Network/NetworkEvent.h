#pragma once

#include "NetworkAddress.h"
#include "NetworkPlayer.h"

struct NetworkEvent
{
    enum class Type
    {
        ON_RECEIVE,
        ON_PLAYER_JOIN,
        ON_PLAYER_LEAVE
    };

    NetworkEvent() = default;
    NetworkEvent(Type _type, const NetworkPlayer& _player)
    : type(_type), player(_player)
    {}

    NetworkEvent(Type _type, const NetworkAddress& _sender)
    : type(_type), sender(_sender)
    {}
    NetworkEvent(Type _type, NetworkMessage&& _message, const NetworkAddress& _sender)
    : type(_type), message(std::move(_message)), sender(_sender)
    {}
    
    Type type;
    NetworkMessage message;
    NetworkPlayer player;

    NetworkAddress sender;
};