#pragma once

class PlayerInfo;
class WorldConfig;
class GameResult;
class Cell;
class Character;

class GameListener
{
public:
    virtual void onPlayerJoined(const PlayerInfo&) {}
    virtual void onPlayerLeft(const PlayerInfo&) {}
    virtual void onPlayerInfoUpdated(const PlayerInfo&) {}
    
    virtual void onGameStart(const WorldConfig&) {}
    
    virtual void onCharacterToggleFlagCell(const Cell&, const Character&) {}
    virtual void onCharacterDie(const Character&) {}
    
    // TODO
    // OnTextEntered
    // void onGameFinish(const GameResult&) {}
    // void OnCharachterSpawned(const PlayerInfo& _info);
};
