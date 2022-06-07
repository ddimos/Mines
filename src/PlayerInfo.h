#pragma once
#include <SFML/Graphics.hpp>
#include "Network/NetworkAddress.h"

struct PlayerInfo
{
    std::string name = "";
    NetworkAddress address;
    //sf::Color color = sf::Color::White;
};
