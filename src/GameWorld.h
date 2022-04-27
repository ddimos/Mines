#pragma once
#include "Utils.h"
#include "Character.h"
#include "Camera.h"
#include "Cell.h"
#include <SFML/Network.hpp>

class GameWorld
{
public:
    GameWorld();
    ~GameWorld();

    void Update(float _dt);
    void Render(sf::RenderWindow& _window);

    void CreateWorld(WorldPosition _worldSize, size_t _bombsNumber);
    void SpawnCharacter(bool _spawnMaster, unsigned _id);

    unsigned GenerateId();
    
    const Camera& GetCamera() const { return m_camera; }
    WorldPosition GetWorldSize() const { return m_worldSize; }

    Cell& getCell(int _x, int _y);
    Cell& getCell(WorldPosition _pos);

    void OnPlayerUncoverCell(WorldPosition _pos);
    void OnPlayerToggleFlagCell(WorldPosition _pos);

    void OnSpawnCharacterPacketReceived(sf::Packet& _packet);
    void OnReplicateCharacterPacketReceived(sf::Packet& _packet);
    void OnReplicateUncoverCellPacketReceived(sf::Packet& _packet);
    void OnReplicateToggleFlagCellPacketReceived(sf::Packet& _packet);
    
private:

    void onUncoverCell(WorldPosition _pos);
    void createCells(WorldPosition _worldSize);
    void generateBombs(size_t _bombsNumber);
    std::set<WorldPosition> 
        getPositionsOfNotBombNeighboringCells(WorldPosition _position);
    void uncoverCellsInRadius(WorldPosition _pos, int _radius);



    WorldPosition m_worldSize = {};
    std::vector<Cell> m_cells;
    std::vector<Character> m_characters;
    Character* m_mainCharachter = nullptr;

    Camera m_camera;

    int m_cellsLeftToUncover = 0;
};
