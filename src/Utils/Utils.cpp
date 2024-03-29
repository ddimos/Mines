#include "Utils/Utils.h"
#include "Utils/Log.h"
#include "Game.h"


sf::Vector2f WorldPosition::getWindowPosition() const
{
    return sf::Vector2f(
        x * (CELL_SIZE + OFFSET_BETWEEN_CELLS),
        y * (CELL_SIZE + OFFSET_BETWEEN_CELLS));
}

void WorldPosition::setFromWindowPosition(const sf::Vector2f& _pos)
{
    if (_pos.x == 0.f)
        x = 0;
    else
        x = floor(_pos.x / (CELL_SIZE + OFFSET_BETWEEN_CELLS));

    if (_pos.y == 0.f)
        y = 0;
    else
        y = floor(_pos.y / (CELL_SIZE + OFFSET_BETWEEN_CELLS));
}

int WorldPosition::getLength() const
{
    return static_cast<int>(sqrt(x*x + y*y));
}

float calculateCenterX(float _width)
{
    return calculateCenterX(Game::Get().GetWindow().getSize().x / 2.f, _width);
}

float calculateCenterX(float _x, float _width)
{
    return _x - _width / 2.f;
}

// ---------------------------------------------------------

unsigned getTextureNumByColorId(ColorID _colorId)
{
    if (_colorId == ColorIdInvalid || _colorId > NUMBER_OF_AVAILABLE_COLORS)
    {
        LOG_ERROR("This color id is not supported " + tstr(_colorId));
        return 0;
    }
    return _colorId - 1;
}
