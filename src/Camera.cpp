#include "Camera.h"
#include "Game.h"
#include "Utils/Log.h"
#include "Utils/Utils.h"
#include "Character.h"
#include <algorithm>

Camera::Camera()
    : m_sizeInWindowsCoord{(float)GAME_FIELD_SIZE.x, (float)GAME_FIELD_SIZE.y}
{
}

void Camera::OnInit(WorldPosition _worldSize)
{
    m_worldSize = _worldSize;
    m_worldSizeInWindowsCoord = getWindowPosition(_worldSize.x, _worldSize.y);
    WorldPosition size;
    size.setFromWindowPosition(m_sizeInWindowsCoord);
    m_initialTargetedPos.x = (m_worldSizeInWindowsCoord.x >= m_sizeInWindowsCoord.x) 
    ? m_worldSize.x / 2 : size.x / 2;
    m_initialTargetedPos.y = (m_worldSizeInWindowsCoord.y >= m_sizeInWindowsCoord.y) 
    ? m_worldSize.y / 2 : size.y / 2;
    m_posInWindowsCoord = m_initialTargetedPos.getWindowPosition();
}

void Camera::Update(float _dt)
{
    WorldPosition targetedPos = m_initialTargetedPos;
    if (m_targetedCharacter)
        targetedPos = m_targetedCharacter->GetPos();

    if (approximatelyEqual(m_posInWindowsCoord.x, targetedPos.getWindowPosition().x)
     && approximatelyEqual(m_posInWindowsCoord.y, targetedPos.getWindowPosition().y))
        return;

    sf::Vector2f newPos = (targetedPos.getWindowPosition() - m_posInWindowsCoord) * m_speed * _dt + m_posInWindowsCoord;
    m_posInWindowsCoord.x = (m_worldSizeInWindowsCoord.x >= m_sizeInWindowsCoord.x)
        ? std::clamp(newPos.x, 0.f + m_sizeInWindowsCoord.x / 2.f, m_worldSizeInWindowsCoord.x - m_sizeInWindowsCoord.x / 2.f)
        : m_initialTargetedPos.getWindowPosition().x;
    m_posInWindowsCoord.y = (m_worldSizeInWindowsCoord.y >= m_sizeInWindowsCoord.y)
        ? std::clamp(newPos.y, 0.f + m_sizeInWindowsCoord.y / 2.f, m_worldSizeInWindowsCoord.y - m_sizeInWindowsCoord.y / 2.f)
        : m_initialTargetedPos.getWindowPosition().y;
}