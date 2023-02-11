#pragma once

class PlayerInfo;
class WorldConfig;
class GameResult;

class GameListener
{
public:
    virtual void onPlayerJoined(const PlayerInfo&) {}
    virtual void onPlayerLeft(const PlayerInfo&) {}
    virtual void onPlayerInfoUpdated(const PlayerInfo&) {}
    
    virtual void onGameStart(const WorldConfig&) {}
    // TODO
    // OnTextEntered
    // onFlagSet
    // OnGameStart(int _bombsNum);
    // void onGameFinish(const GameResult&) {}
    // void OnCharachterSpawned(const PlayerInfo& _info);
};
