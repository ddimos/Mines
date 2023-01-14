#include "StartMenu.h"
#include "Log.h"
#include "Utils.h"

namespace
{
    constexpr int BUTTON_WIDTH = 320;
    constexpr int BUTTON_HEIGHT = 65;
    
    constexpr float WELCOME_TEXT_Y = 180.f;
    constexpr float START_BUTTON_Y = 250.f;
    constexpr float JOIN_BUTTON_Y = 350.f;

    constexpr unsigned WELCOME_TEXT_SIZE = 35;

    // texture coordinats
    const sf::IntRect START_BUTTON  {0,            0,             BUTTON_WIDTH, BUTTON_HEIGHT};
    const sf::IntRect START_BUTTON_H{BUTTON_WIDTH, 0,             BUTTON_WIDTH, BUTTON_HEIGHT};
    const sf::IntRect JOIN_BUTTON   {0,            BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT};
    const sf::IntRect JOIN_BUTTON_H {BUTTON_WIDTH, BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT};
}

StartMenu::StartMenu()
    : BaseMenu(MenuType::START_MENU)
{
    m_text.setFont(ResourceManager::getFont("poppins_bold")); 
    m_text.setString("WELCOME!");
    m_text.setCharacterSize(WELCOME_TEXT_SIZE); 
    m_text.setFillColor(sf::Color::White);
    m_text.setPosition(sf::Vector2f(calculateCenterX(m_text.getGlobalBounds().width), WELCOME_TEXT_Y)); 
 
    const auto& texture = ResourceManager::getTexture("start_menu_buttons");
    m_startGameButton.setTexture(texture);
    m_startGameButton.setTextureRect(START_BUTTON);
    m_startGameButton.setPosition(sf::Vector2f(calculateCenterX(m_startGameButton.getGlobalBounds().width), START_BUTTON_Y)); 
    m_joinGameButton.setTexture(texture);
    m_joinGameButton.setTextureRect(JOIN_BUTTON);
    m_joinGameButton.setPosition(sf::Vector2f(calculateCenterX(m_joinGameButton.getGlobalBounds().width), JOIN_BUTTON_Y));
 
    m_buttons.emplace_back(Button{m_startGameButton.getGlobalBounds(),
     [this](bool _isHovered){
        LOG("Start Game hover " + tstr(_isHovered));
        if (_isHovered)
            m_startGameButton.setTextureRect(START_BUTTON_H);
        else
            m_startGameButton.setTextureRect(START_BUTTON);
     },
     [this](bool _isClicked){
        LOG("Start Game click " + tstr(_isClicked));
     }});
    m_buttons.emplace_back(Button{m_joinGameButton.getGlobalBounds(),
     [this](bool _isHovered){
        LOG("Join Game hover " + tstr(_isHovered));
        if (_isHovered)
            m_joinGameButton.setTextureRect(JOIN_BUTTON_H);
        else
            m_joinGameButton.setTextureRect(JOIN_BUTTON);
     },
     [this](bool _isClicked){
        LOG("Join Game click " + tstr(_isClicked));
     }});
}

void StartMenu::onDraw(sf::RenderWindow& _window)
{
    
    _window.draw(m_text);
    _window.draw(m_startGameButton);
    _window.draw(m_joinGameButton);
}