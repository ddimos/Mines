#include "CreateMenu.h"
#include "Log.h"
#include "Utils.h"
#include "Game.h"

namespace
{
    constexpr int TEXT_SIZE = 24;   // Can be global

    constexpr int COLUMN_1 = 100;
    constexpr int COLUMN_2 = 550;
    constexpr int LINE_1 = 150;
    constexpr int LINE_2 = 200;
    constexpr int LINE_3 = 450;
    constexpr int LINE_4 = 500;
    constexpr int LINE_5 = 600;
    
    constexpr int INPUT_FIELD_2_WIDTH = 100;
    constexpr int INPUT_FIELD_2_HEIGHT = 45;
    constexpr int DISTANCE_BETWEEN_INPUT_FIELDS = INPUT_FIELD_2_WIDTH + 18;
    
    constexpr int CHOOSE_FILED_BUTTON_WIDTH = 100;
    constexpr int CHOOSE_FIELD_BUTTON_HEIGHT = 45;
    constexpr int DISTANCE_BETWEEN_FIELD_BUTTONS = 18 + CHOOSE_FILED_BUTTON_WIDTH;

    constexpr int CHOOSE_COLOR_BUTTON_SIZE = 35;
    constexpr int DISTANCE_BETWEEN_COLOR_BUTTONS = 14 + CHOOSE_COLOR_BUTTON_SIZE;

    constexpr int START_BUTTON_WIDTH = 200;
    constexpr int START_BUTTON_HEIGHT = 62;

