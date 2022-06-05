#include "Cell.h"
#include "Game.h"
#include "Character.h"

Cell::Cell(WorldPosition _position)
	: m_position{_position}
{
    m_shape.setPosition(_position.getWindowPosition());
    m_shape.setFillColor(sf::Color::Green);
    m_shape.setSize(sf::Vector2f{CELL_SIZE, CELL_SIZE});

    const float padding = (CELL_SIZE / 2.f - CELL_SIZE / RADIUS_DECREASE_KOEF);

    m_flagShape.setPosition(_position.getWindowPosition() + sf::Vector2f{padding, padding});
    m_flagShape.setFillColor(sf::Color::Red);
    m_flagShape.setRadius(CELL_SIZE / RADIUS_DECREASE_KOEF);


    m_text.setPosition(_position.getWindowPosition());
    m_text.setFont(Game::Get().GetFont());
    m_text.setCharacterSize(24); 
    m_text.setFillColor(sf::Color::Black);
}


void Cell::ToggleFlag()
{
    if (m_state == State::UNCOVERED)
        return;

    m_state = (m_state == State::COVERED) ? State::FLAGGED : State::COVERED;
}

void Cell::OnUncoverCell()
{
    if (m_state == State::FLAGGED)
        return;
    
    m_state = Cell::State::UNCOVERED;
    if (m_type != ValueType::BOMB)
    {
        if (m_type == ValueType::NUMBER)
            m_shape.setFillColor(sf::Color::Blue);
        else 
            m_shape.setFillColor(sf::Color::Cyan);
    }
    else
    {
        m_shape.setFillColor(sf::Color::Red);
    }

    if (m_type == ValueType::NUMBER) // better do it on init
        m_text.setString(std::to_string(m_number));
}


void Cell::OnToggleFlag(Character& _char)
{
    if (m_state == State::UNCOVERED)
        return;
    
    m_state = (m_state == State::FLAGGED) ? State::COVERED : State::FLAGGED;
    m_flagShape.setFillColor(_char.GetInfo().color);
}

void Cell::Update(float _dt)
{
    (void)_dt;
}

void Cell::Render(sf::RenderWindow& _window)
{
    _window.draw(m_shape);

    if (m_state == State::UNCOVERED && m_type != ValueType::EMPTY)
        _window.draw(m_text);
    if (m_state == State::FLAGGED)
        _window.draw(m_flagShape);
}