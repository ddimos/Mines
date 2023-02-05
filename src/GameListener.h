#pragma once

class PlayerInfo;

class GameListener
{
public:
    virtual void onPlayerJoined(const PlayerInfo&) {}
    virtual void onPlayerLeft(const PlayerInfo&) {}
    
    // TODO
    // OnTextEntered
    // onFlagSet
    // OnGameStart(int _bombsNum);
    // void OnGameFinish(bool _isVictory);
    // void OnCharachterSpawned(const PlayerInfo& _info);
};
