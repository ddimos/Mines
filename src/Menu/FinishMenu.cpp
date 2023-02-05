#include "FinishMenu.h"
#include "Utils/ResourceManager.h"
#include "Utils/Log.h"
#include "Game.h"

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
        ResourceManager::getTexture("create_menu_start_button"),    // TODO use a different texture
        sf::IntRect{0,   0, 200, 62},
        sf::IntRect{200, 0, 200, 62},
        [this](){
            LOG("Start Again click ");
            Game::Get().OnFinishMenuStartAgainButtonPressed();
        }
    ));

    m_menuItems.emplace_back(std::make_unique<Button>(
        sf::Vector2f(500, 300),
        ResourceManager::getTexture("create_menu_start_button"),    // TODO use a different texture
        sf::IntRect{0,   0, 200, 62},
        sf::IntRect{200, 0, 200, 62},
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
    bool isVictory = Game::Get().IsVictory(); // GetGameResult() , should have isVictory, player who has steped
    m_mainText.setString(
        isVictory
            ? "Congratulations!"
            : "Oops!");

    m_descriptionText.setString(
        isVictory
            ? "You found all the mines"
            : "// stepped on a mine");

    m_mainText.setPosition(sf::Vector2f(calculateCenterX(m_mainText.getGlobalBounds().width), 100));
    m_descriptionText.setPosition(sf::Vector2f(calculateCenterX(m_descriptionText.getGlobalBounds().width), 150));
}

