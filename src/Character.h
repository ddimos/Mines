#pragma once
#include "Utils.h"
#include <SFML/Network.hpp>

class Character
{
public:
    Character(bool _isMaster, unsigned _id);
    ~Character();

    void Update(float _dt);
	void Render(sf::RenderWindow& _window);

    WorldPosition GetPos() const { return m_position; }
    void SetPos(WorldPosition _pos) { m_position = _pos; }

    unsigned GetUncoverRadius() const { return m_uncoverRadius; }

    bool IsMaster() const { return m_isMaster; }
    unsigned GetId() const { return m_id; }

    void OnReplicateCharacterPacketReceived(sf::Packet& _packet);
    void OnReplicateUncoverCellPacketReceived(sf::Packet& _packet);
    void OnReplicateToggleFlagCellPacketReceived(sf::Packet& _packet);

private:

    void onCharacterUncoverCell(WorldPosition _pos);
    void onCharacterToggleFlagCell(WorldPosition _pos);

    void replicatePos();
    void replicateUncoverCell();
    void replicateToggleFlagCell();

    const float DECREASE_KOEF = 1.25;
    const float m_cooldown = 0.3f; 
    
    WorldPosition m_position;
    WorldPosition m_prevPosition;
    sf::Vector2f m_padding = {};

    bool m_isLeftWasDown = false;
    bool m_isRightWasDown = false;
    bool m_isUpWasDown = false;
    bool m_isDownWasDown = false;
    float m_cooldownX = m_cooldown; 
    float m_cooldownY = m_cooldown; 
    
    unsigned m_uncoverRadius = 10;

    sf::RectangleShape m_shape;

    bool m_isMaster = true; 
    unsigned m_id = 0;
};
