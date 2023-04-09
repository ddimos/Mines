#include "ConfigureMenu.h"
#include "Utils/Log.h"
#include "Utils/ResourceManager.h"
#include "Game.h"
#include "Utils/Config.h"

namespace
{
    constexpr int COLUMN_1 = 100;   // TODO Unite with Join Menu. Can be global
    constexpr int COLUMN_2 = 550;
    constexpr int LINE_1 = 160;
    constexpr int LINE_2 = 210;
    constexpr int LINE_3 = 340;
    constexpr int LINE_4 = 390;
    constexpr int LINE_5 = 580;
    
    constexpr int INPUT_FIELD_2_WIDTH = 100;
    constexpr int INPUT_FIELD_2_HEIGHT = 45;
    constexpr int DISTANCE_BETWEEN_INPUT_FIELDS = INPUT_FIELD_2_WIDTH + 18;
    
    constexpr int CHOOSE_FILED_BUTTON_WIDTH = 100;
    constexpr int CHOOSE_FIELD_BUTTON_HEIGHT = 45;
    constexpr int DISTANCE_BETWEEN_FIELD_BUTTONS = 18;

    constexpr int CHOOSE_MODE_BUTTON_WIDTH = 100;
    constexpr int CHOOSE_MODE_BUTTON_HEIGHT = 45;
    constexpr int DISTANCE_BETWEEN_MODE_BUTTONS = 18;

    constexpr int START_BUTTON_WIDTH = 200;
    constexpr int START_BUTTON_HEIGHT = 62;

    constexpr int X_SPRITE_WIDTH = 9;
    constexpr int X_SPRITE_HEIGHT = 12;
    constexpr int MINE_SPRITE_SIZE = 28;

    bool validateNumber(sf::Uint32 _enteredChar, const std::string& _enteredText, unsigned _maxValue)
    {
        if (_enteredChar >= 48 && _enteredChar <= 57) // Only numbers
        {
            const unsigned enteredValue = std::stoi(_enteredText);
            if (enteredValue == 0 && _enteredChar == 48)
                return false;
            return enteredValue <= _maxValue;
        }
        return false;
    }
}

