#pragma once
#include <SFML/System.hpp>

enum class NetworkMessageType : sf::Uint16 
{
    None = 0,
    CREATE_GAME,
    SHARE_PEERS,
    SHARE_LOCAL_PLAYER_INFO,
    CREATE_CHARACTER,
    REPLICATE_CHARACTER_POS,
    REPLICATE_CHARACTER_UNCOVER,
    REPLICATE_CHARACTER_TOGGLE
};
