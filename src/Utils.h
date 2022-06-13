#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include "Constants.h"

inline int getIndex(int _x, int _y)
{
    return _x + _y * CELL_COUNT;
}

inline sf::Vector2f getWindowPosition(int _x, int _y)
{
    return sf::Vector2f(
        _x * (CELL_SIZE + OFFSET_BETWEEN_CELLS),
        _y * (CELL_SIZE + OFFSET_BETWEEN_CELLS));
}

struct WorldPosition
{
    int x = 0;
    int y = 0;

    sf::Vector2f getWindowPosition() const;
    void setFromWindowPosition(const sf::Vector2f& _pos); // 0-50--55-105--110

    int getLength() const;
};

inline bool operator==(const WorldPosition& _lhs, const WorldPosition& _rhs)
{
    return _lhs.x == _rhs.x && _lhs.y == _rhs.y;
}
inline bool operator!=(const WorldPosition& _lhs, const WorldPosition& _rhs)
{
    return !(_lhs == _rhs);
}
inline bool operator>(const WorldPosition& _lhs, const WorldPosition& _rhs)
{
    return getIndex(_lhs.x, _lhs.y) > getIndex(_rhs.x, _rhs.y);
}
inline bool operator<(const WorldPosition& _lhs, const WorldPosition& _rhs)
{
    return getIndex(_lhs.x, _lhs.y) < getIndex(_rhs.x, _rhs.y);
}
inline WorldPosition operator-(const WorldPosition& _lhs, const WorldPosition& _rhs)
{
    return WorldPosition{_lhs.x - _rhs.x, _lhs.y - _rhs.y};
}

inline bool approximatelyEqual(float _a, float _b)
{
    return fabs(_a - _b) <= std::max(fabs(_a), fabs(_b)) * EPSILON;
}

void provideSeed(unsigned _seed);
int getRand();
