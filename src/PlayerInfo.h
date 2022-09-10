#pragma once
#include <SFML/Graphics.hpp>
#include "Network/NetworkPlayer.h"
#include "Character.h"

struct PlayerInfo
{

    // TODO add GetPlayerId() GetName()
    NetworkPlayer networkPlayerCopy;
    CharacterInfo charInfoCopy;
};
