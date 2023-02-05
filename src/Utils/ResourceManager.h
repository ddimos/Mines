#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>

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
