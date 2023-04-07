#include "Character.h"
#include "Game.h"
#include "Utils/Log.h"
#include "Utils/ResourceManager.h"
#include "NetworkMessageType.h"
#include "Network/Network.h"

#include <algorithm>

Character::Character(GameWorld& _gameWorldRef, bool _isMaster, bool _canControl, const CharacterInfo& _info)
    :
    m_gameWorldRef(_gameWorldRef),
    m_isMaster(_isMaster),
    m_canControl(_canControl),
    m_info(_info)
{
    m_sprite.setTexture(ResourceManager::getTexture("players"));
    m_sprite.setTextureRect(
        sf::IntRect(
            CELL_SIZE * getTextureNumByColorId(_info.colorId),
            0,
            CELL_SIZE,
            CELL_SIZE));
}

void Character::Update(float _dt)
{
    (void)_dt;
    if (!m_isMaster && !m_canControl)
        return;

    if (m_canControl)
        readControls();
    updatePosFromControls();

    if (m_isDead)
        return;

    updateStateFromControls();

    if (!m_isMaster)
    {
        replicateControls();
        return;
    }
    
    replicatePos();

    if (m_controls.isSpacePressed)
        replicateUncoverCell();
    
    if (m_controls.isXPressed)
        replicateToggleFlagCell();

    m_controls.reset();
    m_prevPosition = m_position;
}

void Character::Render(sf::RenderWindow& _window)
{
    m_sprite.setPosition(m_position.getWindowPosition());
    _window.draw(m_sprite);
}

void Character::onCharacterUncoverCell(WorldPosition _pos)
{
    m_gameWorldRef.OnCharacterUncoverCell(_pos, *this);
}

void Character::onCharacterToggleFlagCell(WorldPosition _pos)
{
    m_gameWorldRef.OnCharacterToggleFlagCell(_pos, *this);
}

void Character::readControls()
{
    m_controls.reset();
    m_controls.isLeftPressed = Game::isKeyPressed(sf::Keyboard::Left);
    m_controls.isRightPressed = Game::isKeyPressed(sf::Keyboard::Right);
    m_controls.isUpPressed = Game::isKeyPressed(sf::Keyboard::Up);
    m_controls.isDownPressed = Game::isKeyPressed(sf::Keyboard::Down);
    m_controls.isSpacePressed = Game::isKeyPressed(sf::Keyboard::Space);
    m_controls.isXPressed = Game::isKeyPressed(sf::Keyboard::F);
}

void Character::updateStateFromControls()
{
    if (m_controls.isSpacePressed)
        onCharacterUncoverCell(m_position);
    if (m_controls.isXPressed)
        onCharacterToggleFlagCell(m_position);   
}

void Character::updatePosFromControls()
{
    sf::Vector2i deltaPos = {0, 0};
    if (m_controls.isLeftPressed)
        deltaPos.x -= 1;
    if (m_controls.isRightPressed)
        deltaPos.x += 1;
    if (m_controls.isUpPressed)
        deltaPos.y -= 1;
    if (m_controls.isDownPressed)
        deltaPos.y += 1;

    const auto worldSize = Game::Get().GetGameWorld().GetWorldSize();
    m_position.x = std::clamp(m_position.x + deltaPos.x, 0, (int)worldSize.x - 1);
    m_position.y = std::clamp(m_position.y + deltaPos.y, 0, (int)worldSize.y - 1);
}

void Character::replicateControls()
{
    if (!m_controls.hasValue())
        return;
    
    NetworkMessage message(Network::Get().GetHostPeerId(), true); // TODO to split movement and other more important inputs?
    message.Write(static_cast<sf::Uint16>(NetworkMessageType::REPLICATE_CHARACTER_CONTROLS));
    message.Write((CharacterID)m_info.characterId);
    message.Write(m_controls.isLeftPressed);
    message.Write(m_controls.isRightPressed);
    message.Write(m_controls.isUpPressed);
    message.Write(m_controls.isDownPressed);
    message.Write(m_controls.isSpacePressed);
    message.Write(m_controls.isXPressed);

    Network::Get().Send(message);
}

void Character::replicatePos()
{
    if (m_prevPosition == m_position)
        return;

    NetworkMessage message(false);
    message.Write(static_cast<sf::Uint16>(NetworkMessageType::REPLICATE_CHARACTER_POS));
    message.Write((CharacterID)m_info.characterId);
    message.Write((sf::Int32)m_position.x);
    message.Write((sf::Int32)m_position.y);
    
    Network::Get().Send(message);
}

void Character::replicateUncoverCell()
{
    NetworkMessage message(true);
    message.Write(static_cast<sf::Uint16>(NetworkMessageType::REPLICATE_CHARACTER_UNCOVER));
    message.Write((CharacterID)m_info.characterId);
    message.Write((sf::Int32)m_position.x);
    message.Write((sf::Int32)m_position.y);

    Network::Get().Send(message);
}

void Character::replicateToggleFlagCell()
{
    NetworkMessage message(true);
    message.Write(static_cast<sf::Uint16>(NetworkMessageType::REPLICATE_CHARACTER_TOGGLE));
    message.Write((CharacterID)m_info.characterId);
    message.Write((sf::Int32)m_position.x);
    message.Write((sf::Int32)m_position.y);

    Network::Get().Send(message);
}

void Character::OnReplicateCharacterControlsMessageReceived(NetworkMessage& _message)
{
    _message.Read(m_controls.isLeftPressed);
    _message.Read(m_controls.isRightPressed);
    _message.Read(m_controls.isUpPressed);
    _message.Read(m_controls.isDownPressed);
    _message.Read(m_controls.isSpacePressed);
    _message.Read(m_controls.isXPressed);
}

void Character::OnReplicateCharacterMessageReceived(NetworkMessage& _message)
{
    if (m_canControl)   // TODO don't send it to this player at all
        return;

    _message.Read(m_position.x);
    _message.Read(m_position.y);
}

void Character::OnReplicateUncoverCellMessageReceived(NetworkMessage& _message)
{
    if (m_canControl)   // TODO don't send it to this player at all
        return;

    _message.Read(m_position.x);
    _message.Read(m_position.y);
    onCharacterUncoverCell(m_position);
}

void Character::OnReplicateToggleFlagCellMessageReceived(NetworkMessage& _message)
{
    if (m_canControl)   // TODO don't send it to this player at all
        return;

    _message.Read(m_position.x);
    _message.Read(m_position.y);
    onCharacterToggleFlagCell(m_position);
}

void Character::OnCharacterDie()
{
    m_isDead = true;
}

void Character::Controls::reset()
{
    isLeftPressed = false;
    isRightPressed = false;
    isUpPressed = false;
    isDownPressed = false;
    isSpacePressed = false;
    isXPressed = false;
}

bool Character::Controls::hasValue() const
{
    return isLeftPressed || isRightPressed || isUpPressed
        || isDownPressed || isSpacePressed || isXPressed;
}