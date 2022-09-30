#pragma once
#include <SFML/System.hpp>

enum class NetworkMessageType : sf::Uint16 
{
    None = 0,
    CREATE_GAME,
    CREATE_CHARACTER,
    REPLICATE_CHARACTER_CONTROLS,
    REPLICATE_CHARACTER_POS,
    REPLICATE_CHARACTER_UNCOVER,
    REPLICATE_CHARACTER_TOGGLE
};
