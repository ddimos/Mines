#pragma once
#include <unordered_map>
#include "GameListener.h"
#include "PlayerInfo.h"

struct PlayerStats
{
    unsigned numberOfDeathsTotal = 0;
    unsigned numberOfDeathsLastGame = 0;
    unsigned numberOfFoundBombsTotal = 0;
    unsigned numberOfFoundBombsLastGame = 0;
};

using PlayersStats = std::unordered_map<PlayerID, PlayerStats>;

class GameStatsController : public GameListener
{
public:
    GameStatsController() = default;
    ~GameStatsController() = default;

    void OnInit();

    const PlayersStats& GetPlayersStats() const { return m_playersStats; }

    void ProvideStatsFromHost(PlayersStats&& _stats);

private:
    void onPlayerJoined(const PlayerInfo&) override;
    void onPlayerLeft(const PlayerInfo&) override;
    void onGameStart(const WorldConfig&) override;
    void onGameFinish(const GameResult&) override;

    PlayersStats m_playersStats;
};
