#pragma once
#include "Utils/Utils.h"
#include "Network/NetworkMessage.h"
#include "Network/NetworkPlayer.h"

struct CharacterInfo
{
    ColorID colorId = ColorIdInvalid;
    PlayerID playerId = PlayerIdInvalid;
    CharacterID characterId = CharacterIdInvalid;
};

class Character
{
public:
    Character(bool _isMaster, bool _canControl, const CharacterInfo& _info);

    void Update(float _dt);
	void Render(sf::RenderWindow& _window);

    WorldPosition GetPos() const { return m_position; }
    void SetPos(WorldPosition _pos) { m_position = _pos; }

    unsigned GetUncoverRadius() const { return m_uncoverRadius; }

    bool IsMaster() const { return m_isMaster; }
    CharacterID GetId() const { return m_info.characterId; }
    const CharacterInfo& GetInfo() const { return m_info; }

    void OnReplicateCharacterControlsMessageReceived(NetworkMessage& _message);
    void OnReplicateCharacterMessageReceived(NetworkMessage& _message);
    void OnReplicateUncoverCellMessageReceived(NetworkMessage& _message);
    void OnReplicateToggleFlagCellMessageReceived(NetworkMessage& _message);

private:

    struct Controls
    {
        Controls(const Controls&) = default;
        bool isLeftPressed = false;
        bool isRightPressed = false;
        bool isUpPressed = false;
        bool isDownPressed = false;
        bool isSpacePressed = false;
        bool isXPressed = false;

        void reset();
        bool hasValue() const;
    };

    void onCharacterUncoverCell(WorldPosition _pos);
    void onCharacterToggleFlagCell(WorldPosition _pos);

    void readControls();
    void updateStateFromControls();
    void replicateControls();
    void replicatePos();
    void replicateUncoverCell();
    void replicateToggleFlagCell();

    WorldPosition m_position;
    WorldPosition m_prevPosition;

    Controls m_controls = {};

    unsigned m_uncoverRadius = 10;

    sf::Sprite m_sprite;

    bool m_isMaster = true; 
    bool m_canControl = false;

    CharacterInfo m_info;
};
