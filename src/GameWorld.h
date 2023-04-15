#pragma once
#include "Utils/Utils.h"
#include "Character.h"
#include "Camera.h"
#include "Cell.h"
#include "WorldMap.h"
#include "Random.h"
#include "Network/NetworkMessage.h"
#include <set>

class GameWorld
{
public:
    using Cells = std::vector<Cell>;
    using Characters = std::vector<Character>;

    GameWorld();
    ~GameWorld() = default;

    void Update(float _dt);
    void Render(sf::RenderWindow& _window);

    void CreateWorld(WorldConfig _worldConfig);
    void SpawnCharacter(bool _spawnMaster, bool _canControl, const CharacterInfo& _info);
    void DestroyWorld();

    const Camera& GetCamera() const { return m_camera; }
    WorldPosition GetWorldSize() const { return m_worldConfig.worldSize; }
    GameMode GetGameMode() const { return m_worldConfig.gameMode; }

    Cell& getCell(int _x, int _y);
    Cell& getCell(WorldPosition _pos);
    const Cells& GetCells() const { return m_cells; }
    const Characters& GetCharacters() const { return m_characters; }
    const Character& GetCharacter(CharacterID _id) const;

    void OnCharacterUncoverCell(WorldPosition _pos, Character& _char);
    void OnCharacterToggleFlagCell(WorldPosition _pos, Character& _char);

    void OnReplicateCharacterControlsMessageReceived(NetworkMessage& _message);
    void OnReplicateCharacterMessageReceived(NetworkMessage& _message);
    void OnReplicateUncoverCellMessageReceived(NetworkMessage& _message);
    void OnReplicateToggleFlagCellMessageReceived(NetworkMessage& _message);
    
private:

    void onUncoverCell(WorldPosition _pos, const Character& _char);
    void createCells(WorldPosition _worldSize);
    void generateBombs(size_t _bombsNumber);
    std::set<WorldPosition> 
        getPositionsOfNotBombNeighboringCells(WorldPosition _position);
    void uncoverCellsInRadius(WorldPosition _pos, const Character& _char);
    int getCellIndex(int _x, int _y);
    bool areThereAlivePlayers() const;

    WorldMap m_worldMap;
    Random m_randomizer;

    WorldConfig m_worldConfig = {};
    Cells m_cells;
    Characters m_characters;
    Character* m_mainCharachter = nullptr;

    Camera m_camera;

    int m_cellsLeftToUncover = 0;
};
