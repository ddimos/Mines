#include "Cell.h"

Cell::Cell(WorldPosition _position)
	: m_position{_position}
{
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
}

void Cell::OnToggleFlag()
{
    if (m_state == State::UNCOVERED)
        return;
    
    m_state = (m_state == State::FLAGGED) ? State::COVERED : State::FLAGGED;
}
