#include "Cell.h"
#include "Character.h"

Cell::Cell(WorldPosition _position)
	: m_position{_position}
{
}

void Cell::ToggleFlag(const Character& _char)
{
    if (m_state == State::UNCOVERED)
        return;

    m_state = (m_state == State::COVERED) ? State::FLAGGED : State::COVERED;
    m_flaggedCharId = _char.GetId();
}

void Cell::Uncover()
{
    if (m_state == State::FLAGGED)
        return;

    m_state = Cell::State::UNCOVERED;
}
