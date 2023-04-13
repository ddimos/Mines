#include "FinishMenu.h"
#include "Utils/ResourceManager.h"
#include "Utils/Log.h"
#include "Game.h"
#include "PlayerInfo.h"
#include "GameStats.h"

namespace
{
    constexpr float MAIN_TEXT_Y = 180.f;
    constexpr float DESCRIPTION_TEXT_Y = MAIN_TEXT_Y + 44.f;

    constexpr float STATS_FIRST_ROW_Y = DESCRIPTION_TEXT_Y + 90.f;
    constexpr float STATS_FIRST_COLUMN_X = 200.f;
    constexpr float DISTANCE_BETWEEN_COLUMNS_X = 332.f;
    constexpr float DISTANCE_BETWEEN_ROWS_X = 50.f;
    constexpr float DIED_NUM_OFSET_X = 182.f;
    constexpr float MINES_NUM_OFSET_X = 78.f;

    constexpr float BUTTON_Y = DESCRIPTION_TEXT_Y + 300.f;

    constexpr int BUTTON_WIDTH = 175;
    constexpr int BUTTON_HEIGHT = 50;
    constexpr int DISTANCE_BETWEEN_BUTTONS = 25;

    static const sf::Color DIED_NUM_COLOR(255, 86, 86);
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
            LOG_DEBUG("Start Again click ");
            Game::Get().OnFinishMenuStartAgainButtonPressed();
        }
    ));

    m_menuItems.emplace_back(std::make_unique<Button>(
        sf::Vector2f(buttonX + BUTTON_WIDTH + DISTANCE_BETWEEN_BUTTONS, BUTTON_Y),
        ResourceManager::getTexture("finish_menu_buttons"),
        sf::IntRect{0,            BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT},
        sf::IntRect{BUTTON_WIDTH, BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT},
        [this](){
            LOG_DEBUG("Back To Menu click ");
            Game::Get().OnFinishMenuBackToMenuButtonPressed();
        }
    ));
}

void FinishMenu::onDraw(sf::RenderWindow& _window)
{
    _window.draw(m_background);
    _window.draw(m_mainText);
    _window.draw(m_descriptionText);
    for (const auto& stat : m_stats)
    {
        _window.draw(stat.name);
        _window.draw(stat.died);
        _window.draw(stat.mines);
    }
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
            : (result.loserPtr == nullptr ? "Everyone" : result.loserPtr->networkPlayerCopy.GetName())
                + " stepped on a mine");

    m_mainText.setPosition(sf::Vector2f(calculateCenterX(m_mainText.getGlobalBounds().width), MAIN_TEXT_Y));
    m_descriptionText.setPosition(sf::Vector2f(calculateCenterX(m_descriptionText.getGlobalBounds().width), DESCRIPTION_TEXT_Y));

    const auto& fontBold = ResourceManager::getFont("poppins_bold");
    const auto& fontReg = ResourceManager::getFont("poppins_regular");

    m_stats.clear();

    int x = 0, y = 0;
    for (const auto&[id, stats] : Game::Get().GetStatsController().GetPlayersStats())
    {
        StatsText statsText;
        auto* playerInfo = Game::Get().GetPlayerInfo(id);

        sf::Vector2f pos = sf::Vector2f(
                STATS_FIRST_COLUMN_X + y * DISTANCE_BETWEEN_COLUMNS_X,
                STATS_FIRST_ROW_Y + x * DISTANCE_BETWEEN_ROWS_X);

        statsText.name.setString(playerInfo->networkPlayerCopy.GetName());
        statsText.name.setFont(fontReg); 
        statsText.name.setCharacterSize(FONT_SIZE_4); 
        statsText.name.setFillColor(sf::Color::White);
        statsText.name.setPosition(pos);

        pos.x += DIED_NUM_OFSET_X;

        statsText.died.setString(std::to_string(stats.numberOfDeathsTotal) + "(" + std::to_string(stats.numberOfDeathsLastGame) + ")");
        statsText.died.setFont(fontBold); 
        statsText.died.setCharacterSize(FONT_SIZE_4); 
        statsText.died.setFillColor(DIED_NUM_COLOR);
        statsText.died.setPosition(calculateCenterX(pos.x, statsText.died.getLocalBounds().width), pos.y);

        pos.x += MINES_NUM_OFSET_X;

        statsText.mines.setString(std::to_string(stats.numberOfFoundBombsTotal) + "(" + std::to_string(stats.numberOfFoundBombsLastGame) + ")");
        statsText.mines.setFont(fontBold); 
        statsText.mines.setCharacterSize(FONT_SIZE_4); 
        statsText.mines.setFillColor(sf::Color::White);
        statsText.mines.setPosition(calculateCenterX(pos.x, statsText.mines.getLocalBounds().width), pos.y);

        m_stats.emplace_back(statsText);
        x++;
        if (x >= 4)
        {
            x = 0;
            ++y;
        }
    }
}

