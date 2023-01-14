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

std::unordered_map<std::string, sf::Texture> ResourceManager::ms_textures;
std::unordered_map<std::string, sf::Font> ResourceManager::ms_fonts;
sf::Texture ResourceManager::ms_nullTexture;
sf::Font ResourceManager::ms_nullFont;

void ResourceManager::StartUp()
{
    loadTexture("res/textures/all.png", "tileset");
    loadTexture("res/textures/start_menu_buttons.png", "start_menu_buttons");
    
    loadFont("res/fonts/IBMPlexSansThaiLooped-Regular.ttf", "poppins_regular");
    loadFont("res/fonts/IBMPlexSansThaiLooped-Bold.ttf", "poppins_bold");
}

void ResourceManager::loadTexture(std::string _path, std::string _name)
{
    sf::Texture texture;
    if (!texture.loadFromFile(_path))
    {
        LOG_ERROR("Couldn't load a texture " + _path);
        return;
    }
    ms_textures.insert_or_assign(_name, texture);
}

void ResourceManager::loadFont(std::string _path, std::string _name)
{
    sf::Font font;
    if (!font.loadFromFile(_path))
    {
        LOG_ERROR("Couldn't load a font " + _path);
        return;
    }
    ms_fonts.insert_or_assign(_name, font);
}

const sf::Texture& ResourceManager::getTexture(std::string _name)
{
    auto it = ms_textures.find(_name);
    if (it != ms_textures.end())
        return it->second;

    LOG_ERROR("Couldn't find a texture " + _name);
    return ms_nullTexture;
}

const sf::Font& ResourceManager::getFont(std::string _name)
{
    auto it = ms_fonts.find(_name);
    if (it != ms_fonts.end())
        return it->second;

    LOG_ERROR("Couldn't find a font " + _name);
    return ms_nullFont;
}

