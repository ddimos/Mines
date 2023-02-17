#pragma once
#include "Utils/Utils.h"
#include "Character.h"
#include "Camera.h"
#include "Cell.h"
#include "WorldMap.h"
#include "Network/NetworkMessage.h"
#include <set>

class GameWorld
{
public:
    GameWorld();
    ~GameWorld() = default;

    void Update(float _dt);
    void Render(sf::RenderWindow& _window);

    void CreateWorld(WorldConfig _worldConfig);
    void SpawnCharacter(bool _spawnMaster, bool _canControl, const CharacterInfo& _info);
    void DestroyWorld();

    const Camera& GetCamera() const { return m_camera; }
    WorldPosition GetWorldSize() const { return m_worldConfig.worldSize; }

    Cell& getCell(int _x, int _y);
    Cell& getCell(WorldPosition _pos);
    const std::vector<Character>& GetCharacters() const { return m_characters; }
    const Character& GetCharacter(CharacterID _id) const;

    void OnCharacterUncoverCell(WorldPosition _pos, Character& _char);
    void OnCharacterToggleFlagCell(WorldPosition _pos, Character& _char);

    void OnReplicateCharacterControlsMessageReceived(NetworkMessage& _message);
    void OnReplicateCharacterMessageReceived(NetworkMessage& _message);
    void OnReplicateUncoverCellMessageReceived(NetworkMessage& _message);
    void OnReplicateToggleFlagCellMessageReceived(NetworkMessage& _message);
    
private:

    void onUncoverCell(WorldPosition _pos);
    void createCells(WorldPosition _worldSize);
    void generateBombs(size_t _bombsNumber);
    std::set<WorldPosition> 
        getPositionsOfNotBombNeighboringCells(WorldPosition _position);
    void uncoverCellsInRadius(WorldPosition _pos, int _radius);
    int getCellIndex(int _x, int _y);

    WorldMap m_worldMap;

    WorldConfig m_worldConfig = {};
    std::vector<Cell> m_cells;
    std::vector<Character> m_characters;
    Character* m_mainCharachter = nullptr;

    Camera m_camera;

    int m_cellsLeftToUncover = 0;
};
