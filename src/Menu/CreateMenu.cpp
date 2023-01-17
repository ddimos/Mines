#include "CreateMenu.h"
#include "Log.h"
#include "Utils.h"
#include "Game.h"

CreateMenu::CreateMenu()
    : BaseMenu(MenuType::CREATE_MENU)
{
    m_text.setFont(ResourceManager::getFont("poppins_bold")); 
    m_text.setString("WELCOME!");
    //m_text.setCharacterSize(WELCOME_TEXT_SIZE); 
    m_text.setFillColor(sf::Color::White);
    //m_text.setPosition(sf::Vector2f(calculateCenterX(m_text.getGlobalBounds().width), WELCOME_TEXT_Y)); 
 
    //const auto& texture = ResourceManager::getTexture("start_menu_buttons");

    m_menuItems.emplace_back(std::make_unique<InputField>(
        sf::Vector2f(MenuItem::CENTER_ALLIGNED, 450),
        ResourceManager::getTexture("input"),
        ResourceManager::getFont("poppins_regular"),
        "help text",
        [](sf::Uint32 _char){
            std::string a;
            a+= _char;
            LOG("Input " + a);
            return true;
        },
        [this](const std::string& _enteredText){
            LOG("Input finish " + _enteredText);
            m_enteredStr = _enteredText;
        }
    ));
}

void CreateMenu::onDraw(sf::RenderWindow& _window)
{

}