#include "LobbyMenu.h"
#include "Utils.h"
#include "Log.h"
#include "Game.h"

LobbyMenu::LobbyMenu()
    : BaseMenu(MenuType::LOBBY_MENU)
{
    const auto& font = ResourceManager::getFont("poppins_regular");
    m_Text.setFont(font); 
    m_Text.setString("LOBBY!");
    m_Text.setCharacterSize(DEFAULT_TEXT_SIZE); 
    m_Text.setFillColor(sf::Color::White);
    m_Text.setPosition(sf::Vector2f(300, 100));

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
}

void LobbyMenu::onDraw(sf::RenderWindow& _window)
{
    _window.draw(m_Text);
}
