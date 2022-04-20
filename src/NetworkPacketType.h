#pragma once
#include <SFML/System.hpp>

enum class NetworkPacketType : sf::Uint16
{
    None = 0,
    CONNECT = 1,
    CREATE_GAME = 2,
    CREATE_CHARACTER,
    REPLICATE_CHARACTER_POS,
    REPLICATE_CHARACTER_UNCOVER,
    REPLICATE_CHARACTER_TOGGLE
};
