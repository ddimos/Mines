#include "WorldMap.h"
#include "GameWorld.h"
#include "Utils/Log.h"
#include "Utils/Utils.h"
#include "Utils/ResourceManager.h"

WorldMap::WorldMap(const GameWorld& _gameWorld)
    : m_gameWorld(_gameWorld)
    , m_tileset(ResourceManager::getTexture("tileset"))
{
}

void WorldMap::OnUncoverCell(const Cell& _cell)
{
    setNewTextureToCell(_cell);
}

void WorldMap::OnToggleFlag(const Cell& _cell)
{
    setNewTextureToCell(_cell);
}

void WorldMap::CreateMap(WorldPosition _worldSize)
{
    m_worldSize = _worldSize;
    m_vertices.setPrimitiveType(sf::Quads);
    m_vertices.resize(_worldSize.x * _worldSize.y * 4);

    for (int i = 0; i < _worldSize.x; ++i)
    {
        for (int j = 0; j < _worldSize.y; ++j)
        {
            const int tilePos = i + j * _worldSize.x;
            const int tileNumber = 0;   // a generated cell is always covered

            sf::Vertex* quad = &m_vertices[tilePos * 4];
            sf::Vector2f windPos = WorldPosition{i, j}.getWindowPosition();

            quad[0].position = sf::Vector2f(windPos.x,              windPos.y);
            quad[1].position = sf::Vector2f(windPos.x + CELL_SIZE,  windPos.y);
            quad[2].position = sf::Vector2f(windPos.x + CELL_SIZE,  windPos.y + CELL_SIZE);
            quad[3].position = sf::Vector2f(windPos.x,              windPos.y + CELL_SIZE);
    
            assignNewTexture(quad, getTilePosition(tileNumber));
        }
    }
}

int WorldMap::getTileNumber(const Cell& _cell) const
{
    if (_cell.IsCovered())
        return 0;
    if (_cell.IsFlagged())
    {
        const auto& ch = m_gameWorld.GetCharacter(_cell.GetCharacterIdWhoFlaggedOrExploded());
        return ch.GetInfo().colorId + 10; // 10 is an offset 
    }
    if (_cell.IsEmpty())
        return 1;
    if (_cell.IsNumber())
        return _cell.GetNumber() + 1;   // 1 is an offset 
    
    // The cell has a mine

    if (m_gameWorld.GetGameMode() == GameMode::NORMAL)
    {
        const auto& ch = m_gameWorld.GetCharacter(_cell.GetCharacterIdWhoFlaggedOrExploded());
        return ch.GetInfo().colorId + 18; // 18 is an offset 
    }
    
    return 10;  // just a mine
}

sf::Vector2i WorldMap::getTilePosition(int _tileNumber) const
{
    int tu =_tileNumber % (m_tileset.getSize().x / TILE_SIZE.x);
    int tv =_tileNumber / (m_tileset.getSize().x / TILE_SIZE.x);
    return sf::Vector2i{ tu, tv };
}

void WorldMap::assignNewTexture(sf::Vertex* _quad, sf::Vector2i _tilePos)
{
    _quad[0].texCoords = sf::Vector2f( _tilePos.x      * TILE_SIZE.x,  _tilePos.y      * TILE_SIZE.y);
    _quad[1].texCoords = sf::Vector2f((_tilePos.x + 1) * TILE_SIZE.x,  _tilePos.y      * TILE_SIZE.y);
    _quad[2].texCoords = sf::Vector2f((_tilePos.x + 1) * TILE_SIZE.x, (_tilePos.y + 1) * TILE_SIZE.y);
    _quad[3].texCoords = sf::Vector2f( _tilePos.x      * TILE_SIZE.x, (_tilePos.y + 1) * TILE_SIZE.y);
}

void WorldMap::setNewTextureToCell(const Cell& _cell)
{
    int index = getCellIndex(_cell);
    sf::Vertex* quad = &m_vertices[index * 4];
    assignNewTexture(quad, getTilePosition(getTileNumber(_cell)));
}

int WorldMap::getCellIndex(const Cell& _cell)
{
    return _cell.GetPosition().x + _cell.GetPosition().y * (m_worldSize.x);
}

void WorldMap::draw(sf::RenderTarget& _target, sf::RenderStates _states) const
{
    _states.transform *= getTransform();
    _states.texture = &m_tileset;
    _target.draw(m_vertices, _states);
}