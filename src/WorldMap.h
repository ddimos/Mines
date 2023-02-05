#pragma once
#include "Utils/Utils.h"
#include "Cell.h"

class WorldMap : public sf::Drawable, public sf::Transformable
{
public:
    WorldMap();
    ~WorldMap() = default;

    void CreateMap(WorldPosition _worldSize);

    void OnUncoverCell(const Cell& _cell);
    void OnToggleFlag(const Cell& _cell);

private:

    virtual void draw(sf::RenderTarget& _target, sf::RenderStates _states) const;
    int getTileNumber(const Cell& _cell) const;
    sf::Vector2i getTilePosition(int _tileNumber) const;
    void assignNewTexture(sf::Vertex* _quad, sf::Vector2i _tilePos);
    void setNewTextureToCell(const Cell& _cell);

    const sf::Vector2u TILE_SIZE{32, 32};

    sf::VertexArray m_vertices;
    const sf::Texture& m_tileset;
};

