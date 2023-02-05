#include "LobbyMenu.h"
#include "Utils/ResourceManager.h"
#include "Utils/Log.h"
#include "Game.h"

LobbyMenu::LobbyMenu()
    : BaseMenu(MenuType::LOBBY_MENU)
{
    const auto& fontReg = ResourceManager::getFont("poppins_regular");
    const auto& fontBold = ResourceManager::getFont("poppins_bold");
    m_waitingText.setFont(fontBold); 
    m_waitingText.setString("Waiting..");
    m_waitingText.setCharacterSize(DEFAULT_TEXT_SIZE); 
    m_waitingText.setFillColor(sf::Color::White);
    m_waitingText.setPosition(sf::Vector2f(calculateCenterX(m_waitingText.getGlobalBounds().width), 100));
    m_descriptionText.setFont(fontReg); 
    m_descriptionText.setCharacterSize(DEFAULT_TEXT_SIZE); 
    m_descriptionText.setFillColor(sf::Color::White);
    m_descriptionText.setPosition(sf::Vector2f(calculateCenterX(m_descriptionText.getGlobalBounds().width), 150));
}

void LobbyMenu::onActivate()
{
    if (!m_isFirstActivation)
        return;
    m_isFirstActivation = false;

    if (Game::Get().IsSessionMaster())
    {
        m_menuItems.emplace_back(std::make_unique<Button>(
            sf::Vector2f(MenuItem::CENTER_ALLIGNED, 300),
            ResourceManager::getTexture("create_menu_start_button"),    // TODO use a different texture
            sf::IntRect{0,   0, 200, 62},
            sf::IntRect{200, 0, 200, 62},
            [this](){
                LOG("Start Game click ");
                Game::Get().OnLobbyMenuButtonPressed();
            }
        ));

        m_descriptionText.setString("You can start the game");
    }
    else
    {
        m_descriptionText.setString(
            "The game will start once everyone is here");
    }
    m_descriptionText.setPosition(sf::Vector2f(calculateCenterX(m_descriptionText.getGlobalBounds().width), 150));
}

void LobbyMenu::onDraw(sf::RenderWindow& _window)
{
    _window.draw(m_waitingText);
    _window.draw(m_descriptionText);
}
