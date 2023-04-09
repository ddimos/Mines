#include "CreateMenu.h"
#include "Utils/Log.h"
#include "Utils/ResourceManager.h"
#include "Game.h"
#include "Utils/Config.h"

namespace
{
    constexpr int COLUMN_1 = 100;   // TODO Unite with Join Menu and Config Menu. Can be global
    constexpr int COLUMN_2 = 550;
    constexpr int LINE_2_5 = 300;
    constexpr int LINE_3_5 = 350;
    constexpr int LINE_3 = 340;
    constexpr int LINE_4 = 390;
    constexpr int LINE_5 = 580;

    constexpr int CHOOSE_COLOR_BUTTON_SIZE = 35;
    constexpr int DISTANCE_BETWEEN_COLOR_BUTTONS = 14;

    constexpr int START_BUTTON_WIDTH = 200;
    constexpr int START_BUTTON_HEIGHT = 62;
}

CreateMenu::CreateMenu()
    : BaseMenu(MenuType::CREATE_MENU)
{ 
    const auto& font = ResourceManager::getFont("poppins_regular");

    m_enterNameText.setFont(font);
    m_enterNameText.setString("Enter your name");
    m_enterNameText.setCharacterSize(FONT_SIZE_2); 
    m_enterNameText.setFillColor(sf::Color::White);
    m_enterNameText.setPosition(sf::Vector2f(COLUMN_1, LINE_2_5));
    m_chooseColorText.setFont(font);
    m_chooseColorText.setString("Choose your color");
    m_chooseColorText.setCharacterSize(FONT_SIZE_2); 
    m_chooseColorText.setFillColor(sf::Color::White);
    m_chooseColorText.setPosition(sf::Vector2f(COLUMN_2, LINE_2_5));

    m_menuItems.emplace_back(std::make_unique<SetOfChoosableButtons>(
        NUMBER_OF_AVAILABLE_COLORS,
        sf::Vector2f(COLUMN_2, LINE_3_5),
        DISTANCE_BETWEEN_COLOR_BUTTONS,
        ResourceManager::getTexture("colors"),
        sf::Vector2i{
            CHOOSE_COLOR_BUTTON_SIZE,
            CHOOSE_COLOR_BUTTON_SIZE},
        [this](unsigned _buttonNum, bool _isChosen){
            LOG_DEBUG("CHOOSE Color " + tstr(_buttonNum) + " - " + tstr(_isChosen));
            onColorButtonChosen(_buttonNum, _isChosen);
        }
    ));

    m_menuItems.emplace_back(std::make_unique<InputField>(
        sf::Vector2f(COLUMN_1, LINE_3_5),
        ResourceManager::getTexture("field1"),
        ResourceManager::getFont("poppins_regular"),
        "name",
        [](sf::Uint32 _char, const std::string& _enteredText){
            (void)_char;
            (void)_enteredText;
            return true;
        },
        [this](const std::string& _enteredText){
            LOG_DEBUG("Input finish " + _enteredText);
            m_enteredName = _enteredText;
        }
    ));

    m_menuItems.emplace_back(std::make_unique<Button>(
        sf::Vector2f(COLUMN_1, LINE_5),
        ResourceManager::getTexture("create_menu_start_button"),
        sf::IntRect{0,                  0, START_BUTTON_WIDTH, START_BUTTON_HEIGHT},
        sf::IntRect{START_BUTTON_WIDTH, 0, START_BUTTON_WIDTH, START_BUTTON_HEIGHT},
        [this](){
            LOG_DEBUG("Start Game click ");
            MenuInputs inputs;
            inputs.playerName = m_enteredName;
            inputs.playerColorId = static_cast<ColorID>(m_chosenColor);
            if (inputs.playerColorId == ColorIdInvalid)
                    inputs.playerColorId = 1;   // TODO move this code elsewhere?
            Game::Get().OnCreateMenuButtonPressed(inputs);
        }
    ));
}

void CreateMenu::onDraw(sf::RenderWindow& _window)
{
    _window.draw(m_enterNameText);
    _window.draw(m_chooseColorText);
}

void CreateMenu::onColorButtonChosen(unsigned _buttonNum, bool _isChosen)
{
    if (_isChosen)
        m_chosenColor = _buttonNum;
    else
        m_chosenColor = 0;
}