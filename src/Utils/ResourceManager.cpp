#include "Utils/ResourceManager.h"
#include "Utils/Log.h"

std::unordered_map<std::string, sf::Texture> ResourceManager::ms_textures;
std::unordered_map<std::string, sf::Font> ResourceManager::ms_fonts;
sf::Texture ResourceManager::ms_nullTexture;
sf::Font ResourceManager::ms_nullFont;

void ResourceManager::StartUp()
{
    loadTexture("res/textures/background.png", "background");
    loadTexture("res/textures/transparent_background.png", "transparent_background");
    loadTexture("res/textures/transparent_background2.png", "transparent_background2");
    loadTexture("res/textures/tileset.png", "tileset");
    loadTexture("res/textures/players.png", "players");
    loadTexture("res/textures/start_menu_buttons.png", "start_menu_buttons");
    loadTexture("res/textures/choose_color_buttons.png", "choose_color_buttons");
    loadTexture("res/textures/choose_size_button.png", "choose_size_button");
    loadTexture("res/textures/create_menu_start_button.png", "create_menu_start_button");
    loadTexture("res/textures/join_menu_join_button.png", "join_menu_join_button");
    loadTexture("res/textures/field1.png", "field1");
    loadTexture("res/textures/field2.png", "field2");
    loadTexture("res/textures/create_menu_elements.png", "create_menu_elements");
    loadTexture("res/textures/finish_menu_buttons.png", "finish_menu_buttons");
    loadTexture("res/textures/info_panel.png", "info_panel");
 
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
