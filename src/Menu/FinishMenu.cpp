#include "FinishMenu.h"
#include "Utils/ResourceManager.h"
#include "Utils/Log.h"
#include "Game.h"

namespace
{
    constexpr float MAIN_TEXT_Y = 300.f;
    constexpr float DESCRIPTION_TEXT_Y = MAIN_TEXT_Y + 44.f;
    constexpr float BUTTON_Y = DESCRIPTION_TEXT_Y + 60.f;

    constexpr int BUTTON_WIDTH = 175;
    constexpr int BUTTON_HEIGHT = 50;
    constexpr int DISTANCE_BETWEEN_BUTTONS = 25;
}

FinishMenu::FinishMenu()
    : BaseMenu(MenuType::FINISH_MENU)
{
    m_background.setTexture(ResourceManager::getTexture("transparent_background2"));
    m_background.setPosition({0.f, 0.f});

    const auto& fontReg = ResourceManager::getFont("poppins_regular");
    const auto& fontBold = ResourceManager::getFont("poppins_bold");
    m_mainText.setFont(fontBold); 
    m_mainText.setString("Waiting..");
    m_mainText.setCharacterSize(FONT_SIZE_1); 
    m_mainText.setFillColor(sf::Color::White);
    m_mainText.setPosition(sf::Vector2f(calculateCenterX(m_mainText.getGlobalBounds().width), MAIN_TEXT_Y));
    m_descriptionText.setFont(fontReg); 
    m_descriptionText.setString("textttt..");
    m_descriptionText.setCharacterSize(FONT_SIZE_3); 
    m_descriptionText.setFillColor(sf::Color::White);
    m_descriptionText.setPosition(sf::Vector2f(calculateCenterX(m_descriptionText.getGlobalBounds().width), DESCRIPTION_TEXT_Y));

    int buttonX = (int)calculateCenterX(2 * BUTTON_WIDTH + DISTANCE_BETWEEN_BUTTONS);
    m_menuItems.emplace_back(std::make_unique<Button>(
        sf::Vector2f(buttonX, BUTTON_Y),
        ResourceManager::getTexture("finish_menu_buttons"),
        sf::IntRect{0,            0, BUTTON_WIDTH, BUTTON_HEIGHT},
        sf::IntRect{BUTTON_WIDTH, 0, BUTTON_WIDTH, BUTTON_HEIGHT},
        [this](){
            LOG("Start Again click ");
            Game::Get().OnFinishMenuStartAgainButtonPressed();
        }
    ));

    m_menuItems.emplace_back(std::make_unique<Button>(
        sf::Vector2f(buttonX + BUTTON_WIDTH + DISTANCE_BETWEEN_BUTTONS, BUTTON_Y),
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
    _window.draw(m_background);
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

    m_mainText.setPosition(sf::Vector2f(calculateCenterX(m_mainText.getGlobalBounds().width), MAIN_TEXT_Y));
    m_descriptionText.setPosition(sf::Vector2f(calculateCenterX(m_descriptionText.getGlobalBounds().width), DESCRIPTION_TEXT_Y));
}

