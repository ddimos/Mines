#pragma once
#include <SFML/System.hpp>

enum class NetworkPacketType : sf::Uint16
{
    None = 0,
    JOIN_REQUEST = 1,
    JOIN_ACCEPT = 2,
    CREATE_CHARACTER,
    REPLICATE_CHARACTER_POS,
    REPLICATE_CHARACTER_UNCOVER,
    REPLICATE_CHARACTER_TOGGLE
};
