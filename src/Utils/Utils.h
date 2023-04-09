#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.h"
#include <cmath>
#include <chrono>
#include <string>


inline sf::Vector2f getWindowPosition(int _x, int _y)
{
    return sf::Vector2f(
        _x * (CELL_SIZE + OFFSET_BETWEEN_CELLS),
        _y * (CELL_SIZE + OFFSET_BETWEEN_CELLS));
}

// ---------------------------------------------------------

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
inline WorldPosition operator-(const WorldPosition& _lhs, const WorldPosition& _rhs)
{
    return WorldPosition{_lhs.x - _rhs.x, _lhs.y - _rhs.y};
}

// ---------------------------------------------------------

inline bool approximatelyEqual(float _a, float _b)
{
    return fabs(_a - _b) <= std::max(fabs(_a), fabs(_b)) * EPSILON;
}

// ---------------------------------------------------------

void provideSeed(unsigned _seed);
int getRand();

// ---------------------------------------------------------

float calculateCenterX(float _width);

// ---------------------------------------------------------

class Profiler
{
public:
    Profiler(const std::string& _context = "");
    ~Profiler();

private:
    std::string m_context = "";
    std::chrono::_V2::system_clock::time_point m_t = {};
};

#define PROFILE_POINT(context) Profiler p(context);

// ---------------------------------------------------------

enum class GameMode
{
    EASY,
    NORMAL,
    HARD
};

struct WorldConfig
{
    void ApplyDefaultSmallWorld()  { worldSize={10, 10}; bombsCount = 10;}
    void ApplyDefaultMediumWorld() { worldSize={25, 25}; bombsCount = 30;}
    void ApplyDefaultLargeWorld()  { worldSize={50, 50}; bombsCount = 100;}

    bool IsValid() const { return worldSize.x != 0 && worldSize.y != 0 && bombsCount != 0; }
    
    WorldPosition worldSize = {};
    unsigned bombsCount = 0;
    GameMode gameMode = GameMode::EASY;
};

// ---------------------------------------------------------

using ColorID = sf::Uint8;
const ColorID ColorIdInvalid = 0; 

unsigned getTextureNumByColorId(ColorID _colorId);

// ---------------------------------------------------------

using CharacterID = sf::Uint16;
const CharacterID CharacterIdInvalid = 65535; 
