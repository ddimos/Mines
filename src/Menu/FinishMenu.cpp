#include "FinishMenu.h"
#include "Utils/ResourceManager.h"
#include "Utils/Log.h"
#include "Game.h"

namespace
{
    constexpr int BUTTON_WIDTH = 200;
    constexpr int BUTTON_HEIGHT = 50;
}

FinishMenu::FinishMenu()
    : BaseMenu(MenuType::FINISH_MENU)
{
    const auto& fontReg = ResourceManager::getFont("poppins_regular");
    const auto& fontBold = ResourceManager::getFont("poppins_bold");
    m_mainText.setFont(fontBold); 
    m_mainText.setString("Waiting..");
    m_mainText.setCharacterSize(DEFAULT_TEXT_SIZE); 
    m_mainText.setFillColor(sf::Color::White);
    m_mainText.setPosition(sf::Vector2f(calculateCenterX(m_mainText.getGlobalBounds().width), 100));
    m_descriptionText.setFont(fontReg); 
    m_descriptionText.setString("textttt..");
    m_descriptionText.setCharacterSize(DEFAULT_TEXT_SIZE); 
    m_descriptionText.setFillColor(sf::Color::White);
    m_descriptionText.setPosition(sf::Vector2f(calculateCenterX(m_descriptionText.getGlobalBounds().width), 150));

    m_menuItems.emplace_back(std::make_unique<Button>(
        sf::Vector2f(300, 300),
        ResourceManager::getTexture("finish_menu_buttons"),
        sf::IntRect{0,            0, BUTTON_WIDTH, BUTTON_HEIGHT},
        sf::IntRect{BUTTON_WIDTH, 0, BUTTON_WIDTH, BUTTON_HEIGHT},
        [this](){
            LOG("Start Again click ");
            Game::Get().OnFinishMenuStartAgainButtonPressed();
        }
    ));

    m_menuItems.emplace_back(std::make_unique<Button>(
        sf::Vector2f(500, 300),
        ResourceManager::getTexture("finish_menu_buttons"),
        sf::IntRect{0,            BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT},
        sf::IntRect{BUTTON_WIDTH, BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT},
        [this](){
            LOG("Back To Menu click ");
            Game::Get().OnFinishMenuBackToMenuButtonPressed();
        }
    ));
}

void FinishMenu::onDraw(sf::RenderWindow& _window)
{
    _window.draw(m_mainText);
    _window.draw(m_descriptionText);
}

void FinishMenu::onActivate()
{
    const auto& result = Game::Get().GetGameResult();
    m_mainText.setString(
        result.isVictory
            ? "Congratulations!"
            : "Oops!");

    m_descriptionText.setString(
        result.isVictory
            ? "You found all the mines"
            : result.loserName + " stepped on a mine");

    m_mainText.setPosition(sf::Vector2f(calculateCenterX(m_mainText.getGlobalBounds().width), 100));
    m_descriptionText.setPosition(sf::Vector2f(calculateCenterX(m_descriptionText.getGlobalBounds().width), 150));
}

