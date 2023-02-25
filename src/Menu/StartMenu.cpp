#include "StartMenu.h"
#include "Utils/Log.h"
#include "Utils/ResourceManager.h"
#include "Game.h"

namespace
{
    constexpr int BUTTON_WIDTH = 320;
    constexpr int BUTTON_HEIGHT = 65;
    
    constexpr float WELCOME_TEXT_Y = 230.f;
    constexpr float START_BUTTON_Y = WELCOME_TEXT_Y + 72.f;
    constexpr float JOIN_BUTTON_Y = START_BUTTON_Y + 90.f;

    // texture coordinats
    const sf::IntRect START_BUTTON  {0,            0,             BUTTON_WIDTH, BUTTON_HEIGHT};
    const sf::IntRect START_BUTTON_H{BUTTON_WIDTH, 0,             BUTTON_WIDTH, BUTTON_HEIGHT};
    const sf::IntRect JOIN_BUTTON   {0,            BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT};
    const sf::IntRect JOIN_BUTTON_H {BUTTON_WIDTH, BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT};
}

StartMenu::StartMenu()
    : BaseMenu(MenuType::START_MENU)
{
    // TODO ? create a menu item for text ?
    m_text.setFont(ResourceManager::getFont("poppins_bold")); 
    m_text.setString("WELCOME!");
    m_text.setCharacterSize(FONT_SIZE_1); 
    m_text.setFillColor(sf::Color::White);
    m_text.setPosition(sf::Vector2f(calculateCenterX(m_text.getGlobalBounds().width), WELCOME_TEXT_Y)); 
 
    const auto& texture = ResourceManager::getTexture("start_menu_buttons");

    m_menuItems.emplace_back(std::make_unique<Button>(
            sf::Vector2f(MenuItem::CENTER_ALLIGNED, START_BUTTON_Y),
            texture,
            START_BUTTON,
            START_BUTTON_H,
            [this](){
                LOG_DEBUG(Logger::Type::MENU, "Start Game click ");
                Game::Get().OnStartMenuStartButtonPressed();
            }
        ));

    m_menuItems.emplace_back(std::make_unique<Button>(
            sf::Vector2f(MenuItem::CENTER_ALLIGNED, JOIN_BUTTON_Y),
            texture,
            JOIN_BUTTON,
            JOIN_BUTTON_H,
            [this](){
                LOG_DEBUG(Logger::Type::MENU, "Join Game click ");
                Game::Get().OnStartMenuJoinButtonPressed();
            }
        ));
}

void StartMenu::onDraw(sf::RenderWindow& _window)
{
    _window.draw(m_text);
}