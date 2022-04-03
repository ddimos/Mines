#include "Character.h"
#include "Game.h"
#include "Log.h"
#include <algorithm>

Character::Character(/* args */)
{
    m_shape.setSize(sf::Vector2f{CELL_SIZE / DECREASE_KOEF, CELL_SIZE / DECREASE_KOEF});
    m_padding = {(CELL_SIZE - CELL_SIZE / DECREASE_KOEF) / 2, (CELL_SIZE - CELL_SIZE / DECREASE_KOEF) / 2};
    m_shape.setFillColor(sf::Color::Yellow);
}

Character::~Character()
{
}

void Character::Update(float _dt)
{
    sf::Vector2i deltaPos = {0, 0};
    if (Game::isKeyDown(sf::Keyboard::Left))
    {
        m_cooldownX = (m_isLeftWasDown)
                    ? m_cooldownX - _dt
                    : 0.f;
        if (m_cooldownX <= 0.f)
        {
            deltaPos.x -= 1;
            m_cooldownX = m_cooldown;
        }
        m_isLeftWasDown = true;
    }
    else
        m_isLeftWasDown = false;

    if (Game::isKeyDown(sf::Keyboard::Right))
    {
        m_cooldownX = (m_isRightWasDown)
                    ? m_cooldownX - _dt
                    : 0.f;
        if (m_cooldownX <= 0.f)
        {
            deltaPos.x += 1;
            m_cooldownX = m_cooldown;
        }
        m_isRightWasDown = true;
    }
    else
        m_isRightWasDown = false;
    

    if (Game::isKeyDown(sf::Keyboard::Up))
    {
        m_cooldownY = (m_isUpWasDown)
                    ? m_cooldownY - _dt
                    : 0.f;
        if (m_cooldownY <= 0.f)
        {
            deltaPos.y -= 1;
            m_cooldownY = m_cooldown;
        }
        m_isUpWasDown = true;
    }
    else
        m_isUpWasDown = false;

    if (Game::isKeyDown(sf::Keyboard::Down))
    {
        m_cooldownY = (m_isDownWasDown)
                    ? m_cooldownY - _dt
                    : 0.f;
        if (m_cooldownY <= 0.f)
        {
            deltaPos.y += 1;
            m_cooldownY = m_cooldown;
        }
        m_isDownWasDown = true;
    }
    else
        m_isDownWasDown = false;

    const auto worldSize = Game::Get().GetGameWorld().GetWorldSize();
    m_position.x = std::clamp(m_position.x + deltaPos.x, 0, (int)worldSize.x - 1);
    m_position.y = std::clamp(m_position.y + deltaPos.y, 0, (int)worldSize.y - 1);

    if (Game::isKeyPressed(sf::Keyboard::Space))
    {
        Game::Get().OnPlayerUncoverCell(m_position);
    }

    if (Game::isKeyPressed(sf::Keyboard::Z))
    {
        Game::Get().OnPlayerToggleFlagCell(m_position);
    }
}

void Character::Render(sf::RenderWindow& _window)
{
    m_shape.setPosition(m_position.getWindowPosition() + m_padding);
    _window.draw(m_shape);
}