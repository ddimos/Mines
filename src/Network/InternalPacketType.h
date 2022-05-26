#pragma once

#include <SFML/Network.hpp>

enum class InternalPacketType : sf::Uint8
{
    None,
    INTERNAL_CONNECT_REQUEST,
    INTERNAL_CONNECT_ACCEPT,
    INTERNAL_DISCONNECT,
    INTERNAL_HEARTBEAT,
    USER_PACKET
};
