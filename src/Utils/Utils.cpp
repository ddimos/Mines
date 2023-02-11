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

void provideSeed(unsigned _seed)
{
    srand(_seed);
}

int getRand()
{
    return rand();
}

float calculateCenterX(float _width)
{
    return Game::Get().GetWindow().getSize().x / 2.f - _width / 2.f;
}

// ---------------------------------------------------------

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

// ---------------------------------------------------------


sf::Color getColorById(ColorID _colorId)
{
    // TODO to load it from config?
    switch (_colorId)
    {
    case 1:
        return sf::Color(174, 248, 148); 
    case 2:
        return sf::Color(96, 249, 166); 
    case 3:
        return sf::Color(130, 244, 244);
    case 4:
        return sf::Color(106, 210, 255); 
    case 5:
        return sf::Color(114, 162, 255); 
    case 6:
        return sf::Color(129, 127, 255); 
    case 7:
        return sf::Color(215, 131, 255); 
    case 8:
        return sf::Color(255, 129, 220); 
    default:
        LOG_ERROR("This color id is not supported " + tstr(_colorId));
        return sf::Color::Yellow; 
        break;
    }
}