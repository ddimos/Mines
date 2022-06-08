#pragma once
#include <SFML/Graphics.hpp>
#include "Network/NetworkAddress.h"
#include "Character.h"

struct PlayerInfo
{
    std::string name = "";
    NetworkAddress address;
    CharacterInfo charInfoCopy;
};
