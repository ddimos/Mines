#include "GameStats.h"
#include "Utils/Log.h"
#include "Game.h"
#include "GameWorld.h"

void GameStatsController::OnInit()
{
    Game::Get().RegisterGameListener(this);
}

void GameStatsController::ProvideStatsFromHost(PlayersStats&& _stats)
{
    m_playersStats = std::move(_stats);
}

void GameStatsController::onPlayerJoined(const PlayerInfo& _info)
{
    if (!Game::Get().IsSessionMaster())
        return;

    m_playersStats.insert_or_assign(_info.networkPlayerCopy.GetPlayerId(), PlayerStats{});
}

void GameStatsController::onPlayerLeft(const PlayerInfo& _info)
{
    if (!Game::Get().IsSessionMaster())
        return;
    m_playersStats.erase(_info.networkPlayerCopy.GetPlayerId());
}

void GameStatsController::onGameStart(const WorldConfig&)
{
    if (!Game::Get().IsSessionMaster())
        return;

    for (auto& [id, stats] : m_playersStats)
    {
        stats.numberOfDeathsLastGame = 0;
        stats.numberOfFoundBombsLastGame = 0;
    }
}

void GameStatsController::onGameFinish(const GameResult&)
{
    // TODO perhaps I can use other callbacks(onCharacterToggleFlagCell) to compute stats 
    if (!Game::Get().IsSessionMaster())
        return;

    const auto& cells = Game::Get().GetGameWorld().GetCells();   
    for (auto& cell : cells)
    {
        if (!cell.IsBomb())
            continue;
        if (!cell.IsFlagged())
            continue;

        auto charId = cell.GetCharacterIdWhoFlaggedOrExploded();
        auto it = m_playersStats.find(charId);  // CharacterId and PlayerId should be equal
        if (it == m_playersStats.end())
        {
            LOG_ERROR("No player stats for this id: " + tstr(charId));
            return;
        }
        it->second.numberOfFoundBombsLastGame++;
        it->second.numberOfFoundBombsTotal++;
    }
    const auto& characters = Game::Get().GetGameWorld().GetCharacters();   
    for (auto& character : characters)
    {
        if (!character.IsDead())
            continue;

        auto it = m_playersStats.find(character.GetId());  // CharacterId and PlayerId should be equal
        if (it == m_playersStats.end())
        {
            LOG_ERROR("No player stats for this id: " + tstr(character.GetId()));
            return;
        }
        it->second.numberOfDeathsLastGame++;
        it->second.numberOfDeathsTotal++;
    }
}
