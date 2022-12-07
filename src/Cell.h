#pragma once
#include "Utils.h"

class Character;

class Cell
{
public:
	enum class ValueType
	{
		EMPTY,
		BOMB,
		NUMBER
	};

    enum class State
    {
        COVERED,
        UNCOVERED,
        FLAGGED
    };

	Cell(WorldPosition _position);

	bool IsCovered() const { return m_state == Cell::State::COVERED; }
	bool IsFlagged() const { return m_state == Cell::State::FLAGGED; }
	bool IsNumber() const { return m_type == Cell::ValueType::NUMBER; }
	bool IsEmpty() const { return m_type == Cell::ValueType::EMPTY; }

	WorldPosition GetPosition() const { return m_position; }
	int GetNumber() const { return m_number; }
	State GetState() const { return m_state; }

    void ToggleFlag();

    void OnUncoverCell();
    void OnToggleFlag();

private:
	friend class GameWorld;

	void setType(ValueType _type) { m_type = _type;}
	void increaseNumber() { m_number++; }

	WorldPosition m_position = {};
    State m_state = State::COVERED;
	ValueType m_type = ValueType::EMPTY;
	int m_number = 0;
};