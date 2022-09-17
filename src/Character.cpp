#include "Character.h"
#include "Game.h"
#include "Log.h"
#include "NetworkMessageType.h"
#include "Network/Network.h"

#include <algorithm>

Character::Character(bool _isMaster, unsigned _id, const CharacterInfo& _info)
    :
    m_isMaster(_isMaster),
    m_id(_id),
    m_info(_info)
{
    m_shape.setSize(sf::Vector2f{CELL_SIZE / DECREASE_KOEF, CELL_SIZE / DECREASE_KOEF});
    m_padding = {(CELL_SIZE - CELL_SIZE / DECREASE_KOEF) / 2, (CELL_SIZE - CELL_SIZE / DECREASE_KOEF) / 2};
    sf::Color color(m_info.color.r, m_info.color.g, m_info.color.b, 190);
    m_shape.setFillColor(color);
    m_shape.setOutlineColor(sf::Color::Black);
    m_shape.setOutlineThickness(1.5f);
}

Character::~Character()
{
}

void Character::Update(float _dt)
{
    if (!m_isMaster)
        return;
    
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

    replicatePos();
    // LOG("X: " + tstr(m_position.x) + " Y: " + tstr(m_position.y));

    if (Game::isKeyPressed(sf::Keyboard::Space))
    {
        onCharacterUncoverCell(m_position);
        replicateUncoverCell();
    }
    
    if (Game::isKeyPressed(sf::Keyboard::X))
    {
        onCharacterToggleFlagCell(m_position);   
        replicateToggleFlagCell();
    }

    m_prevPosition = m_position;
}

void Character::Render(sf::RenderWindow& _window)
{
    m_shape.setPosition(m_position.getWindowPosition() + m_padding);
    _window.draw(m_shape);
}

void Character::onCharacterUncoverCell(WorldPosition _pos)
{
    Game::Get().OnCharacterUncoverCell(_pos, *this);
}

void Character::onCharacterToggleFlagCell(WorldPosition _pos)
{
    Game::Get().OnCharacterToggleFlagCell(_pos, *this);
}

void Character::replicatePos()
{
    if (m_prevPosition == m_position)
        return;

    NetworkMessage message(false);
    message.Write(static_cast<sf::Uint16>(NetworkMessageType::REPLICATE_CHARACTER_POS));
    message.Write(m_id);
    message.Write((sf::Int32)m_position.x);
    message.Write((sf::Int32)m_position.y);
    
    Network::Get().Send(message);
}

void Character::replicateUncoverCell()
{
    NetworkMessage message(true);
    message.Write(static_cast<sf::Uint16>(NetworkMessageType::REPLICATE_CHARACTER_UNCOVER));
    message.Write(m_id);
    message.Write((sf::Int32)m_position.x);
    message.Write((sf::Int32)m_position.y);

    Network::Get().Send(message);
}

void Character::replicateToggleFlagCell()
{
    NetworkMessage message(true);
    message.Write(static_cast<sf::Uint16>(NetworkMessageType::REPLICATE_CHARACTER_TOGGLE));
    message.Write(m_id);
    message.Write((sf::Int32)m_position.x);
    message.Write((sf::Int32)m_position.y);

    Network::Get().Send(message);
}

void Character::OnReplicateCharacterMessageReceived(NetworkMessage& _message)
{
    if (m_isMaster)
        return;

    _message.Read(m_position.x);
    _message.Read(m_position.y);
}

void Character::OnReplicateUncoverCellMessageReceived(NetworkMessage& _message)
{
    _message.Read(m_position.x);
    _message.Read(m_position.y);
    onCharacterUncoverCell(m_position);
}

void Character::OnReplicateToggleFlagCellMessageReceived(NetworkMessage& _message)
{
    _message.Read(m_position.x);
    _message.Read(m_position.y);
    onCharacterToggleFlagCell(m_position);
}
