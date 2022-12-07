#include "Utils.h"
#include "Log.h"


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

void provideSeed(unsigned _seed)
{
    srand(_seed);
}

int getRand()
{
    return rand();
}

Profiler::Profiler(const std::string& _context)
    : m_context(_context)
{
    m_context.append(" ");
    m_t = std::chrono::high_resolution_clock::now();
}

Profiler::~Profiler()
{
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_ms = t2 - m_t;

    LOG(m_context + tstr(duration_ms.count()));
}