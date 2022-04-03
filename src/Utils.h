#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

const float CELL_SIZE = 25.f;
const float OFFSET_BETWEEN_CELLS = 5.f;
const sf::Vector2u WINDOWS_SIZE = { 1000, 800 };

const int CELL_COUNT = 50;
const int CAMERA_OFFSET = 2;
const int BOMBS_COUNT = 80;

const float EPSILON = 0.0001f; 

const int InfiniteRadius = -1;

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

    sf::Vector2f getWindowPosition() const
    {
        return sf::Vector2f(
            x * (CELL_SIZE + OFFSET_BETWEEN_CELLS),
            y * (CELL_SIZE + OFFSET_BETWEEN_CELLS));
    }

    // 0-50--55-105--110
    void setFromWindowPosition(const sf::Vector2f& _pos)
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

    int getLength() const
    {
        return static_cast<int>(sqrt(x*x + y*y));
    }
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

int getRand();
