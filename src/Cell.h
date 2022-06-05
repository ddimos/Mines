#pragma once
#include "Utils.h"
#include <set>

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
	bool IsNumber() const { return m_type == Cell::ValueType::NUMBER; }
	bool IsEmpty() const { return m_type == Cell::ValueType::EMPTY; }

	void SetValue(const std::string& _str)	{ m_text.setString(_str); }
	void SetType(ValueType _type) { m_type = _type;}

	void IncreaseNumber() { m_number++; }

    void ToggleFlag();
	void Update(float _dt);
	void Render(sf::RenderWindow& _window);

    void OnUncoverCell();
    void OnToggleFlag(Character& _char);

//private

	const float RADIUS_DECREASE_KOEF = 3.f;

	WorldPosition m_position;
	sf::RectangleShape m_shape;//(sf::Vector2f(120.f, 50.f));
	sf::CircleShape m_flagShape;
	sf::Text m_text;

    State m_state = State::COVERED;
	ValueType m_type = ValueType::EMPTY;
	int m_number = 0;
};