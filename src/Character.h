#pragma once
#include "Utils.h"
#include "Network/NetworkMessage.h"
#include "Network/NetworkPlayer.h"

struct CharacterInfo
{
    sf::Color color = sf::Color::Yellow;
    PlayerID playerId = PlayerIdInvalid;
};

class Character
{
public:
                        // TODO move to the character info?
    Character(bool _isMaster, bool _canControl, unsigned _id, const CharacterInfo& _info);
    ~Character();

    void Update(float _dt);
	void Render(sf::RenderWindow& _window);

    WorldPosition GetPos() const { return m_position; }
    void SetPos(WorldPosition _pos) { m_position = _pos; }

    unsigned GetUncoverRadius() const { return m_uncoverRadius; }

    bool IsMaster() const { return m_isMaster; }
    unsigned GetId() const { return m_id; }
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

    const float DECREASE_KOEF = 1.25;

    WorldPosition m_position;
    WorldPosition m_prevPosition;
    sf::Vector2f m_padding = {};

    Controls m_controls = {};

    unsigned m_uncoverRadius = 10;

    sf::RectangleShape m_shape;

    bool m_isMaster = true; 
    bool m_canControl = false;
    unsigned m_id = 0;

    CharacterInfo m_info;
};
