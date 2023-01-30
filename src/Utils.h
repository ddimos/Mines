#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.h"
#include <cmath>
#include <chrono>
#include <string>
#include <unordered_map>

inline int getIndex(int _x, int _y)
{
    return _x + _y * CELL_COUNT;    // outdated
}

// ---------------------------------------------------------

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

class ResourceManager
{
public:
    static void StartUp();

    static const sf::Texture& getTexture(std::string _name);
    static const sf::Font& getFont(std::string _name);

private:
    static void loadTexture(std::string _path, std::string _name);
    static void loadFont(std::string _path, std::string _name);

    static std::unordered_map<std::string, sf::Texture> ms_textures;
    static std::unordered_map<std::string, sf::Font> ms_fonts;

    static sf::Texture ms_nullTexture;
    static sf::Font ms_nullFont;
};

// ---------------------------------------------------------

struct WorldConfig
{
    static WorldConfig GetSmallWorld() { return WorldConfig{{10, 10}, 10};}
    static WorldConfig GetMediumWorld() { return WorldConfig{{25, 25}, 30};}
    static WorldConfig GetLargeWorld() { return WorldConfig{{50, 50}, 100};}

    WorldPosition worldSize = {};
    unsigned bombsCount = 0;
};
