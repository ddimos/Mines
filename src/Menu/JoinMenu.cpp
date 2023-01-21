#include "JoinMenu.h"
#include "Log.h"
#include "Utils.h"
#include "Game.h"

namespace
{
    constexpr int TEXT_SIZE = 24;   // TODO Can be global

    constexpr int COLUMN_1 = 100;   // TODO Unite with Create Menu. Can be global
    constexpr int COLUMN_2 = 550;
    constexpr int LINE_1 = 150;
    constexpr int LINE_2 = 200;
    constexpr int LINE_3 = 450;
    constexpr int LINE_4 = 500;
    constexpr int LINE_5 = 600;

    constexpr int JOIN_BUTTON_WIDTH = 200;
    constexpr int JOIN_BUTTON_HEIGHT = 62;

    constexpr int CHOOSE_COLOR_BUTTON_SIZE = 35;
    constexpr int DISTANCE_BETWEEN_COLOR_BUTTONS = 14; 
}

JoinMenu::JoinMenu()
    : BaseMenu(MenuType::JOIN_MENU)
{

    const auto& font = ResourceManager::getFont("poppins_regular");
    m_enterHostAddressText.setFont(font); 
    m_enterHostAddressText.setString("Enter the host address");
    m_enterHostAddressText.setCharacterSize(TEXT_SIZE); 
    m_enterHostAddressText.setFillColor(sf::Color::White);
    m_enterHostAddressText.setPosition(sf::Vector2f(COLUMN_1, LINE_1)); 
    m_enterNameText.setFont(font);
    m_enterNameText.setString("Enter your name");
    m_enterNameText.setCharacterSize(TEXT_SIZE); 
    m_enterNameText.setFillColor(sf::Color::White);
    m_enterNameText.setPosition(sf::Vector2f(COLUMN_1, LINE_3));
    m_chooseColorText.setFont(font);
    m_chooseColorText.setString("Choose your color");
    m_chooseColorText.setCharacterSize(TEXT_SIZE); 
    m_chooseColorText.setFillColor(sf::Color::White);
    m_chooseColorText.setPosition(sf::Vector2f(COLUMN_2, LINE_3));

    m_menuItems.emplace_back(std::make_unique<SetOfChoosableButtons>(
        8,
        sf::Vector2f(COLUMN_2, LINE_4),
        DISTANCE_BETWEEN_COLOR_BUTTONS,
        ResourceManager::getTexture("choose_color_buttons"),
        sf::Vector2i{
            CHOOSE_COLOR_BUTTON_SIZE,
            CHOOSE_COLOR_BUTTON_SIZE},
        [this](unsigned _buttonNum, bool _isChosen){
            LOG("CHOOSE Color " + tstr(_buttonNum) + " - " + tstr(_isChosen));
            onColorButtonChosen(_buttonNum, _isChosen);
        }
    ));

    m_menuItems.emplace_back(std::make_unique<InputField>(
        sf::Vector2f(COLUMN_1, LINE_2),
        ResourceManager::getTexture("field1"),
        ResourceManager::getFont("poppins_regular"),
        "help text xxx.xxx.xxx",
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

    m_menuItems.emplace_back(std::make_unique<InputField>(
        sf::Vector2f(COLUMN_1, LINE_4),
        ResourceManager::getTexture("field1"),
        ResourceManager::getFont("poppins_regular"),
        "help text ",
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

    m_menuItems.emplace_back(std::make_unique<Button>(
            sf::Vector2f(COLUMN_1, LINE_5),
            ResourceManager::getTexture("join_menu_join_button"),
            sf::IntRect{0,                 0, JOIN_BUTTON_WIDTH, JOIN_BUTTON_HEIGHT},
            sf::IntRect{JOIN_BUTTON_WIDTH, 0, JOIN_BUTTON_WIDTH, JOIN_BUTTON_HEIGHT},
            [this](){
                LOG("Join Game click ");
            //    Game::Get().OnJoinButtonPressed();
            }
        ));
}

void JoinMenu::onDraw(sf::RenderWindow& _window)
{
    _window.draw(m_enterHostAddressText);
    _window.draw(m_enterNameText);
    _window.draw(m_chooseColorText);
}

void JoinMenu::onColorButtonChosen(unsigned _buttonNum, bool _isChosen)
{
    (void)_buttonNum;
    (void)_isChosen;
}