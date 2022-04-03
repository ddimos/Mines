#pragma once
#include "Utils.h"

class Character;

class Camera
{
public:
    Camera();
    ~Camera();

    void Update(float _dt);

    void SetTargetedCharacter(Character* _char) { m_targetedCharacter = _char; }
    const sf::Vector2f& GetPos() const { return m_posInWindowsCoord; }
    const sf::Vector2f& GetSize() const { return m_sizeInWindowsCoord; }

    void OnInit(WorldPosition _worldSize);

private:
    Character*      m_targetedCharacter = nullptr;
    sf::Vector2f    m_posInWindowsCoord = {0.f, 0.f};
    sf::Vector2f    m_worldSizeInWindowsCoord = {0, 0};
    WorldPosition   m_worldSize = {0, 0};
    WorldPosition   m_initialTargetedPos = {0, 0};
    
    bool            m_blockCamera = false;
    sf::Vector2f    m_sizeInWindowsCoord = {1000.f, 1000.f};
    const float     m_speed = 10.f;
};