ConfigureMenu::ConfigureMenu()
    : BaseMenu(MenuType::CONFIGURE_MENU)
{ 
    const auto& font = ResourceManager::getFont("poppins_regular");
    m_chooseFieldSizeText.setFont(font); 
    m_chooseFieldSizeText.setString("Choose field size");
    m_chooseFieldSizeText.setCharacterSize(FONT_SIZE_2); 
    m_chooseFieldSizeText.setFillColor(sf::Color::White);
    m_chooseFieldSizeText.setPosition(sf::Vector2f(COLUMN_1, LINE_1)); 
    m_enterFieldSizeText.setFont(font);
    m_enterFieldSizeText.setString("or make your own");
    m_enterFieldSizeText.setCharacterSize(FONT_SIZE_2); 
    m_enterFieldSizeText.setFillColor(sf::Color::White);
    m_enterFieldSizeText.setPosition(sf::Vector2f(COLUMN_2, LINE_1));
    m_chooseGameModeText.setFont(font);
    m_chooseGameModeText.setString("Choose game mode");
    m_chooseGameModeText.setCharacterSize(FONT_SIZE_2); 
    m_chooseGameModeText.setFillColor(sf::Color::White);
    m_chooseGameModeText.setPosition(sf::Vector2f(COLUMN_1, LINE_3));

    m_menuItems.emplace_back(std::make_unique<SetOfChoosableButtons>(
        3,
        sf::Vector2f(COLUMN_1, LINE_2),
        DISTANCE_BETWEEN_FIELD_BUTTONS,
        ResourceManager::getTexture("choose_size_button"),
        sf::Vector2i{
            CHOOSE_FILED_BUTTON_WIDTH,
            CHOOSE_FIELD_BUTTON_HEIGHT},
        [this](unsigned _buttonNum, bool _isChosen){
            LOG_DEBUG("CHOOSE SIZE " + tstr(_buttonNum) + " - " + tstr(_isChosen));
            onFieldSizeButtonChosen(_buttonNum, _isChosen);
        }
    ));

    m_menuItems.emplace_back(std::make_unique<SetOfChoosableButtons>(
        3,
        sf::Vector2f(COLUMN_1, LINE_4),
        DISTANCE_BETWEEN_MODE_BUTTONS,
        ResourceManager::getTexture("choose_mode_button"),
        sf::Vector2i{
            CHOOSE_MODE_BUTTON_WIDTH,
            CHOOSE_MODE_BUTTON_HEIGHT},
        [this](unsigned _buttonNum, bool _isChosen){
            LOG_DEBUG("CHOOSE mode " + tstr(_buttonNum) + " - " + tstr(_isChosen));
            onGameModeButtonChosen(_buttonNum, _isChosen);
        }
    ));

    const unsigned maxWorldWidth = Config::Get().GetConfig("maxWorldWidth", DEFAULT_MAX_WORLD_WIDTH);
    m_menuItems.emplace_back(std::make_unique<InputField>(
        sf::Vector2f(COLUMN_2, LINE_2),
        ResourceManager::getTexture("field2"),
        ResourceManager::getFont("poppins_regular"),
        "Width",
        [maxWorldWidth](sf::Uint32 _char, const std::string& _enteredText){
            return validateNumber(_char, _enteredText, maxWorldWidth);
        },
        [this](const std::string& _enteredText){
            LOG_DEBUG("Input finish " + _enteredText);
            m_enteredWidth = std::stoi(_enteredText);
        }
    ));
    m_widthInputField = static_cast<InputField*>(m_menuItems.back().get());

    const unsigned maxWorldHeight = Config::Get().GetConfig("maxWorldHeight", DEFAULT_MAX_WORLD_HEIGHT);
    m_menuItems.emplace_back(std::make_unique<InputField>(
        sf::Vector2f(COLUMN_2 + DISTANCE_BETWEEN_INPUT_FIELDS, LINE_2),
        ResourceManager::getTexture("field2"),
        ResourceManager::getFont("poppins_regular"),
        "Height",
        [maxWorldHeight](sf::Uint32 _char, const std::string& _enteredText){
            return validateNumber(_char, _enteredText, maxWorldHeight);
        },
        [this](const std::string& _enteredText){
            LOG_DEBUG("Input finish " + _enteredText);
            m_enteredHeight = std::stoi(_enteredText);
        }
    ));
    m_heightInputField = static_cast<InputField*>(m_menuItems.back().get());

    const unsigned maxWorldBombsCount = Config::Get().GetConfig("maxWorldBombsCount", DEFAULT_MAX_WORLD_BOMBS_COUNT);
    m_menuItems.emplace_back(std::make_unique<InputField>(
        sf::Vector2f(COLUMN_2 + 2 * DISTANCE_BETWEEN_INPUT_FIELDS + MINE_SPRITE_SIZE, LINE_2),
        ResourceManager::getTexture("field2"),
        ResourceManager::getFont("poppins_regular"),
        "333",
        [maxWorldBombsCount, this](sf::Uint32 _char, const std::string& _enteredText){
            unsigned maxPosibleBombs = m_enteredWidth * m_enteredHeight;
            return validateNumber(_char, _enteredText, maxWorldBombsCount > maxPosibleBombs ? maxPosibleBombs : maxWorldBombsCount);
        },
        [this](const std::string& _enteredText){
            LOG_DEBUG("Input finish " + _enteredText);
            m_enteredBombsCount = std::stoi(_enteredText);
        }
    ));
    m_bombsCountInputField = static_cast<InputField*>(m_menuItems.back().get());

    m_menuItems.emplace_back(std::make_unique<Button>(
        sf::Vector2f(COLUMN_1, LINE_5),
        ResourceManager::getTexture("create_menu_start_button"),
        sf::IntRect{0,                  0, START_BUTTON_WIDTH, START_BUTTON_HEIGHT},
        sf::IntRect{START_BUTTON_WIDTH, 0, START_BUTTON_WIDTH, START_BUTTON_HEIGHT},
        [this](){
            LOG_DEBUG("Start Game click ");
            MenuInputs inputs;
            inputs.worldConfig.bombsCount = m_enteredBombsCount;
            inputs.worldConfig.worldSize.x = m_enteredWidth;
            inputs.worldConfig.worldSize.y = m_enteredHeight;
            GameMode mode = GameMode::EASY;
            if (m_enteredGameMode > 0)
                mode = static_cast<GameMode>(m_enteredGameMode - 1);
            inputs.worldConfig.gameMode = mode;

            Game::Get().OnConfigureMenuButtonPressed(inputs);
        }
    ));

    const auto& elementsTexture = ResourceManager::getTexture("create_menu_elements");
    m_spriteX.setTexture(elementsTexture);
    m_spriteX.setTextureRect(sf::IntRect{0, 0, X_SPRITE_WIDTH, X_SPRITE_HEIGHT});
    m_spriteX.setPosition(sf::Vector2f(COLUMN_2 + INPUT_FIELD_2_WIDTH + 4, LINE_2 + 18));

    m_spriteMine.setTexture(elementsTexture);
    m_spriteMine.setTextureRect(sf::IntRect{X_SPRITE_WIDTH, 0, MINE_SPRITE_SIZE, MINE_SPRITE_SIZE});
    m_spriteMine.setPosition(sf::Vector2f(COLUMN_2 + DISTANCE_BETWEEN_INPUT_FIELDS + INPUT_FIELD_2_WIDTH + 9, LINE_2 + 9));
}

void ConfigureMenu::onDraw(sf::RenderWindow& _window)
{
    _window.draw(m_chooseFieldSizeText);
    _window.draw(m_enterFieldSizeText);
    _window.draw(m_chooseGameModeText);
    _window.draw(m_spriteX);
    _window.draw(m_spriteMine);
}

void ConfigureMenu::onFieldSizeButtonChosen(unsigned _buttonNum, bool _isChosen)
{
    WorldConfig worldConfig;
    if (_isChosen)
    {
        if (_buttonNum == 1)
            worldConfig.ApplyDefaultSmallWorld();
        else if (_buttonNum == 2)
            worldConfig.ApplyDefaultMediumWorld();
        else if (_buttonNum == 3)
            worldConfig.ApplyDefaultLargeWorld();
    }
    
    m_widthInputField->Fill(std::to_string(worldConfig.worldSize.x));
    m_heightInputField->Fill(std::to_string(worldConfig.worldSize.y));
    m_bombsCountInputField->Fill(std::to_string(worldConfig.bombsCount));
}

void ConfigureMenu::onGameModeButtonChosen(unsigned _buttonNum, bool _isChosen)
{
    if (_isChosen)
        m_enteredGameMode = _buttonNum;
    else
        m_enteredGameMode = 0;
}