    constexpr int X_SPRITE_WIDTH = 9;
    constexpr int X_SPRITE_HEIGHT = 12;
    constexpr int MINE_SPRITE_SIZE = 28;

#define CREATE_SIZE_BUTTON(num)                                                         \
    m_menuItems.emplace_back(std::make_unique<ChoosableButton>(                         \
            sf::Vector2f(COLUMN_1 + num * DISTANCE_BETWEEN_FIELD_BUTTONS, LINE_2),      \
            worldSizeTexture,                                                           \
            sf::IntRect{                                                                \
                num * CHOOSE_FILED_BUTTON_WIDTH,                                        \
                0,                                                                      \
                CHOOSE_FILED_BUTTON_WIDTH,                                              \
                CHOOSE_FIELD_BUTTON_HEIGHT},                                            \
            sf::IntRect{                                                                \
                num * CHOOSE_FILED_BUTTON_WIDTH,                                        \
                CHOOSE_FIELD_BUTTON_HEIGHT,                                             \
                CHOOSE_FILED_BUTTON_WIDTH,                                              \
                CHOOSE_FIELD_BUTTON_HEIGHT},                                            \
            [this](ChoosableButton& _button, bool _isChosen){                           \
                LOG("CHOOSE SIZE BUTTON " + tstr(num) + " - " + tstr(_isChosen));       \
                onFieldSizeButtonChosen(_button, _isChosen, WorldSize::OPTION_##num);   \
            }                                                                           \
        ));                                                                             \

#define CREATE_COLOR_BUTTON(num)                                                        \
    m_menuItems.emplace_back(std::make_unique<ChoosableButton>(                         \
            sf::Vector2f(COLUMN_2 + num * DISTANCE_BETWEEN_COLOR_BUTTONS, LINE_4),      \
            colorButtonsTexture,                                                        \
            sf::IntRect{                                                                \
                num * CHOOSE_COLOR_BUTTON_SIZE,                                         \
                0,                                                                      \
                CHOOSE_COLOR_BUTTON_SIZE,                                               \
                CHOOSE_COLOR_BUTTON_SIZE},                                              \
            sf::IntRect{                                                                \
                num * CHOOSE_COLOR_BUTTON_SIZE,                                         \
                CHOOSE_COLOR_BUTTON_SIZE,                                               \
                CHOOSE_COLOR_BUTTON_SIZE,                                               \
                CHOOSE_COLOR_BUTTON_SIZE},                                              \
            [this](ChoosableButton& _button, bool _isChosen){                           \
                LOG("CHOOSE Color " + tstr(num) + " - " + tstr(_isChosen));             \
                onColorButtonChosen(_button, _isChosen, Color::OPTION_##num);           \
            }                                                                           \
        ));                                                                             \
    

}

CreateMenu::CreateMenu()
    : BaseMenu(MenuType::CREATE_MENU)
{ 
    const auto& worldSizeTexture = ResourceManager::getTexture("choose_size_button");
    const auto& colorButtonsTexture = ResourceManager::getTexture("choose_color_buttons");
    const auto& elementsTexture = ResourceManager::getTexture("create_menu_elements");

    const auto& font = ResourceManager::getFont("poppins_regular");
    m_chooseFieldSizeText.setFont(font); 
    m_chooseFieldSizeText.setString("Choose field size");
    m_chooseFieldSizeText.setCharacterSize(TEXT_SIZE); 
    m_chooseFieldSizeText.setFillColor(sf::Color::White);
    m_chooseFieldSizeText.setPosition(sf::Vector2f(COLUMN_1, LINE_1)); 
    m_enterFieldSizeText.setFont(font);
    m_enterFieldSizeText.setString("or make your own");
    m_enterFieldSizeText.setCharacterSize(TEXT_SIZE); 
    m_enterFieldSizeText.setFillColor(sf::Color::White);
    m_enterFieldSizeText.setPosition(sf::Vector2f(COLUMN_2, LINE_1));
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

    CREATE_SIZE_BUTTON(0)
    CREATE_SIZE_BUTTON(1)
    CREATE_SIZE_BUTTON(2)
    
    CREATE_COLOR_BUTTON(0)
    CREATE_COLOR_BUTTON(1)
    CREATE_COLOR_BUTTON(2)
    CREATE_COLOR_BUTTON(3)
    CREATE_COLOR_BUTTON(4)
    CREATE_COLOR_BUTTON(5)
    CREATE_COLOR_BUTTON(6)
    CREATE_COLOR_BUTTON(7)

    m_menuItems.emplace_back(std::make_unique<InputField>(
        sf::Vector2f(COLUMN_1, LINE_4),
        ResourceManager::getTexture("field1"),
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

    m_menuItems.emplace_back(std::make_unique<InputField>(
        sf::Vector2f(COLUMN_2, LINE_2),
        ResourceManager::getTexture("field2"),
        ResourceManager::getFont("poppins_regular"),
        "Width",
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
        sf::Vector2f(COLUMN_2 + DISTANCE_BETWEEN_INPUT_FIELDS, LINE_2),
        ResourceManager::getTexture("field2"),
        ResourceManager::getFont("poppins_regular"),
        "Height",
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
        sf::Vector2f(COLUMN_2 + 2 * DISTANCE_BETWEEN_INPUT_FIELDS + MINE_SPRITE_SIZE, LINE_2),
        ResourceManager::getTexture("field2"),
        ResourceManager::getFont("poppins_regular"),
        "333",
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
            ResourceManager::getTexture("create_menu_start_button"),
            sf::IntRect{0,                  0, START_BUTTON_WIDTH, START_BUTTON_HEIGHT},
            sf::IntRect{START_BUTTON_WIDTH, 0, START_BUTTON_WIDTH, START_BUTTON_HEIGHT},
            [this](){
                LOG("Start Game click ");
            //    Game::Get().OnCreateButtonPressed();
            }
        ));

    m_spritex.setTexture(elementsTexture);
    m_spriteX.setTextureRect(sf::IntRect{0, 0, X_SPRITE_WIDTH, X_SPRITE_HEIGHT});
    m_spriteX.setPosition(sf::Vector2f(COLUMN_2 + INPUT_FIELD_2_WIDTH + 4, LINE_2 + 18));

    m_spriteMine.setTexture(elementsTexture);
    m_spriteMine.setTextureRect(sf::IntRect{X_SPRITE_WIDTH, 0, MINE_SPRITE_SIZE, MINE_SPRITE_SIZE});
    m_spriteMine.setPosition(sf::Vector2f(COLUMN_2 + DISTANCE_BETWEEN_INPUT_FIELDS + INPUT_FIELD_2_WIDTH + 9, LINE_2 + 9));
}

void CreateMenu::onDraw(sf::RenderWindow& _window)
{
    _window.draw(m_chooseFieldSizeText);
    _window.draw(m_enterFieldSizeText);
    _window.draw(m_enterNameText);
    _window.draw(m_chooseColorText);
    _window.draw(m_spriteX);
    _window.draw(m_spriteMine);
}

void CreateMenu::onFieldSizeButtonChosen(ChoosableButton& _button, bool _isChosen, WorldSize _worldSize)
{
    if (!_isChosen)
    {
        m_chosenFieldSizeButton = nullptr;
        return;
    }
    if (m_chosenFieldSizeButton)
        m_chosenFieldSizeButton->Unchoose();
    m_chosenFieldSizeButton = &_button;
}

void CreateMenu::onColorButtonChosen(ChoosableButton& _button, bool _isChosen, Color _color)
{
    if (!_isChosen)
    {
        m_chosenColorButton = nullptr;
        return;
    }
    if (m_chosenColorButton)
        m_chosenColorButton->Unchoose();
    m_chosenColorButton = &_button;
}