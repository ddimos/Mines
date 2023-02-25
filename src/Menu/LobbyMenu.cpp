#include "LobbyMenu.h"
#include "Utils/ResourceManager.h"
#include "Utils/Log.h"
#include "Game.h"

namespace
{
    constexpr float WAITING_TEXT_Y = 280.f;
    constexpr float DESCRIPTION_TEXT_Y = WAITING_TEXT_Y + 54.f;
    constexpr float BUTTON_Y = DESCRIPTION_TEXT_Y + 85.f;

    constexpr float OFFSET_FOR_NOT_HOSTS_Y = 40.f;
}
LobbyMenu::LobbyMenu()
    : BaseMenu(MenuType::LOBBY_MENU)
{
    const auto& fontReg = ResourceManager::getFont("poppins_regular");
    const auto& fontBold = ResourceManager::getFont("poppins_bold");
    m_background.setTexture(ResourceManager::getTexture("transparent_background"));
    m_background.setPosition({0.f, 0.f});

    m_waitingText.setFont(fontBold); 
    m_waitingText.setString("Waiting..");
    m_waitingText.setCharacterSize(FONT_SIZE_1); 
    m_waitingText.setFillColor(sf::Color::White);
    m_waitingText.setPosition(sf::Vector2f(calculateCenterX(m_waitingText.getGlobalBounds().width), WAITING_TEXT_Y));
    m_descriptionText.setFont(fontReg); 
    m_descriptionText.setCharacterSize(FONT_SIZE_3); 
    m_descriptionText.setFillColor(sf::Color::White);
    m_descriptionText.setPosition(sf::Vector2f(calculateCenterX(m_descriptionText.getGlobalBounds().width), DESCRIPTION_TEXT_Y));

    m_menuItems.emplace_back(std::make_unique<Button>(
        sf::Vector2f(MenuItem::CENTER_ALLIGNED, BUTTON_Y),
        ResourceManager::getTexture("create_menu_start_button"),
        sf::IntRect{0,   0, 200, 62},
        sf::IntRect{200, 0, 200, 62},
        [this](){
            LOG_DEBUG("Start Game click ");
            Game::Get().OnLobbyMenuButtonPressed();
        }
    ));
    m_button = m_menuItems.back().get();
}

void LobbyMenu::onActivate()
{
    if (Game::Get().IsSessionMaster())
    {
        m_waitingText.setPosition(m_waitingText.getPosition().x, WAITING_TEXT_Y);
        m_descriptionText.setString("You can start the game");
        m_descriptionText.setPosition(sf::Vector2f(calculateCenterX(m_descriptionText.getGlobalBounds().width), DESCRIPTION_TEXT_Y));
    }
    else
    {
        m_button->Deactivate();

        m_waitingText.setPosition(m_waitingText.getPosition().x, WAITING_TEXT_Y + OFFSET_FOR_NOT_HOSTS_Y);
        m_descriptionText.setString(
            "  The game will start\nonce everyone is here");
        m_descriptionText.setPosition(sf::Vector2f(calculateCenterX(m_descriptionText.getGlobalBounds().width), DESCRIPTION_TEXT_Y + OFFSET_FOR_NOT_HOSTS_Y));
    }
}

void LobbyMenu::onDraw(sf::RenderWindow& _window)
{
    _window.draw(m_background);
    _window.draw(m_waitingText);
    _window.draw(m_descriptionText);
}
