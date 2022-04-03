#pragma once
#include "Utils.h"

class Character
{
public:
    Character();
    ~Character();

    void Update(float _dt);
	void Render(sf::RenderWindow& _window);

    WorldPosition GetPos() const { return m_position; }
    void SetPos(WorldPosition _pos) { m_position = _pos; }

    unsigned GetUncoverRadius() const { return m_uncoverRadius; }

private:

    const float DECREASE_KOEF = 1.25;
    const float m_cooldown = 0.3f; 
    
    WorldPosition m_position;
    sf::Vector2f m_padding = {};

    bool m_isLeftWasDown = false;
    bool m_isRightWasDown = false;
    bool m_isUpWasDown = false;
    bool m_isDownWasDown = false;
    float m_cooldownX = m_cooldown; 
    float m_cooldownY = m_cooldown; 
    
    unsigned m_uncoverRadius = 10;

    sf::RectangleShape m_shape;
};
